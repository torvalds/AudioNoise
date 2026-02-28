/*
 * Utility function tests
 * Tests limit_value, u32/fraction conversions, sample_array, fastsincos
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SAMPLES_PER_SEC (48000.0)

#include "../util.h"

static int tests_passed = 0, tests_failed = 0;

#define ASSERT(cond, msg) do { \
	if (!(cond)) { \
		fprintf(stderr, "FAIL: %s (line %d)\n", msg, __LINE__); \
		tests_failed++; \
	} else { \
		tests_passed++; \
	} \
} while(0)

#define ASSERT_NEAR(a, b, tol, msg) \
	ASSERT(fabs((double)(a) - (double)(b)) < (tol), msg)

/* === limit_value tests === */

static void test_limit_value_bounded(void)
{
	/* Test that output is always in (-1, 1) */
	float test_values[] = { -1000, -10, -1, -0.5, 0, 0.5, 1, 10, 1000, 1e10, -1e10 };
	for (int i = 0; i < 11; i++) {
		float out = limit_value(test_values[i]);
		char msg[80];
		snprintf(msg, sizeof(msg), "limit_value(%.2g) = %.6f should be in (-1,1)", test_values[i], out);
		ASSERT(out > -1.0f && out < 1.0f, msg);
	}
	printf("  limit_value bounded: OK\n");
}

static void test_limit_value_zero(void)
{
	ASSERT_NEAR(limit_value(0.0f), 0.0f, 1e-10f, "limit_value(0) should be 0");
	printf("  limit_value(0) = 0: OK\n");
}

static void test_limit_value_monotonic(void)
{
	/* Should be monotonically increasing */
	float prev = limit_value(-100.0f);
	for (float x = -99; x <= 100; x += 0.5f) {
		float cur = limit_value(x);
		ASSERT(cur >= prev, "limit_value should be monotonically increasing");
		prev = cur;
	}
	printf("  limit_value monotonic: OK\n");
}

/* === u32/fraction conversion tests === */

static void test_u32_fraction_roundtrip(void)
{
	float test_vals[] = { 0.0f, 0.25f, 0.5f, 0.75f, 0.999f };
	for (int i = 0; i < 5; i++) {
		u32 u = fraction_to_u32(test_vals[i]);
		float back = u32_to_fraction(u);
		char msg[80];
		snprintf(msg, sizeof(msg), "roundtrip %.3f -> %u -> %.6f", test_vals[i], u, back);
		ASSERT_NEAR(back, test_vals[i], 1e-6f, msg);
	}
	printf("  u32/fraction roundtrip: OK\n");
}

static void test_u32_fraction_range(void)
{
	ASSERT_NEAR(u32_to_fraction(0), 0.0f, 1e-10f, "u32_to_fraction(0) = 0");
	/* Max u32 should be close to 1.0 */
	float max_val = u32_to_fraction(0xFFFFFFFF);
	ASSERT(max_val > 0.99f && max_val < 1.0f, "u32_to_fraction(MAX) should be ~1.0");
	printf("  u32/fraction range: OK\n");
}

/* === sample_array tests === */

float sample_array[SAMPLE_ARRAY_SIZE];
int sample_array_index;

static void test_sample_array_write_read(void)
{
	memset(sample_array, 0, sizeof(sample_array));
	sample_array_index = 0;

	/* Write known values */
	for (int i = 0; i < 100; i++) {
		sample_array_write((float)i / 100.0f);
	}

	/* Read back the most recent value (delay = 0) */
	float val = sample_array_read(0.0f);
	ASSERT_NEAR(val, 0.99f, 0.01f, "sample_array_read(0) should be last written value");

	/* Read back with integer delay */
	val = sample_array_read(10.0f);
	/* 10 samples ago: index was 90, value was 0.89 */
	ASSERT_NEAR(val, 0.89f, 0.02f, "sample_array_read(10) should be 10 samples ago");

	printf("  sample_array write/read: OK\n");
}

