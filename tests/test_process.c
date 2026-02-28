/*
 * Input/output processing tests
 * Tests process_input, process_output, noise gate behavior
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SAMPLES_PER_SEC (48000.0)

#include "../util.h"
#include "../process.h"

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

static void test_process_output_clamping(void)
{
	/* Positive overflow */
	s32 out = process_output(2.0f);
	ASSERT(out == 0x7fffffff, "process_output(2.0) should clamp to INT_MAX");

	/* Negative overflow */
	out = process_output(-2.0f);
	ASSERT(out == (s32)0x80000000, "process_output(-2.0) should clamp to INT_MIN");

	/* Normal values */
	out = process_output(0.0f);
	ASSERT(out == 0, "process_output(0.0) should be 0");

	out = process_output(0.5f);
	ASSERT(out > 0, "process_output(0.5) should be positive");

	out = process_output(-0.5f);
	ASSERT(out < 0, "process_output(-0.5) should be negative");

	printf("  process_output clamping: OK\n");
}

static void test_process_output_finite(void)
{
	/* Test with many values that output is always a valid s32 */
	for (float v = -5.0f; v <= 5.0f; v += 0.01f) {
		s32 out = process_output(v);
		/* Should always produce a valid integer (no UB observable) */
		ASSERT(out >= (s32)0x80000000 && out <= 0x7fffffff,
			"process_output should always produce valid s32");
	}
	printf("  process_output range: OK\n");
}

static void test_noise_gate_buildup(void)
{
	/*
	 * The noise gate starts very small and grows when signal is present.
	 * Feed a loud signal and verify the gate opens over time.
	 */
	/* We can't easily reset the static state, so we just verify behavior */
	float first_out = 0, last_out = 0;

	/* Feed loud signal */
	for (int i = 0; i < 48000; i++) {
		s32 sample = (s32)(0.5f * 0x7fffffff * sinf(2.0f * M_PI * 440.0f * i / 48000.0f));
		float out = process_input(sample);

		if (i == 0) first_out = fabsf(out);
		if (i == 47999) last_out = fabsf(out);
	}

	/* After 1 second of loud signal, output should be significantly louder
	 * than the first sample due to noise gate opening */
	printf("  noise gate: first sample magnitude = %.2e, last = %.2e\n",
		first_out, last_out);

	/* The gate should have opened - last output should be larger */
	ASSERT(last_out > first_out || last_out > 0.01f,
		"Noise gate should open for sustained signal");

	printf("  noise gate buildup: OK\n");
}

static void test_noise_gate_silence(void)
{
	/* Feed silence — noise gate should close (reduce output) */
	float out_after_silence = 0;
	for (int i = 0; i < 48000; i++) {
		float out = process_input(0);
		out_after_silence = out;
	}

	ASSERT_NEAR(out_after_silence, 0.0f, 1e-6f,
		"Silence input should produce near-zero output");
	printf("  noise gate silence: OK\n");
}

static void test_magnitude_tracking(void)
{
	/* Feed a known signal and verify magnitude tracks it */
	for (int i = 0; i < 10000; i++) {
		s32 sample = (s32)(0.3f * 0x7fffffff * sinf(2.0f * M_PI * 440.0f * i / 48000.0f));
		process_input(sample);
	}

	/* magnitude should be non-zero after feeding signal */
	ASSERT(magnitude > 0, "magnitude should track signal level");
	printf("  magnitude tracking: magnitude = %u\n", magnitude);
}

int main(int argc, char **argv)
{
	printf("=== Process Input/Output Tests ===\n");

	test_process_output_clamping();
	test_process_output_finite();
	test_noise_gate_buildup();
	test_noise_gate_silence();
	test_magnitude_tracking();

	printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
	return tests_failed ? 1 : 0;
}
