//
// Formant-preserving pitch expansion ("Giant Whisper")
//
// The idea: split the signal into an amplitude envelope and a
// carrier oscillation, then shift only the carrier frequency.
// This changes the perceived "size" of the sound source without
// the chipmunk/monster pitch effect.
//
// We use a Hilbert transform approximation via cascaded allpass
// filters to get a ~90° phase-shifted version of the input.
// From there we can extract the analytic signal and manipulate
// the instantaneous frequency.
//
// Is this a proper Hilbert transform? No. Is it good enough for
// a guitar pedal? Probably. The allpass chain gives us roughly
// 90° across most of the audio band, with the usual garbage at
// the extremes. Don't look too closely.
//
// The phase unwrapping is also a bit sketchy — we're doing it
// sample-by-sample with no lookahead, so transients will glitch.
// But hey, that's character, right?
//

static struct {
	float pitch_ratio;
	float env_smooth;
	float blend;
	float formant_strength;

	// Hilbert transform: two parallel allpass chains
	// Chain A processes the original, Chain B gives ~90° shift
	// We use 4 cascaded allpass sections for decent bandwidth
	struct biquad ap_i[4];	// in-phase path (delays to match)
	struct biquad ap_q[4];	// quadrature path (~90° shift)

	// Phase accumulator for frequency shifting
	float prev_phase;
	float out_phase;

	// Envelope follower
	float envelope;

	// Previous input for the in-phase path delay matching
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
	// Set up the allpass chains for Hilbert transform approximation.
	//
	// The trick: run two parallel allpass chains with different
	// coefficients. Chain A and Chain B each have ~constant group
	// delay, but their phase responses differ by ~90° across the
	// audio band.
	//
	// These frequencies and Q values are hand-picked to give
	// reasonable coverage from ~100Hz to ~10kHz. Below 100Hz
	// it falls apart. Above 10kHz too. Guitar lives mostly
	// in that range so we're fine. Mostly.
	//
	// The "in-phase" chain just matches the group delay of
	// the quadrature chain so they stay time-aligned.
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
	// Run both allpass chains
	float sig_i = in;
	float sig_q = in;

	for (int i = 0; i < 4; i++) {
		sig_i = biquad_step(&formant.ap_i[i], sig_i);
		sig_q = biquad_step(&formant.ap_q[i], sig_q);
	}

	// sig_i ≈ delayed original, sig_q ≈ 90° shifted
	// Together they form a (crappy) analytic signal

	// Instantaneous amplitude (envelope)
	float env = sqrtf(sig_i * sig_i + sig_q * sig_q);

	// Smooth the envelope — pot[1] controls how much detail we keep
	// Low smoothing = more envelope detail = more natural
	// High smoothing = flatter envelope = more "whisper-like"
	float smooth = 0.001f + formant.env_smooth * 0.05f;
	formant.envelope += smooth * (env - formant.envelope);

	// Instantaneous phase
	float phase = atan2f(sig_q, sig_i);

	// Phase increment (instantaneous frequency, roughly)
	float dphase = phase - formant.prev_phase;
	formant.prev_phase = phase;

	// Unwrap phase difference to [-pi, pi]
	// This is the sketchy part. Works OK for smooth signals,
	// gets confused by transients. Such is life.
	while (dphase > M_PI) dphase -= 2 * M_PI;
	while (dphase < -M_PI) dphase += 2 * M_PI;

	// Scale the frequency by pitch_ratio, blended with formant strength
	float ratio = linear(formant.formant_strength, 1.0f, formant.pitch_ratio);
	float new_dphase = dphase * ratio;

	// Accumulate output phase
	formant.out_phase += new_dphase;

	// Keep output phase in reasonable range to avoid float precision issues
	// after running for a few million samples
	while (formant.out_phase > M_PI) formant.out_phase -= 2 * M_PI;
	while (formant.out_phase < -M_PI) formant.out_phase += 2 * M_PI;

	// Reconstruct: envelope * cos(shifted phase)
	// We use the smoothed envelope to preserve formant structure
	float wet = formant.envelope * cosf(formant.out_phase);

	// Sanity check — don't let it blow up
	wet = limit_value(wet);

	return linear(formant.blend, in, wet);
}
