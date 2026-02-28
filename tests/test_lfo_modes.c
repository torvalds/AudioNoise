/*
 * LFO mode tests
 * Tests triangle, sawtooth, and sinewave modes beyond the exhaustive test
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SAMPLES_PER_SEC (48000.0)

#include "../util.h"
#include "../lfo.h"

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

static void test_sinewave_range(void)
{
	struct lfo_state lfo = { .idx = 0 };
	set_lfo_freq(&lfo, 100.0f);  /* 100 Hz */

	float min_val = 1.0f, max_val = -1.0f;
	int n = (int)(SAMPLES_PER_SEC / 100.0f) * 3;  /* 3 full cycles */

	for (int i = 0; i < n; i++) {
		float val = lfo_step(&lfo, lfo_sinewave);
		if (val < min_val) min_val = val;
		if (val > max_val) max_val = val;

		ASSERT(val >= -1.001f && val <= 1.001f, "Sinewave should be in [-1, 1]");
	}

	ASSERT(max_val > 0.99f, "Sinewave should reach near +1");
	ASSERT(min_val < -0.99f, "Sinewave should reach near -1");
	printf("  Sinewave range: [%.6f, %.6f]\n", min_val, max_val);
}

static void test_triangle_symmetry(void)
{
	struct lfo_state lfo = { .idx = 0 };
	set_lfo_freq(&lfo, 100.0f);

	float min_val = 1.0f, max_val = -1.0f;
	float sum = 0;
	int n = (int)(SAMPLES_PER_SEC / 100.0f) * 4;  /* 4 full cycles */

	for (int i = 0; i < n; i++) {
		float val = lfo_step(&lfo, lfo_triangle);
		if (val < min_val) min_val = val;
		if (val > max_val) max_val = val;
		sum += val;

		ASSERT(val >= -1.001f && val <= 1.001f, "Triangle should be in [-1, 1]");
	}

	/* Triangle wave should be symmetric: average should be near 0 */
	float avg = sum / n;
	ASSERT_NEAR(avg, 0.0f, 0.02f, "Triangle wave should have zero DC offset");
	ASSERT(max_val > 0.99f, "Triangle should reach near +1");
	ASSERT(min_val < -0.99f, "Triangle should reach near -1");

	/* Symmetry: |max| should equal |min| approximately */
	ASSERT_NEAR(fabsf(max_val), fabsf(min_val), 0.02f, "Triangle should be symmetric");

	printf("  Triangle: range [%.4f, %.4f], avg=%.6f\n", min_val, max_val, avg);
}

static void test_sawtooth_range(void)
{
	struct lfo_state lfo = { .idx = 0 };
	set_lfo_freq(&lfo, 100.0f);

	float min_val = 1.0f, max_val = -1.0f;
	int n = (int)(SAMPLES_PER_SEC / 100.0f) * 3;

	for (int i = 0; i < n; i++) {
		float val = lfo_step(&lfo, lfo_sawtooth);
		if (val < min_val) min_val = val;
		if (val > max_val) max_val = val;

		ASSERT(val >= -0.001f && val <= 1.001f, "Sawtooth should be in [0, 1)");
	}

	ASSERT(min_val >= 0.0f, "Sawtooth min should be >= 0");
	ASSERT(max_val < 1.0f, "Sawtooth max should be < 1");
	ASSERT(max_val > 0.99f, "Sawtooth should reach near 1");
	printf("  Sawtooth range: [%.6f, %.6f]\n", min_val, max_val);
}

static void test_frequency_accuracy(void)
{
	/* Test that a 100Hz LFO completes ~100 cycles per second */
	struct lfo_state lfo = { .idx = 0 };
	set_lfo_freq(&lfo, 100.0f);

	int zero_crossings = 0;
	float prev = 0;
	int n = (int)SAMPLES_PER_SEC;  /* 1 second */

	for (int i = 0; i < n; i++) {
		float val = lfo_step(&lfo, lfo_sinewave);
		/* Count positive-going zero crossings */
		if (prev <= 0 && val > 0)
			zero_crossings++;
		prev = val;
	}

	/* Should have ~100 positive zero crossings in 1 second */
	ASSERT(zero_crossings >= 98 && zero_crossings <= 102,
		"100Hz LFO should have ~100 zero crossings per second");
	printf("  Frequency accuracy: 100Hz LFO -> %d crossings/sec\n", zero_crossings);
}

static void test_lfo_440hz(void)
{
	struct lfo_state lfo = { .idx = 0 };
	set_lfo_freq(&lfo, 440.0f);

	int zero_crossings = 0;
	float prev = 0;
	int n = (int)SAMPLES_PER_SEC;

	for (int i = 0; i < n; i++) {
		float val = lfo_step(&lfo, lfo_sinewave);
		if (prev <= 0 && val > 0)
			zero_crossings++;
		prev = val;
	}

	ASSERT(zero_crossings >= 438 && zero_crossings <= 442,
		"440Hz LFO should have ~440 zero crossings per second");
	printf("  Frequency accuracy: 440Hz LFO -> %d crossings/sec\n", zero_crossings);
}

int main(int argc, char **argv)
{
	printf("=== LFO Mode Tests ===\n");

	test_sinewave_range();
	test_triangle_symmetry();
	test_sawtooth_range();
	test_frequency_accuracy();
	test_lfo_440hz();

	printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
	return tests_failed ? 1 : 0;
}
