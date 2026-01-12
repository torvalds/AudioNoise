//
// Distortion/Overdrive effect - waveshaping with multiple modes
//
// Provides soft clipping (overdrive) through hard clipping (fuzz)
// with optional tone control via low-pass filter.
//
static struct {
	float drive;
	float tone_freq;
	float level;
	int mode;  // 0=soft (tanh), 1=hard clip, 2=asymmetric
	struct biquad tone_filter;
} distortion;

static inline void distortion_init(float pot1, float pot2, float pot3, float pot4)
{
	// pot1: drive/gain (1x - 50x)
	distortion.drive = 1.0f + pot1 * 49.0f;

	// pot2: tone (roll off high frequencies, 1kHz - 10kHz)
	distortion.tone_freq = 1000 + pot2 * 9000;
	biquad_lpf(&distortion.tone_filter, distortion.tone_freq, 0.707f);

	// pot3: output level (0 - 100%)
	distortion.level = pot3;

	// pot4: mode selection
	if (pot4 < 0.33f)
		distortion.mode = 0;  // soft clip (tanh)
	else if (pot4 < 0.66f)
		distortion.mode = 1;  // hard clip
	else
		distortion.mode = 2;  // asymmetric

	const char *mode_names[] = { "soft", "hard", "asymmetric" };

	fprintf(stderr, "distortion:");
	fprintf(stderr, " drive=%gx", distortion.drive);
	fprintf(stderr, " tone=%g Hz", distortion.tone_freq);
	fprintf(stderr, " level=%g", pot3);
	fprintf(stderr, " mode=%s\n", mode_names[distortion.mode]);
}

// Soft clipping using tanh approximation
static inline float soft_clip(float x)
{
	// Fast tanh approximation: x / (1 + |x|)
	// Gives smooth saturation curve
	return x / (1.0f + fabsf(x));
}

// Hard clipping
static inline float hard_clip(float x)
{
	if (x > 1.0f) return 1.0f;
	if (x < -1.0f) return -1.0f;
	return x;
}

// Asymmetric clipping (tube-like even harmonics)
static inline float asymmetric_clip(float x)
{
	if (x > 0)
		return soft_clip(x);
	else
		return soft_clip(x * 0.7f) * 0.7f;
}

static inline float distortion_step(float in)
{
	// Apply drive
	float driven = in * distortion.drive;

	// Apply waveshaping based on mode
	float shaped;
	switch (distortion.mode) {
	case 0:
		shaped = soft_clip(driven);
		break;
	case 1:
		shaped = hard_clip(driven);
		break;
	case 2:
	default:
		shaped = asymmetric_clip(driven);
		break;
	}

	// Apply tone filter
	float filtered = biquad_step(&distortion.tone_filter, shaped);

	// Apply output level
	return filtered * distortion.level;
}