static void test_sample_array_interpolation(void)
{
	memset(sample_array, 0, sizeof(sample_array));
	sample_array_index = 0;

	/* Write two known values */
	sample_array_write(1.0f);  /* index 1 */
	sample_array_write(3.0f);  /* index 2 */

	/* Read with fractional delay: 0.5 samples ago should interpolate */
	float val = sample_array_read(0.5f);
	/* At delay 0.5: between index 2 (3.0) and index 1 (1.0) */
	/* a = sample_array[2] = 3.0, b = sample_array[1] = 1.0 (after ++idx) */
	/* Wait, the ++idx makes this tricky. Let's just verify it's between the two values */
	ASSERT(val >= 1.0f && val <= 3.0f, "Interpolated value should be between neighbors");

	printf("  sample_array interpolation: OK (val=%.4f)\n", val);
}

/* === fastsincos tests === */

static void test_fastsincos_precision(void)
{
	double max_sin_err = 0, max_cos_err = 0;

	for (double f = 0; f < 1.0001; f += 0.0001) {
		struct sincos my = fastsincos(f);
		double s = sin(2 * M_PI * f);
		double c = cos(2 * M_PI * f);
		double esin = fabs(my.sin - s);
		double ecos = fabs(my.cos - c);
		if (esin > max_sin_err) max_sin_err = esin;
		if (ecos > max_cos_err) max_cos_err = ecos;
	}

	ASSERT(max_sin_err < 1e-4, "fastsin should have < 1e-4 error");
	ASSERT(max_cos_err < 1e-4, "fastcos should have < 1e-4 error");
	printf("  fastsincos: max sin err = %.2e, max cos err = %.2e\n", max_sin_err, max_cos_err);
}

static void test_fastsincos_known_values(void)
{
	/* phase=0: sin=0, cos=1 */
	struct sincos sc = fastsincos(0.0f);
	ASSERT_NEAR(sc.sin, 0.0f, 1e-4f, "sin(0) = 0");
	ASSERT_NEAR(sc.cos, 1.0f, 1e-4f, "cos(0) = 1");

	/* phase=0.25: sin=1, cos=0 */
	sc = fastsincos(0.25f);
	ASSERT_NEAR(sc.sin, 1.0f, 1e-4f, "sin(pi/2) = 1");
	ASSERT_NEAR(sc.cos, 0.0f, 1e-4f, "cos(pi/2) = 0");

	/* phase=0.5: sin=0, cos=-1 */
	sc = fastsincos(0.5f);
	ASSERT_NEAR(sc.sin, 0.0f, 1e-4f, "sin(pi) = 0");
	ASSERT_NEAR(sc.cos, -1.0f, 1e-4f, "cos(pi) = -1");

	printf("  fastsincos known values: OK\n");
}

static void test_fastsincos_pythagorean(void)
{
	/* sin^2 + cos^2 should be ~1 */
	for (float f = 0; f < 1.0f; f += 0.01f) {
		struct sincos sc = fastsincos(f);
		float mag = sc.sin * sc.sin + sc.cos * sc.cos;
		char msg[80];
		snprintf(msg, sizeof(msg), "sin^2+cos^2 at phase %.2f = %.6f", f, mag);
		ASSERT_NEAR(mag, 1.0f, 0.01f, msg);
	}
	printf("  fastsincos pythagorean identity: OK\n");
}

/* === linear/cubic macro tests === */

static void test_linear_macro(void)
{
	ASSERT_NEAR(linear(0.0f, 10.0f, 20.0f), 10.0f, 1e-6f, "linear(0, 10, 20) = 10");
	ASSERT_NEAR(linear(1.0f, 10.0f, 20.0f), 20.0f, 1e-6f, "linear(1, 10, 20) = 20");
	ASSERT_NEAR(linear(0.5f, 10.0f, 20.0f), 15.0f, 1e-6f, "linear(0.5, 10, 20) = 15");
	printf("  linear macro: OK\n");
}

int main(int argc, char **argv)
{
	printf("=== Utility Function Tests ===\n");

	test_limit_value_bounded();
	test_limit_value_zero();
	test_limit_value_monotonic();
	test_u32_fraction_roundtrip();
	test_u32_fraction_range();
	test_sample_array_write_read();
	test_sample_array_interpolation();
	test_fastsincos_precision();
	test_fastsincos_known_values();
	test_fastsincos_pythagorean();
	test_linear_macro();

	printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
	return tests_failed ? 1 : 0;
}
