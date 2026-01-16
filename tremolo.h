//
// Tremolo effect - amplitude modulation via LFO
//
// Classic guitar amp tremolo effect that modulates volume
// using a sine or triangle wave LFO.
//
static struct {
	struct lfo_state lfo;
	float depth;
	enum lfo_type wave;
} tremolo;

static inline void tremolo_init(float pot1, float pot2, float pot3, float pot4)
{
	// pot1: LFO rate (0.5 - 15 Hz)
	float rate = 0.5 + pot1 * 14.5;
	set_lfo_freq(&tremolo.lfo, rate);

	// pot2: depth (0 - 100%)
	tremolo.depth = pot2;

	// pot3: waveform (0-0.5 = sine, 0.5-1 = triangle)
	tremolo.wave = pot3 < 0.5 ? lfo_sinewave : lfo_triangle;

	fprintf(stderr, "tremolo:");
	fprintf(stderr, " rate=%g Hz", rate);
	fprintf(stderr, " depth=%g", pot2);
	fprintf(stderr, " wave=%s\n", pot3 < 0.5 ? "sine" : "triangle");
}

static inline float tremolo_step(float in)
{
	// Get LFO value (-1 to 1), convert to gain multiplier
	float lfo = lfo_step(&tremolo.lfo, tremolo.wave);

	// Convert LFO to amplitude multiplier: 1 - depth*(1-lfo)/2
	// When lfo=1, mult=1; when lfo=-1, mult=1-depth
	float mult = 1.0f - tremolo.depth * (1.0f - lfo) * 0.5f;

	return in * mult;
}
