//
// Our LFO generates a quarter cycle (0 .. 1) from a 30-bit cycle
// and when it overflows it changes the quarter counter, which
// then turns 0..1 into a series of [ 0..1 , 1..0 , 0..-1, -1..0 ]
//
// The quarter information is naturally in the two high
// bits of the index
//
// Every audio cycle we update the LFO counter by 'lfo_step',
// so the cycle of one quarter is
//
//     t = 2**30 / SAMPLES_PER_SEC / lfo_step
//
// and a full cycle is four times that (ie the full 32-bit cycle).
//
// Calling that (2**32)/SAMPLES_PER_SEC "F_STEP", we get
//
//     T = F_STEP / lfo_step
//     freq = lfo_step / F_STEP
//             => lfo_step = freq * F_STEP
//     ms = 1000 * T = 1000 * F_STEP / lfo_step
//             => lfo_step = 1000 * F_STEP / ms
//
#define F_STEP (TWO_POW_32/SAMPLES_PER_SEC)

enum lfo_type {
	lfo_sinewave,
	lfo_triangle,
	lfo_sawtooth,
};

struct lfo_state {
	u32 idx, step;
};

// Use this for LFO initializers.
#define LFO_FREQ(x) .step = (x)*F_STEP

static inline void set_lfo_step(struct lfo_state *lfo, float step)
{
	lfo->step = (u32) rintf(step);
}

void set_lfo_freq(struct lfo_state *lfo, float freq)
{
	set_lfo_step(lfo, freq * F_STEP);
}

void set_lfo_ms(struct lfo_state *lfo, float ms)
{
	// Max 10kHz
	if (ms < 0.1)
		ms = 0.1;
	set_lfo_step(lfo, 1000 * F_STEP / ms);
}

float lfo_step(struct lfo_state *lfo, enum lfo_type type)
{
	u32 now = lfo->idx;
	u32 next = now + lfo->step;

	lfo->idx = next;

	if (type == lfo_sawtooth)
		return u32_to_fraction(now);

	float val;
	u32 quarter = now >> 30;
	now <<= 2;

	// Second and fourth quarter reverses direction
	if (quarter & 1)
		now = ~now;

	if (type == lfo_sinewave) {
		u32 idx = now >> (32-QUARTER_SINE_STEP_SHIFT);
		float a = quarter_sin[idx];
		float b = quarter_sin[idx+1];

		now <<= QUARTER_SINE_STEP_SHIFT;
		val = a + (b-a)*u32_to_fraction(now);
	} else {
		val = u32_to_fraction(now);
	}

	// Last two quarters are negative
	if (quarter & 2)
		val = -val;
	return val;
}
