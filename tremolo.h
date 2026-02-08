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

static inline void tremolo_describe(float pot[4])
{
	fprintf(stderr, " rate=%g Hz", linear(pot[0], 0.5, 15));
	fprintf(stderr, " depth=%g", pot[1]);
	fprintf(stderr, " wave=%s\n", pot[2] < 0.5 ? "sine" : "triangle");
}

static inline void tremolo_init(float pot[4])
{
	// pot[0]: LFO rate (0.5 - 15 Hz)
	set_lfo_freq(&tremolo.lfo, linear(pot[0], 0.5, 15));

	// pot[1]: depth (0 - 100%)
	tremolo.depth = pot[1];

	// pot[2]: waveform (0-0.5 = sine, 0.5-1 = triangle)
	tremolo.wave = pot[2] < 0.5 ? lfo_sinewave : lfo_triangle;
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
