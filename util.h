#include <string.h>

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

#define TWO_POW_32 (4294967296.0f)
#define LN2 0.69314718055994530942

// pow2()-1 using taylor series expansion
// good enough for the range we're interested in
// (mainly 0 .. 1, but maybe somebody wants -1 .. 1)
static inline float fastpow2_m1(float x)
{
	const float c1 = LN2,
		    c2 = LN2*LN2/2,
		    c3 = LN2*LN2*LN2/6,
		    c4 = LN2*LN2*LN2/24;
	float x2 = x*x;
	float x3 = x2*x;
	return c1*x + c2*x2 + c3*x3 + c4*x2*x2;
}

static inline float fastpow(float a, float b)
{
    int i;
	float r;
	
    memcpy(&i, &a, sizeof(i));
    i = (int)(b * (i - 1072632447) + 1072632447.0f);
    memcpy(&r, &i, sizeof(r));
    return r;
}

// Very random - but quick - function to smoothly
// limit x to -1 .. 1 when it approaches -2..2
//
// So you can add two values in the -1..1 range and
// then limit the sum to that range too.
static inline float limit_value(float x)
{
	float x2 = x*x;
	float x4 = x2*x2;
	return x*(1 - 0.19*x2 + 0.0162*x4);
}

static inline float u32_to_fraction(u32 val)
{
	return (1.0/TWO_POW_32) * val;
}

static inline u32 fraction_to_u32(float val)
{
	return (u32) (val * TWO_POW_32);
}

// Max ~1.25s delays at ~52kHz
#define SAMPLE_ARRAY_SIZE 65536
#define SAMPLE_ARRAY_MASK (SAMPLE_ARRAY_SIZE-1)
extern float sample_array[SAMPLE_ARRAY_SIZE];
extern int sample_array_index;

static inline void sample_array_write(float val)
{
	u32 idx = SAMPLE_ARRAY_MASK & ++sample_array_index;
	sample_array[idx] = val;
}

static inline float sample_array_read(float delay)
{
	int i = (int) delay;
	float frac = delay - i;
	int idx = sample_array_index - i;

	float a = sample_array[SAMPLE_ARRAY_MASK & idx];
	float b = sample_array[SAMPLE_ARRAY_MASK & ++idx];
	return a + (b-a)*frac;
}

// We can calculate sin/cos at the same time using
// the table lookup. It's "GoodEnough(tm)" and with
// 256 entries it's good to about 4.5 digits of
// precision if I tested it right.
//
// Don't use this for real work. For audio? It's fine.
#include "gensin.h"

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
	b = quarter_sin[idx+1];

	float y = a + (a - b)*phase;

	if (quadrant & 1) {
		float tmp = -x; x = y; y = tmp;
	}
	if (quadrant & 2) {
		x = -x; y = -y;
	}

	return (struct sincos) { x, y };
}
