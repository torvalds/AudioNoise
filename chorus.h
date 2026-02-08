//
// Chorus effect - multiple voices with modulated delays
//
// Creates a thicker sound by mixing the original signal with
// multiple slightly detuned copies using LFO-modulated delays.
//
static struct {
	struct lfo_state lfo1, lfo2, lfo3;
	float delay_ms;
	float depth;
	float mix;
} chorus;

static inline void chorus_describe(float pot[4])
{
	fprintf(stderr, " rate=%g Hz", linear(pot[0], 0.1, 5));
	fprintf(stderr, " delay=%g ms", linear(pot[1], 5, 30));
	fprintf(stderr, " depth=%g", pot[2]);
	fprintf(stderr, " mix=%g\n", pot[3]);
}

static inline void chorus_init(float pot[4])
{
	// pot[0]: LFO rate (0.1 - 5 Hz)
	float rate = linear(pot[0], 0.1, 5);

	// Slightly offset rates for each voice to avoid phase lock
	set_lfo_freq(&chorus.lfo1, rate);
	set_lfo_freq(&chorus.lfo2, rate * 1.1);
	set_lfo_freq(&chorus.lfo3, rate * 0.9);

	// pot[1]: base delay (5 - 30 ms)
	chorus.delay_ms = linear(pot[1], 5, 30);

	// pot[2]: depth/modulation amount (0 - 100%)
	chorus.depth = pot[2];

	// pot[3]: wet/dry mix (0 = dry, 1 = full wet)
	chorus.mix = pot[3];
}

static inline float chorus_step(float in)
{
	// Store input in delay buffer
	sample_array_write(in);

	// Get three modulated delay values
	float lfo1 = lfo_step(&chorus.lfo1, lfo_sinewave);
	float lfo2 = lfo_step(&chorus.lfo2, lfo_sinewave);
	float lfo3 = lfo_step(&chorus.lfo3, lfo_sinewave);

	float base_samples = chorus.delay_ms * SAMPLES_PER_MSEC;
	float mod_range = base_samples * chorus.depth * 0.5;

	float d1 = base_samples + lfo1 * mod_range;
	float d2 = base_samples + lfo2 * mod_range;
	float d3 = base_samples + lfo3 * mod_range;

	// Read delayed samples
	float v1 = sample_array_read(d1);
	float v2 = sample_array_read(d2);
	float v3 = sample_array_read(d3);

	// Mix voices (average of three delayed signals)
	float wet = (v1 + v2 + v3) / 3.0f;

	// Blend dry and wet
	return in * (1.0f - chorus.mix) + wet * chorus.mix;
}
