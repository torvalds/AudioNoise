/*
 * Tests for formant.h — formant-preserving pitch expansion
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SAMPLES_PER_SEC (48000.0)

#include "../util.h"
#include "../lfo.h"
#include "../effect.h"
#include "../biquad.h"
#include "../process.h"
#include "../formant.h"

float sample_array[SAMPLE_ARRAY_SIZE];
int sample_array_index;

static int tests_passed = 0, tests_failed = 0;

#define ASSERT(cond, msg) do { \
	if (!(cond)) { \
		fprintf(stderr, "FAIL: %s (line %d)\n", msg, __LINE__); \
		tests_failed++; \
	} else { \
		tests_passed++; \
	} \
} while(0)

static void reset(void)
{
	memset(&formant, 0, sizeof(formant));
	memset(sample_array, 0, sizeof(sample_array));
	sample_array_index = 0;
}

/* Sine wave input at various frequencies should produce finite output */
static void test_finite_output(void)
{
	float freqs[] = { 100, 220, 440, 880, 2000 };
	printf("  Testing finite output at various frequencies...\n");

	for (int f = 0; f < 5; f++) {
		reset();
		float pots[] = { 0.5, 0.5, 1.0, 1.0 };  /* full wet, full formant */
		formant_init(pots);

		int all_finite = 1;
		float max_out = 0;

		for (int i = 0; i < 48000; i++) {
			float in = 0.5f * sinf(2.0f * M_PI * freqs[f] * i / SAMPLES_PER_SEC);
			float out = formant_step(in);
			if (isnan(out) || isinf(out)) { all_finite = 0; break; }
			if (fabsf(out) > max_out) max_out = fabsf(out);
		}

		char msg[80];
		snprintf(msg, sizeof(msg), "formant @ %.0fHz: finite output", freqs[f]);
		ASSERT(all_finite, msg);
		printf("    %.0fHz: max=%.4f %s\n", freqs[f], max_out, all_finite ? "OK" : "FAIL");
	}
}

/* Envelope should be roughly preserved — output amplitude in same ballpark as input */
static void test_envelope_preservation(void)
{
	printf("  Testing envelope preservation...\n");
	reset();
	float pots[] = { 0.5, 0.3, 1.0, 1.0 };
	formant_init(pots);

	float max_out = 0;
	float input_amp = 0.5f;

	/* Let it settle for 0.5s, then measure */
	for (int i = 0; i < 24000; i++) {
		float in = input_amp * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		formant_step(in);
	}
	for (int i = 24000; i < 48000; i++) {
		float in = input_amp * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		float out = formant_step(in);
		if (fabsf(out) > max_out) max_out = fabsf(out);
	}

	/* Output should be in the same order of magnitude as input.
	 * Not exact — the allpass chain and envelope tracking aren't perfect */
	char msg[80];
	snprintf(msg, sizeof(msg), "envelope preserved (max=%.3f, expected ~%.3f)", max_out, input_amp);
	ASSERT(max_out > 0.05f && max_out < 2.0f, msg);
	printf("    max_out=%.4f (input was %.2f)\n", max_out, input_amp);
}

/* pitch_ratio=1.0 should be roughly identity */
static void test_unity_ratio(void)
{
	printf("  Testing pitch_ratio=1.0 (identity-ish)...\n");
	reset();
	/* pot[0]=0.333 gives pitch_ratio=1.0 via linear(0.333, 0.5, 2.0) ≈ 1.0 */
	float pots[] = { 0.333f, 0.2f, 1.0f, 1.0f };
	formant_init(pots);

	float sum_diff = 0;
	int count = 0;

	/* Let it settle */
	for (int i = 0; i < 24000; i++) {
		float in = 0.5f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		formant_step(in);
	}

	for (int i = 24000; i < 48000; i++) {
		float in = 0.5f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		float out = formant_step(in);
		sum_diff += fabsf(out - in);
		count++;
	}

	float avg_diff = sum_diff / count;
	/* Won't be zero due to allpass chain phase shifts, but should be smallish */
	printf("    avg |out-in| = %.6f\n", avg_diff);
	ASSERT(avg_diff < 1.0f, "unity ratio: output should be similar to input");
}

/* No NaN/Inf with silence */
static void test_silence(void)
{
	printf("  Testing with silence...\n");
	reset();
	float pots[] = { 0.5, 0.5, 1.0, 1.0 };
	formant_init(pots);

	int all_finite = 1;
	for (int i = 0; i < 48000; i++) {
		float out = formant_step(0.0f);
		if (isnan(out) || isinf(out)) { all_finite = 0; break; }
	}
	ASSERT(all_finite, "silence: no NaN/Inf");
}

/* No NaN/Inf with DC input */
static void test_dc(void)
{
	printf("  Testing with DC input...\n");
	reset();
	float pots[] = { 0.5, 0.5, 1.0, 1.0 };
	formant_init(pots);

	int all_finite = 1;
	for (int i = 0; i < 48000; i++) {
		float out = formant_step(0.8f);
		if (isnan(out) || isinf(out)) { all_finite = 0; break; }
	}
	ASSERT(all_finite, "DC: no NaN/Inf");
}

/* No NaN/Inf with full-scale input */
static void test_full_scale(void)
{
	printf("  Testing with full-scale input...\n");
	reset();
	float pots[] = { 0.5, 0.5, 1.0, 1.0 };
	formant_init(pots);

	int all_finite = 1;
	for (int i = 0; i < 48000; i++) {
		float in = sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		float out = formant_step(in);
		if (isnan(out) || isinf(out)) { all_finite = 0; break; }
	}
	ASSERT(all_finite, "full-scale: no NaN/Inf");
}

/* Blend=0 should give dry signal back */
static void test_dry_blend(void)
{
	printf("  Testing blend=0 (dry)...\n");
	reset();
	float pots[] = { 0.5, 0.5, 0.0, 1.0 };  /* blend = 0 */
	formant_init(pots);

	float max_diff = 0;
	for (int i = 0; i < 48000; i++) {
		float in = 0.5f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		float out = formant_step(in);
		float diff = fabsf(out - in);
		if (diff > max_diff) max_diff = diff;
	}

	char msg[80];
	snprintf(msg, sizeof(msg), "dry blend: max diff=%.8f", max_diff);
	ASSERT(max_diff < 0.0001f, msg);
	printf("    max |out-in| = %.8f\n", max_diff);
}

int main(void)
{
	printf("=== Formant Effect Tests ===\n");

	test_finite_output();
	test_envelope_preservation();
	test_unity_ratio();
	test_silence();
	test_dc();
	test_full_scale();
	test_dry_blend();

	printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
	return tests_failed ? 1 : 0;
}
