// Flanger effect based on the MIT-licensed DaisySP library by Electrosmith
// which in turn seems to be based on Soundpipe by Paul Batchelor

static struct {
	struct lfo_state lfo;
	float delay, depth, feedback;

	// Large enough history buffer for 10ms
	unsigned int idx;
	float samples[1024];
} flanger;

static float flanger_pot0(signed char pot) { float p = POT_TO_FLOAT(pot); return p*p*10; }
static float flanger_pot1(signed char pot) { return linear_pot(pot, 0, 4); }
static float flanger_pot2(signed char pot) { return POT_TO_FLOAT(pot); }
static float flanger_pot3(signed char pot) { return POT_TO_FLOAT(pot); }

static inline void flanger_init(signed char pot[10])
{
	set_lfo_freq(&flanger.lfo, flanger_pot0(pot[0]));
	flanger.delay = flanger_pot1(pot[1]) * SAMPLES_PER_MSEC;
	flanger.depth = flanger_pot2(pot[2]);
	flanger.feedback = flanger_pot3(pot[3]);
}

static inline float flanger_step(float in)
{
	float d = 1 + flanger.delay * (1 + lfo_step(&flanger.lfo, lfo_sinewave) * flanger.depth);
	float out;

	out = sample_array_read(d, &flanger.idx, flanger.samples);
	sample_array_write(limit_value(in + out * flanger.feedback), &flanger.idx, flanger.samples);

	return (in + out) / 2;
}

static struct effect flanger_effect = {
	.name = "Flanger",
	.short_name = "FLN",
	.init = flanger_init,
	.step = flanger_step,
	.pots = {
		{ "Freq", desc_Hz, flanger_pot0 },
		{ "Delay", desc_ms, flanger_pot1 },
		{ "Depth", desc_none, flanger_pot2 },
		{ "Feedback", desc_none, flanger_pot3 },
	}
};
