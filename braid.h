//
// Subharmonic-Harmonic Braid
//
// Generate partials at f/2, f, 2f, 3f, 4f from the input pitch,
// then cross-modulate them with Kuramoto-style phase coupling.
// The result is a bass growl with shimmering overtone halo.
//
// The Kuramoto model is a cute physics trick: each oscillator
// nudges its phase toward its neighbors. With weak coupling you
// get interesting beating patterns. With strong coupling they
// lock into a perfect chord. The sweet spot is somewhere in
// between where it's alive but not chaotic.
//
// Frequency tracking is borrowed from pll.h's zero-crossing
// approach. It's not great for polyphonic input but for a
// single guitar note it works well enough. Feed it a chord
// and it'll track... something. Probably the loudest note.
// Or not. Who knows.
//

#define BRAID_N_OSC 5

static struct {
	float coupling;
	float sub_level;
	float brightness;
	float blend;

	// Frequency tracking (stolen from pll.h, simplified)
	float amplitude;
	float decay;
	int samples_since_cross;
	int is_high;
	float smoothed_freq;
	struct biquad track_lpf;

	// The five oscillators: f/2, f, 2f, 3f, 4f
	struct lfo_state osc[BRAID_N_OSC];
	float freq_ratios[BRAID_N_OSC];

	// Kuramoto phase corrections accumulator
	// (we can't read phase directly from lfo_state easily,
	//  so we maintain our own phase tracking. Yes, this means
	//  we're essentially running parallel phase accumulators.
	//  It's redundant but simple.)
	float phase[BRAID_N_OSC];

	// Tone shaping
	struct biquad sub_lpf;
	struct biquad bright_hpf;
} braid;

static inline void braid_describe(float pot[4])
{
	fprintf(stderr, " coupling=%g", pot[0]);
	fprintf(stderr, " sub=%g", pot[1]);
	fprintf(stderr, " brightness=%g", pot[2]);
	fprintf(stderr, " blend=%g\n", pot[3]);
}

static inline void braid_init(float pot[4])
{
	braid.coupling = pot[0];
	braid.sub_level = pot[1];
	braid.brightness = pot[2];
	braid.blend = pot[3];

	// Amplitude tracking decay — same as pll.h
	braid.decay = (float) pow(0.5, 40.0 / SAMPLES_PER_SEC);

	// Input filter for zero-crossing — cut above 1kHz to avoid
	// tracking harmonics instead of fundamental
	biquad_lpf(&braid.track_lpf, 1000.0f, 0.707f);

	// Frequency ratios for the five partials
	braid.freq_ratios[0] = 0.5f;	// sub-octave
	braid.freq_ratios[1] = 1.0f;	// fundamental
	braid.freq_ratios[2] = 2.0f;	// octave up
	braid.freq_ratios[3] = 3.0f;	// fifth above that
	braid.freq_ratios[4] = 4.0f;	// two octaves up

	// Initial frequency guess — A2, reasonable for guitar
	braid.smoothed_freq = 110.0f;
	for (int i = 0; i < BRAID_N_OSC; i++) {
		set_lfo_freq(&braid.osc[i], braid.smoothed_freq * braid.freq_ratios[i]);
		braid.phase[i] = (float)i / BRAID_N_OSC;  // spread initial phases
	}

	// Sub gets a LPF to keep it warm and round
	biquad_lpf(&braid.sub_lpf, 300.0f, 0.707f);

	// Upper harmonics get a HPF to keep them airy
	biquad_hpf(&braid.bright_hpf, 800.0f, 0.707f);
}

static inline float braid_track_amplitude(float in)
{
	float a = fabsf(in);
	if (a < braid.amplitude)
		a = linear(braid.decay, a, braid.amplitude);
	braid.amplitude = a;
	return a;
}

static inline void braid_track_frequency(float in, float amplitude)
{
	float clean = biquad_step(&braid.track_lpf, in);

	braid.samples_since_cross++;

	float threshold = amplitude * 0.1f;
	if (threshold < 0.0001f)
		threshold = 0.0001f;

	if (!braid.is_high && clean > threshold) {
		braid.is_high = 1;

		float freq = SAMPLES_PER_SEC / braid.samples_since_cross;

		// Sanity check — guitar fundamental range roughly 80Hz-1200Hz
		if (freq > 40.0f && freq < 2000.0f)
			braid.smoothed_freq = linear(0.1f, braid.smoothed_freq, freq);

		braid.samples_since_cross = 0;
	} else if (braid.is_high && clean < -threshold) {
		braid.is_high = 0;
	}
}

static inline float braid_step(float in)
{
	float amplitude = braid_track_amplitude(in);
	braid_track_frequency(in, amplitude);

	float K = braid.coupling;
	float freq = braid.smoothed_freq;

	// Update oscillator frequencies and apply Kuramoto coupling
	for (int i = 0; i < BRAID_N_OSC; i++) {
		float target_freq = freq * braid.freq_ratios[i];
		if (target_freq < 20.0f) target_freq = 20.0f;
		if (target_freq > 16000.0f) target_freq = 16000.0f;

		set_lfo_freq(&braid.osc[i], target_freq);

		// Kuramoto coupling: nudge phase toward neighbors
		// Each oscillator couples to its immediate neighbors
		// (with wrapping, so it's a ring topology)
		float correction = 0;
		if (i > 0)
			correction += sinf(braid.phase[i-1] * 2 * M_PI - braid.phase[i] * 2 * M_PI);
		if (i < BRAID_N_OSC - 1)
			correction += sinf(braid.phase[i+1] * 2 * M_PI - braid.phase[i] * 2 * M_PI);

		// Scale coupling and apply — the 0.001 keeps it from
		// going nuts at high K values
		float nudge = K * correction * 0.001f;
		braid.phase[i] += nudge;

		// Keep phase in [0, 1)
		braid.phase[i] -= floorf(braid.phase[i]);
	}

	// Step all oscillators and collect output
	float osc_out[BRAID_N_OSC];
	for (int i = 0; i < BRAID_N_OSC; i++) {
		osc_out[i] = lfo_step(&braid.osc[i], lfo_sinewave);
		// Track phase from LFO index (it's a 32-bit counter, 0 = 0.0, max = 1.0)
		braid.phase[i] = u32_to_fraction(braid.osc[i].idx);
	}

	// Mix the partials with amplitude envelope
	// Sub-octave (f/2) — filtered warm
	float sub = osc_out[0] * amplitude * braid.sub_level;
	sub = biquad_step(&braid.sub_lpf, sub);

	// Fundamental — always present at low level for coherence
	float fund = osc_out[1] * amplitude * 0.3f;

	// Upper harmonics — filtered bright
	float bright = (osc_out[2] * 0.5f + osc_out[3] * 0.3f + osc_out[4] * 0.2f);
	bright *= amplitude * braid.brightness;
	bright = biquad_step(&braid.bright_hpf, bright);

	float wet = limit_value(sub + fund + bright);

	return linear(braid.blend, in, wet);
}
