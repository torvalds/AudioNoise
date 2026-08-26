// Various utility functions mainly for
// imprecise but fast floating point

//
// Sized integer types I'm used to from the kernel.
//
// I dislike 'uint32_t' as being unwieldly (and historically not
// available in all environments, so you end up with a mess of
// configuration), and 'uint' as not having a well-defined size.
//
// I'm not using the 64-bit types yet, but the RP2354 has 32x32
// multiplies giving a 64-bit result, so I'm considering doing
// some fixed-point math, and this preps for it.
//
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// Declare the fast functions with hardware support
float rintf(float);
float sqrtf(float);
float fabsf(float);
float floorf(float);

#define log10f(x) (log2f(x)/LOG2_10)

#define TWO_POW_32 (4294967296.0f)
#define LN2 0.69314718055994530942
#define LOG2_10 3.3219280948873623479

#define SAMPLES_PER_MSEC (SAMPLES_PER_SEC * 0.001)

// Turn -100..100 pot to 0.0..1.0 float internally
#define POT_TO_FLOAT(pot) (((pot) + 100.0f) / 200.0f)

// Turn 0..1 into a range
#define linear(pot, a, b)	((a)+(pot)*((b)-(a)))
#define cubic(pot, a, b)	linear((pot)*(pot)*(pot), a, b)

// Turn a pot value into some reasonable range
#define linear_pot(pot, a, b)	linear(POT_TO_FLOAT(pot), a, b)
#define frequency_pot(pot, a, b) cubic(POT_TO_FLOAT(pot), a, b)

//
// Smoothly limit x to -1 .. 1
//
static inline float limit_value(float x)
{
	return x / (1 + fabsf(x));
}

static inline float u32_to_fraction(u32 val)
{
	return (1.0/TWO_POW_32) * val;
}

static inline u32 fraction_to_u32(float val)
{
	return (u32) (val * TWO_POW_32);
}

static inline void __sample_array_write(float val, unsigned *idxp, unsigned mask, float *array)
{
	array[mask & ++*idxp] = val;
}

#define sample_array_write(val, idxp, array) __sample_array_write(val, idxp, ARRAY_SIZE(array)-1, array)

static inline float __sample_array_read(float delay, unsigned *idxp, unsigned mask, float *array)
{
	int i = (int) delay;
	float frac = delay - i;
	int idx = *idxp - i;

	float a = array[mask & idx];
	float b = array[mask & --idx];
	return linear(frac, a, b);
}

#define sample_array_read(d, idxp, array) __sample_array_read(d, idxp, ARRAY_SIZE(array)-1, array)

#include "log2.h"

#define LOG2_STEPS (1<< LOG2_STEP_SHIFT)

float log2f(float x)
{
	union { float f; unsigned int i; } u = { x };

	// Extract exponent and set it to zero (127)
	int exp = (u.i >> 23) - 127;
	u.i = 0x3f800000 | (u.i & 0x7fffff);
	x = u.f;

	// Lookup table index and fraction
	x = x*LOG2_STEPS - LOG2_STEPS;
	int idx = (int) x;
	x -= idx;

	return exp + linear(x, log2_table[idx], log2_table[idx+1]);
}

#include "pow2.h"

#define POW2_STEPS (1<< POW2_STEP_SHIFT)

float pow2(float x)
{
	// Integer and fractional parts
	int i = (int)floorf(x);
	x -= i;

	unsigned exp_p16 = 1u << (16 + (int)i);

	// Lookup table index and fraction
	x *= POW2_STEPS;
	int idx = (int) x;
	x -= idx;

	// Linear interpolation and final size
	return linear(x, pow2_table[idx], pow2_table[idx+1]) * exp_p16;
}

// We can calculate sin/cos at the same time using
// the table lookup. It's "GoodEnough(tm)" and with
// 256 entries it's good to about 5.3 digits of
// precision if I tested it right.
//
// Don't use this for real work. For audio? It's fine.
#include "quarter_sine.h"

#define QUARTER_SINE_STEPS (1<< QUARTER_SINE_STEP_SHIFT)

struct sincos { float sin, cos; };

// positive phase numbers only, please..
struct sincos fastsincos(float phase)
{
	phase *= 4;
	int quadrant = (int)phase;
	phase -= quadrant;

	phase *= QUARTER_SINE_STEPS;
	int idx = (int) phase;
	phase -= idx;

	float a = quarter_sin[idx];
	float b = quarter_sin[idx+1];

	float x = a + (b-a)*phase;

	idx = QUARTER_SINE_STEPS - idx;
	a = quarter_sin[idx];
	b = quarter_sin[idx-1];

	float y = a + (b-a)*phase;

	if (quadrant & 1) {
		float tmp = -x; x = y; y = tmp;
	}
	if (quadrant & 2) {
		x = -x; y = -y;
	}

	return (struct sincos) { x, y };
}

// Half-time coefficient calculation:
//	0.5 ^ ( 1 / samples )
//	= 2 ^ ( -1 / (ms * SAMPLES_PER_MSEC) )
static inline float time_constant(float ms)
{
	return pow2(-1.0 / SAMPLES_PER_MSEC / ms);
}

static inline float db_to_level(float db)
{
	return pow2(LOG2_10 / 20.0f * db);
}
