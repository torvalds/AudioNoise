// Flanger effect based on the MIT-licensed DaisySP library by Electrosmith,
// which, in turn, seems to be based on Soundpipe by Paul Batchelor

static inline void flanger_init(float pot1, float pot2, float pot3, float pot4)
{
	effect_set_lfo(pot1*pot1*10);	// lfo = 0 .. 10Hz
	effect_set_delay(pot2 * 4);	// delay = 0 .. 4 ms
	effect_set_depth(pot3);		// depth = 0 .. 100%
	effect_set_feedback(pot4);	// feedback = 0 .. 100%

	fprintf(stderr, "flanger:");
	fprintf(stderr, " freq=%g Hz", pot1*pot1*10);
	fprintf(stderr, " delay=%g ms", pot2*10);
	fprintf(stderr, " depth=%g", pot3);
	fprintf(stderr, " feedback=%g\n", pot1*pot1*10);
}

static inline float flanger_step(float in)
{
	float d = 1 + effect_delay * (1 + lfo_step(&effect_lfo, lfo_sinewave) * effect_depth);
	float out;

	out = sample_array_read(d);
	sample_array_write(limit_value(in + out * effect_feedback));

	return (in + out) / 2;
}
