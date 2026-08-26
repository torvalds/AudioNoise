#define EFF_ENABLE_STEPS ((int)SAMPLES_PER_SEC/10)

struct {
	struct biquad_coeff coeff[10];
	struct biquad_state state[10];
} eq;

// Linear gain depending on pot value
static float eq_pot_A(signed char pot)
{
	// Map pot [-100..100] to gain multiplier [0.1..10.0]
	// 10^(pot/100) = 2^(pot * 3.321928 / 100)
	return pow2(pot * 0.03321928f);
}

// Q depending on pot value: more extreme gain
// uses higher Q. Random choices here...
static float eq_pot_Q(signed char prev, signed char pot, signed char next)
{
	int diff = abs(prev-pot) + abs(next-pot);
	return 0.707 + (diff / 300.0);
}

#include "eq-w0.h"

static void eq_init(signed char pot[10])
{
	struct biquad_coeff *c = eq.coeff;

	_biquad_loshelf(c+0, EQ_W0[0], eq_pot_Q(pot[0],pot[0],pot[1]), eq_pot_A(pot[0]));
	_biquad_peaking(c+1, EQ_W0[1], eq_pot_Q(pot[0],pot[1],pot[2]), eq_pot_A(pot[1]));
	_biquad_peaking(c+2, EQ_W0[2], eq_pot_Q(pot[1],pot[2],pot[3]), eq_pot_A(pot[2]));
	_biquad_peaking(c+3, EQ_W0[3], eq_pot_Q(pot[2],pot[3],pot[4]), eq_pot_A(pot[3]));
	_biquad_peaking(c+4, EQ_W0[4], eq_pot_Q(pot[3],pot[4],pot[5]), eq_pot_A(pot[4]));
	_biquad_peaking(c+5, EQ_W0[5], eq_pot_Q(pot[4],pot[5],pot[6]), eq_pot_A(pot[5]));
	_biquad_peaking(c+6, EQ_W0[6], eq_pot_Q(pot[5],pot[6],pot[7]), eq_pot_A(pot[6]));
	_biquad_peaking(c+7, EQ_W0[7], eq_pot_Q(pot[6],pot[7],pot[8]), eq_pot_A(pot[7]));
	_biquad_peaking(c+8, EQ_W0[8], eq_pot_Q(pot[7],pot[8],pot[9]), eq_pot_A(pot[8]));
	_biquad_hishelf(c+9, EQ_W0[9], eq_pot_Q(pot[8],pot[9],pot[9]), eq_pot_A(pot[9]));
}

static float eq_step(float in)
{
	const struct biquad_coeff *c = eq.coeff;
	struct biquad_state *s = eq.state;

	float val = _biquad_step(c+0, s+0, in);
	val = _biquad_peaking_step(c+1, s+1, val);
	val = _biquad_peaking_step(c+2, s+2, val);
	val = _biquad_peaking_step(c+3, s+3, val);
	val = _biquad_peaking_step(c+4, s+4, val);
	val = _biquad_peaking_step(c+5, s+5, val);
	val = _biquad_peaking_step(c+6, s+6, val);
	val = _biquad_peaking_step(c+7, s+7, val);
	val = _biquad_peaking_step(c+8, s+8, val);
	return _biquad_step(c+9, s+9, val);
}

static float biquad_mag_sq(const struct biquad_coeff *c, const struct sincos w0, const struct sincos w2)
{
	float re_num = c->b0 + c->b1 * w0.cos + c->b2 * w2.cos;
	float im_num = c->b1 * w0.sin + c->b2 * w2.sin;
	float num = re_num * re_num + im_num * im_num;

	float re_den = 1.0f + c->a1 * w0.cos + c->a2 * w2.cos;
	float im_den = c->a1 * w0.sin + c->a2 * w2.sin;
	float den = re_den * re_den + im_den * im_den;

	if (den < 1e-12f) return num * 1e12f;
	return num / den;
}

static int eq_magnitude(int x)
{
	// Map x (0..127) to frequency 20Hz .. 17.5kHz
	float freq = 20 * pow2(x / 13.0);

	struct sincos w0 = fastsincos(freq / SAMPLES_PER_SEC);
	struct sincos w2 = fastsincos((2.0f * freq) / SAMPLES_PER_SEC);

	float mag_sq = 1.0f;
	const struct biquad_coeff *c = eq.coeff;
	mag_sq *= biquad_mag_sq(c+0,w0,w2);
	mag_sq *= biquad_mag_sq(c+1,w0,w2);
	mag_sq *= biquad_mag_sq(c+2,w0,w2);
	mag_sq *= biquad_mag_sq(c+3,w0,w2);
	mag_sq *= biquad_mag_sq(c+4,w0,w2);
	mag_sq *= biquad_mag_sq(c+5,w0,w2);
	mag_sq *= biquad_mag_sq(c+6,w0,w2);
	mag_sq *= biquad_mag_sq(c+7,w0,w2);
	mag_sq *= biquad_mag_sq(c+8,w0,w2);
	mag_sq *= biquad_mag_sq(c+9,w0,w2);

	float mag = sqrtf(mag_sq);
	if (mag < 0.0001f) mag = 0.0001f;

	// Convert linear gain to dB
	float db = 20.0f * log10f(mag);

	// Map dB to Y pixels [0..64].
	// y=25 is 1x gain (0 dB).
	// Let's map +20dB to y=0 and -20dB to y=50.
	int y = 32 - (int) rintf(db * 1.6f); // 32 / 20 = 1.6

	return y;
}

//
// Graph the frequency response and put a mark at
// the current active frequency
//
static void eq_graph(struct effect *effect, int active, signed char pots[10])
{
	sh1106_rectangle(0,0,128,64,rect_clear);
	sh1106_graph(0, 128, 0, 63, eq_magnitude);

	// 0..9 onto the right frequencies
	int x = 9 + active * 13;
	int y = eq_magnitude(x);
	if (y > 61)
		y = 61;
	sh1106_rectangle(x-2,y-2,5,5,rect_clear);
}

static struct effect EQ = {
	.name = "Graphic EQ",
	.short_name = "EQ",
	.graph = eq_graph,
	.init = eq_init,
	.step = eq_step,
	.pots = {
		{ "  31 Hz", desc_x,  eq_pot_A, 0 },
		{ "  62 Hz", desc_x,  eq_pot_A, 0 },
		{ " 125 Hz", desc_x,  eq_pot_A, 0 },
		{ " 250 Hz", desc_x,  eq_pot_A, 0 },
		{ " 500 Hz", desc_x,  eq_pot_A, 0 },
		{ "1.0 kHz", desc_x,  eq_pot_A, 0 },
		{ "2.0 kHz", desc_x,  eq_pot_A, 0 },
		{ "4.0 kHz", desc_x,  eq_pot_A, 0 },
		{ "8.0 kHz", desc_x,  eq_pot_A, 0 },
		{ " 16 kHz", desc_x,  eq_pot_A, 0 },
	}
};
