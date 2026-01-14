//
// Classic tremolo effect - amplitude modulation of the input signal
//
// Unlike the 'am' effect which generates its own signal, this
// actually modulates the incoming audio.  Very similar to what
// old Fender amps would call "vibrato" (even though it's really
// amplitude modulation, not frequency modulation).
//
static inline void tremolo_init(float pot1, float pot2, float pot3, float pot4)
{
	float lfo = 0.5 + pot1*pot1*10;	// lfo = 0.5 .. 10.5 Hz

	effect_set_lfo(lfo);
	effect_set_depth(pot2);		// depth = 0 .. 100%

	fprintf(stderr, "tremolo:");
	fprintf(stderr, " lfo=%g Hz", lfo);
	fprintf(stderr, " depth=%g\n", pot2);
}

static inline float tremolo_step(float in)
{
	float mod = lfo_step(&effect_lfo, lfo_sinewave);

	// Scale mod from [-1,1] to [1-depth, 1]
	// When depth=1 and mod=-1, multiplier=0 (full effect)
	// When depth=0, multiplier=1 always (no effect)
	float multiplier = 1 - effect_depth * (1 - mod) / 2;

	return in * multiplier;
}
