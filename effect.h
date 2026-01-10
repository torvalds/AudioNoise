//
// Shared common state for most effects
//
// The effects don't have to use these, but they are here to
// make some basic things very simple to do.
//
float sample_array[SAMPLE_ARRAY_SIZE];
int sample_array_index;

static float effect_feedback;
static float effect_delay, target_effect_delay;
static float effect_depth;
static struct lfo_state effect_lfo;

//Defines for feedback and settings
#define effect_set_lfo(f)	set_lfo_freq(&effect_lfo, f)
#define effect_set_lfo_ms(ms)	set_lfo_ms(&effect_lfo, ms)
#define effect_set_depth(d)	effect_depth = (d)
#define effect_set_feedback(fb)	effect_feedback = (fb)

#define SAMPLES_PER_MSEC (SAMPLES_PER_SEC * 0.001)

static inline void effect_set_delay(float ms)
{
	float samples = ms * SAMPLES_PER_MSEC;

	if (samples > 0 && samples < SAMPLE_ARRAY_SIZE)
		target_effect_delay = samples;
}
