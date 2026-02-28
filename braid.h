//
// Subharmonic-Harmonic Braid
//
// Five oscillators at f/2, f, 2f, 3f, 4f, coupled together
// with Kuramoto-style phase nudging. The result sits somewhere
// between a bass growl and a shimmering overtone halo,
// depending on how tightly you couple them.
//
// The Kuramoto model comes from physics — it describes how
// fireflies synchronize, how neurons phase-lock, how coupled
// pendulums find common rhythm. Each oscillator feels a pull
// toward its neighbors' phase. The coupling strength K controls
// everything:
//
//   K ≈ 0   → oscillators run free, drifting in and out of
//              phase. Rich beating patterns, almost chaotic.
//   K ≈ 0.4 → partial sync. They're aware of each other but
//              not enslaved. This is the sweet spot — alive
//              without being locked.
//   K ≈ 1   → full phase lock. Perfect harmonic series.
//              Mathematically clean. Musically boring.
//
// The interesting sounds live in partial coupling, where the
// system isn't just producing harmonics but *interacting*
// across frequency scales. The sub-octave growl modulates
// the shimmer. The shimmer colors the growl. Neither one
// dominates — they braid together.
//
// Frequency tracking uses zero-crossing detection, borrowed
// from pll.h. Works well for single notes. Feed it a chord
// and it'll track whichever fundamental wins the zero-crossing
// race. Don't think about it too hard.
//

#define BRAID_N_OSC 5

static struct {
	float coupling;
	float sub_level;
	float brightness;
	float blend;

	// Frequency tracking via zero-crossing
	float amplitude;
	float decay;
	int samples_since_cross;
	int is_high;
	float smoothed_freq;
	struct biquad track_lpf;

	// The five oscillators
	struct lfo_state osc[BRAID_N_OSC];
	float freq_ratios[BRAID_N_OSC];

	// We track phase separately from the LFO state because
	// the Kuramoto correction needs to read and nudge phases
	// across oscillators. Redundant? Yes. Simple? Also yes.
	float phase[BRAID_N_OSC];

	// Tone shaping — warmth on the bottom, air on top
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

	braid.decay = (float) pow(0.5, 40.0 / SAMPLES_PER_SEC);

	// LPF on the input for zero-crossing — keep it tracking
	// fundamentals, not harmonics
	biquad_lpf(&braid.track_lpf, 1000.0f, 0.707f);

	// The harmonic series
	braid.freq_ratios[0] = 0.5f;	// sub-octave
	braid.freq_ratios[1] = 1.0f;	// fundamental
	braid.freq_ratios[2] = 2.0f;	// octave
	braid.freq_ratios[3] = 3.0f;	// twelfth
	braid.freq_ratios[4] = 4.0f;	// double octave

	// Start at A2 — reasonable for guitar
	braid.smoothed_freq = 110.0f;
	for (int i = 0; i < BRAID_N_OSC; i++) {
		set_lfo_freq(&braid.osc[i], braid.smoothed_freq * braid.freq_ratios[i]);
		braid.phase[i] = (float)i / BRAID_N_OSC;
	}

	// Sub gets rounded off. Upper harmonics get let through.
	biquad_lpf(&braid.sub_lpf, 300.0f, 0.707f);
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

		// Guitar fundamentals live roughly here
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

	//
	// The Kuramoto step: each oscillator is nudged toward
	// its neighbors on a ring topology.
	//
	//   dθ_i/dt = ω_i + K * Σ sin(θ_j - θ_i)
	//
	// The sin(Δθ) term is the key — it's zero when phases
	// match (locked), maximal at 90° (maximum pull), and
	// reverses past 180° (pushes away). This creates a
	// natural basin of attraction without hard constraints.
	//
	// The 0.001 scaling keeps the correction gentle. At
	// 48kHz sample rate, even small nudges accumulate fast.
	//
	for (int i = 0; i < BRAID_N_OSC; i++) {
		float target_freq = freq * braid.freq_ratios[i];
		if (target_freq < 20.0f) target_freq = 20.0f;
		if (target_freq > 16000.0f) target_freq = 16000.0f;

		set_lfo_freq(&braid.osc[i], target_freq);

		float correction = 0;
		if (i > 0)
			correction += sinf(braid.phase[i-1] * 2 * M_PI - braid.phase[i] * 2 * M_PI);
		if (i < BRAID_N_OSC - 1)
			correction += sinf(braid.phase[i+1] * 2 * M_PI - braid.phase[i] * 2 * M_PI);

		braid.phase[i] += K * correction * 0.001f;
		braid.phase[i] -= floorf(braid.phase[i]);
	}

	// Step all oscillators
	float osc_out[BRAID_N_OSC];
	for (int i = 0; i < BRAID_N_OSC; i++) {
		osc_out[i] = lfo_step(&braid.osc[i], lfo_sinewave);
		braid.phase[i] = u32_to_fraction(braid.osc[i].idx);
	}

	// Mix with input envelope — the oscillators sing,
	// but only as loud as the guitar is playing
	float sub = osc_out[0] * amplitude * braid.sub_level;
	sub = biquad_step(&braid.sub_lpf, sub);

	float fund = osc_out[1] * amplitude * 0.3f;

	float bright = (osc_out[2] * 0.5f + osc_out[3] * 0.3f + osc_out[4] * 0.2f);
	bright *= amplitude * braid.brightness;
	bright = biquad_step(&braid.bright_hpf, bright);

	float wet = limit_value(sub + fund + bright);

	return linear(braid.blend, in, wet);
}
