//
// Minimal echo effect
//
struct {
	float target_delay, delay, depth, feedback;

	// enough samples for 1.2 seconds
	unsigned idx;
	float array[65536];
} echo;

static float echo_pot0(signed char pot) { return POT_TO_FLOAT(pot) * 1000; }
static float echo_pot1(signed char pot) { return POT_TO_FLOAT(pot); }
static float echo_pot2(signed char pot) { return POT_TO_FLOAT(pot); }

static inline void echo_init(signed char pot[10])
{
	echo.target_delay = echo_pot0(pot[0]) * SAMPLES_PER_MSEC;	// delay = 0 .. 1s
	echo.depth = echo_pot1(pot[1]);					// depth = 0 .. 100%
	echo.feedback = echo_pot2(pot[3]);				// feedback = 0 .. 100%
}

static inline float echo_step(float in)
{
	echo.delay = linear(0.001, echo.delay, echo.target_delay);

	float d = 1 + echo.delay;
	float out;

	out = sample_array_read(d, &echo.idx, echo.array);
	sample_array_write(limit_value(in + out * echo.feedback), &echo.idx, echo.array);

	return linear(echo.depth, in, out);
}

static struct effect echo_effect = {
	.name = "Echo",
	.short_name = "ECHO",
	.init = echo_init,
	.step = echo_step,
	.pots = {
		{ "Delay", desc_ms, echo_pot0 },
		{ "Depth", desc_none, echo_pot1 },
		{ "Feedback", desc_none, echo_pot2 },
	}
};
