//
// Extended waveshaper primitives for guitar effects
//
// These complement distortion.h (soft_clip, hard_clip, asymmetric_clip)
// with additional clipping styles for fuzz and synth tones.
//

//
// Foldback distortion - signal folds back when exceeding threshold
// Creates complex harmonics, useful for synth-style fuzz
//
static inline float fold_back(float x, float threshold)
{
	int iterations = 0;

	if (threshold <= 0)
		return 0;

	// Limit iterations to prevent infinite loop on extreme input
	while ((x > threshold || x < -threshold) && iterations++ < 16) {
		if (x > threshold)
			x = 2 * threshold - x;
		else
			x = -2 * threshold - x;
	}
	return x;
}

//
// Diode clipper emulation - asymmetric soft clip
// Approximates silicon diode clipping (forward voltage ~0.6V)
// ratio controls asymmetry (1.0 = symmetric, 0.5 = half negative clip)
//
static inline float diode_clip(float x, float ratio)
{
	float pos = limit_value(x);
	float neg = limit_value(x * ratio) / ratio;
	return x >= 0 ? pos : neg;
}
