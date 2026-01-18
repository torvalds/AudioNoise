//
// Extended waveshaper primitives for guitar effects
//
// These complement distortion.h (soft_clip, hard_clip, asymmetric_clip)
// with additional clipping styles for fuzz, synth, and tube tones.
//

//
// Internal soft clip helper for diode_clip below
// Same algorithm as distortion.h's soft_clip: x / (1 + |x|)
//
static inline float _ws_soft_clip(float x)
{
	float ax = x >= 0 ? x : -x;
	return x / (1 + ax);
}

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
// Tube-style waveshaper using polynomial approximation
// Based on: y = (3x - x³) / 2, normalized for |x| <= 1
// Provides gentle compression with soft knee
//
static inline float tube_clip(float x)
{
	// Pre-limit to avoid polynomial blowup
	if (x > 1.5f) x = 1.5f;
	if (x < -1.5f) x = -1.5f;

	float x2 = x * x;
	return x * (1.5f - 0.5f * x2);
}

//
// Diode clipper emulation - asymmetric soft clip
// Approximates silicon diode clipping (forward voltage ~0.6V)
// ratio controls asymmetry (1.0 = symmetric, 0.5 = half negative clip)
//
static inline float diode_clip(float x, float ratio)
{
	float pos = _ws_soft_clip(x);
	float neg = _ws_soft_clip(x * ratio) / ratio;
	return x >= 0 ? pos : neg;
}
