//
// Formant-preserving pitch expansion ("Giant Whisper")
//
// What happens when you separate a signal into what it says
// (the envelope) and how fast it says it (the carrier)?
// You get to change perceived size without the cartoon pitch
// shift. A bass guitar that sounds like it's being played
// inside a thimble, or a ukulele with the resonance of a
// cathedral.
//
// The trick is a Hilbert transform — a 90° phase shift that
// gives you the "imaginary" part of your signal. Together with
// the original, you have an analytic signal: amplitude and
// instantaneous phase, cleanly separated.
//
// A proper Hilbert transform needs an FFT. We don't do FFTs
// here — single sample in, single sample out, zero latency.
// So instead we fake it with cascaded allpass filters tuned
// to maintain roughly 90° separation across the guitar range.
// Below 100Hz and above 10kHz it gets wobbly. For everything
// in between, it's honestly pretty decent.
//
// The deeper idea: this is what happens when you look at a
// signal from a perpendicular angle. Same information,
// completely different structure revealed. The envelope was
// always there — you just couldn't see it without rotating
// your perspective by exactly 90°.
//
// Phase unwrapping is sample-by-sample with no lookahead,
// so hard transients will glitch. Call it character.
//

static struct {
	float pitch_ratio;
	float env_smooth;
	float blend;
	float formant_strength;

	// Two parallel allpass chains form the Hilbert pair.
	// Chain I matches the group delay of chain Q.
	// Chain Q provides the ~90° shift.
	// 4 stages each for reasonable bandwidth.
	struct biquad ap_i[4];
	struct biquad ap_q[4];

	float prev_phase;
	float out_phase;
	float envelope;
	float prev_in;
} formant;

static inline void formant_describe(float pot[4])
{
	fprintf(stderr, " pitch=%.2fx", linear(pot[0], 0.5, 2.0));
	fprintf(stderr, " env_smooth=%g", pot[1]);
	fprintf(stderr, " blend=%g", pot[2]);
	fprintf(stderr, " formant=%g\n", pot[3]);
}

static inline void formant_init(float pot[4])
{
	formant.pitch_ratio = linear(pot[0], 0.5, 2.0);
	formant.env_smooth = pot[1];
	formant.blend = pot[2];
	formant.formant_strength = pot[3];

	//
	// Allpass frequencies hand-tuned for coverage across
	// ~100Hz to ~10kHz. The I chain and Q chain use offset
	// center frequencies so their phase responses differ
	// by approximately 90° across that band.
	//
	// This is the part where a real DSP engineer would use
	// an optimization algorithm to minimize phase error.
	// We eyeballed it. The guitar doesn't care.
	//
	float freq_i[] = { 100.0f, 560.0f, 2400.0f, 9500.0f };
	float freq_q[] = { 170.0f, 960.0f, 4300.0f, 15500.0f };
	float Q = 0.7071f;

	for (int i = 0; i < 4; i++) {
		biquad_allpass_filter(&formant.ap_i[i], freq_i[i], Q);
		biquad_allpass_filter(&formant.ap_q[i], freq_q[i], Q);
	}

	formant.prev_phase = 0;
	formant.out_phase = 0;
	formant.envelope = 0;
	formant.prev_in = 0;
}

static inline float formant_step(float in)
{
	// Run both allpass chains on the same input
	float sig_i = in;
	float sig_q = in;

	for (int i = 0; i < 4; i++) {
		sig_i = biquad_step(&formant.ap_i[i], sig_i);
		sig_q = biquad_step(&formant.ap_q[i], sig_q);
	}

	// sig_i and sig_q are now (approximately) a Hilbert pair.
	// Together: the analytic signal. Magnitude is the envelope,
	// angle is the instantaneous phase.

	// Envelope extraction — the slow signal hiding inside
	// the fast one. The small signal that informs the big one.
	float env = sqrtf(sig_i * sig_i + sig_q * sig_q);

	// Smooth the envelope. More smoothing = flatter = more
	// "whisper." Less = preserves the natural dynamics.
	float smooth = 0.001f + formant.env_smooth * 0.05f;
	formant.envelope += smooth * (env - formant.envelope);

	// Instantaneous phase and its derivative (≈ frequency)
	float phase = atan2f(sig_q, sig_i);
	float dphase = phase - formant.prev_phase;
	formant.prev_phase = phase;

	// Phase unwrapping. Works great for smooth signals.
	// Confused by transients. Such is life without lookahead.
	while (dphase > M_PI) dphase -= 2 * M_PI;
	while (dphase < -M_PI) dphase += 2 * M_PI;

	// Scale the instantaneous frequency by pitch_ratio,
	// modulated by the formant strength control.
	// At strength=0, ratio=1 (no shift). At strength=1,
	// full pitch_ratio applied.
	float ratio = linear(formant.formant_strength, 1.0f, formant.pitch_ratio);
	float new_dphase = dphase * ratio;

	// Accumulate shifted phase
	formant.out_phase += new_dphase;

	// Keep the accumulator from drifting into float
	// precision territory after a few million samples
	while (formant.out_phase > M_PI) formant.out_phase -= 2 * M_PI;
	while (formant.out_phase < -M_PI) formant.out_phase += 2 * M_PI;

	// Reconstruct: preserved envelope * shifted carrier
	float wet = formant.envelope * cosf(formant.out_phase);
	wet = limit_value(wet);

	return linear(formant.blend, in, wet);
}
