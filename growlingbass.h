//
// Growling/Purring bass - add a minus one octave subharmonic and filtered
// tunable odd/even harmonics distorsion
// Author: Philippe Strauss <catseyechandra@proton.me>
//
static struct {
	float level_sub;
	float level_odd;
	float level_even;
	float tone_freq;
	struct biquad lpf_in;
	struct biquad lpf_odd;
	struct biquad lpf_even;
} growlingbass;

static inline void growlingbass_describe(float pot[4])
{
	fprintf(stderr, " level_sub=%g", pot[0]);
	fprintf(stderr, " level_odd=%g", pot[1]);
	fprintf(stderr, " level_even=%g", pot[2]);
	fprintf(stderr, " tone=%g Hz", pot_frequency(pot[3]));
}

static inline void growlingbass_init(float pot[4])
{
	// minus one octave subharmonic level
	growlingbass.level_sub = pot[0];

	// odd harmonics level
	growlingbass.level_odd = pot[1];

	// even harmonics level
	growlingbass.level_even = pot[2];

	// cutoff frequency for the lowpass after the two distorsion stages
	growlingbass.tone_freq = pot_frequency(pot[3]);

	// fixed input filter on the subharmonic chain
	biquad_lpf(&growlingbass.lpf_in, 300.0f, 0.707f);

	// odd harmonics LPF biquad coeffs
	biquad_lpf(&growlingbass.lpf_odd, growlingbass.tone_freq, 0.707f);

	// even harmonics LPF biquad coeffs
	biquad_lpf(&growlingbass.lpf_even, growlingbass.tone_freq, 0.707f);
}

// Hard clipping
static inline float hard_clip_growlingbass(float x, float ceil)
{
	if (x > 0.05f) return ceil;
	if (x < -0.05f) return -ceil;
	return x;
}

// for detecting and counting periods
static inline float sgn(float x)
{
	if (x > 0.0f) return 1.0f;
	return -1.0f;
}

static inline float growlingbass_step(float in)
{
	static unsigned nperiods = 0;
	static float previous_sign = -1.0f;
	// kind of envelope detection for ceil of hard_clip_growlingbass
	// so the odd harmonics stay relative to the current amplitude
	static float previous_minmax = 0.0f;
	static float minmax = 0.0f;
	float shaped_sub = 0.0f;

	float filtered_in = biquad_step(&growlingbass.lpf_in, in);
	// odd harmonics: hard_clip
	float shaped_odd = hard_clip_growlingbass(filtered_in, previous_minmax);
	// even harmonics (high pitched)
	float shaped_even = abs(in);
	float sign = sgn(filtered_in);

	// if we're on the rising edge of sgn(), we are starting a new period
	if ((sign - previous_sign) > 1.0f) {
		nperiods += 1;
		previous_minmax = minmax;
		minmax = 0;
	}
	//
	if (abs(in)>minmax)
		minmax=abs(in);

	// if we're on the positive, upper half of the signal
	if (sign > 0.0f) {
		// one period over two
		if ((nperiods % 2) == 0)
			shaped_sub = filtered_in; // outup this alternance
		else
			shaped_sub = -filtered_in; // or its negative counterpart
	}

	// Apply tone filter
	float filtered_odd = biquad_step(&growlingbass.lpf_odd, shaped_odd);
	float filtered_even = biquad_step(&growlingbass.lpf_even, shaped_even);

	previous_sign = sign;

	// Apply output levels
	return shaped_sub * growlingbass.level_sub + in
		+ filtered_odd * growlingbass.level_odd + filtered_even * growlingbass.level_even;
}
