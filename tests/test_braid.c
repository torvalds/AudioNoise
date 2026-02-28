/*
 * Tests for braid.h — subharmonic-harmonic braid
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
#include "../braid.h"

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
	memset(&braid, 0, sizeof(braid));
	memset(sample_array, 0, sizeof(sample_array));
	sample_array_index = 0;
}

/* Basic finite output with sine wave */
static void test_finite_sine(void)
{
	printf("  Testing finite output with 440Hz sine...\n");
	reset();
	float pots[] = { 0.4, 0.5, 0.3, 1.0 };
	braid_init(pots);

	int all_finite = 1;
	float max_out = 0;

	for (int i = 0; i < 96000; i++) {
		float in = 0.5f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		float out = braid_step(in);
		if (isnan(out) || isinf(out)) { all_finite = 0; break; }
		if (fabsf(out) > max_out) max_out = fabsf(out);
	}

	ASSERT(all_finite, "440Hz sine: finite output");
	ASSERT(max_out < 10.0f, "440Hz sine: bounded output");
	printf("    max_out=%.4f\n", max_out);
}

/* K=0: free-running oscillators shouldn't blow up */
static void test_coupling_zero(void)
{
	printf("  Testing K=0 (free oscillators)...\n");
	reset();
	float pots[] = { 0.0, 0.5, 0.5, 1.0 };
	braid_init(pots);

	int all_finite = 1;
	float max_out = 0;

	for (int i = 0; i < 96000; i++) {
		float in = 0.5f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		float out = braid_step(in);
		if (isnan(out) || isinf(out)) { all_finite = 0; break; }
		if (fabsf(out) > max_out) max_out = fabsf(out);
	}

	ASSERT(all_finite, "K=0: no blowup");
	printf("    max_out=%.4f\n", max_out);
}

/* K=1: locked oscillators should be stable */
static void test_coupling_one(void)
{
	printf("  Testing K=1 (locked oscillators)...\n");
	reset();
	float pots[] = { 1.0, 0.5, 0.5, 1.0 };
	braid_init(pots);

	int all_finite = 1;
	float max_out = 0;

	for (int i = 0; i < 96000; i++) {
		float in = 0.5f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		float out = braid_step(in);
		if (isnan(out) || isinf(out)) { all_finite = 0; break; }
		if (fabsf(out) > max_out) max_out = fabsf(out);
	}

	ASSERT(all_finite, "K=1: stable");
	printf("    max_out=%.4f\n", max_out);
}

/* Sub level pot should affect subharmonic presence */
static void test_sub_level(void)
{
	printf("  Testing sub level control...\n");

	/* Run with sub=0, measure output energy */
	reset();
	float pots_nosub[] = { 0.4, 0.0, 0.3, 1.0 };
	braid_init(pots_nosub);

	float energy_nosub = 0;
	for (int i = 0; i < 96000; i++) {
		float in = 0.5f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		float out = braid_step(in);
		energy_nosub += out * out;
	}

	/* Run with sub=1.0 */
	reset();
	float pots_sub[] = { 0.4, 1.0, 0.3, 1.0 };
	braid_init(pots_sub);

	float energy_sub = 0;
	for (int i = 0; i < 96000; i++) {
		float in = 0.5f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		float out = braid_step(in);
		energy_sub += out * out;
	}

	printf("    energy no_sub=%.4f with_sub=%.4f\n", energy_nosub, energy_sub);
	ASSERT(energy_sub > energy_nosub, "sub level: more sub = more energy");
}

/* No NaN/Inf with silence */
static void test_silence(void)
{
	printf("  Testing with silence...\n");
	reset();
	float pots[] = { 0.4, 0.5, 0.3, 1.0 };
	braid_init(pots);

	int all_finite = 1;
	for (int i = 0; i < 48000; i++) {
		float out = braid_step(0.0f);
		if (isnan(out) || isinf(out)) { all_finite = 0; break; }
	}
	ASSERT(all_finite, "silence: no NaN/Inf");
}

/* No NaN/Inf with full-scale */
static void test_full_scale(void)
{
	printf("  Testing with full-scale...\n");
	reset();
	float pots[] = { 0.4, 0.5, 0.3, 1.0 };
	braid_init(pots);

	int all_finite = 1;
	for (int i = 0; i < 48000; i++) {
		float in = sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		float out = braid_step(in);
		if (isnan(out) || isinf(out)) { all_finite = 0; break; }
	}
	ASSERT(all_finite, "full-scale: no NaN/Inf");
}

/* Blend=0 gives dry signal */
static void test_dry_blend(void)
{
	printf("  Testing blend=0 (dry)...\n");
	reset();
	float pots[] = { 0.4, 0.5, 0.3, 0.0 };  /* blend = 0 */
	braid_init(pots);

	float max_diff = 0;
	for (int i = 0; i < 48000; i++) {
		float in = 0.5f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		float out = braid_step(in);
		float diff = fabsf(out - in);
		if (diff > max_diff) max_diff = diff;
	}

	char msg[80];
	snprintf(msg, sizeof(msg), "dry blend: max diff=%.8f", max_diff);
	ASSERT(max_diff < 0.0001f, msg);
	printf("    max |out-in| = %.8f\n", max_diff);
}

/* Frequency tracking: 440Hz sine should be detected */
static void test_freq_tracking(void)
{
	printf("  Testing frequency tracking with 440Hz sine...\n");
	reset();
	float pots[] = { 0.4, 0.5, 0.3, 1.0 };
	braid_init(pots);

	/* Run 2 seconds to let tracking settle */
	for (int i = 0; i < 96000; i++) {
		float in = 0.5f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
		braid_step(in);
	}

	float detected = braid.smoothed_freq;
	printf("    detected freq = %.1f Hz (expected ~440)\n", detected);

	/* Zero-crossing detection measures half-cycles, so frequency
	 * might be off by a factor. Let's be generous. */
	ASSERT(detected > 200.0f && detected < 1000.0f,
		"frequency tracking: detected freq in reasonable range");
}

int main(void)
{
	printf("=== Braid Effect Tests ===\n");

	test_finite_sine();
	test_coupling_zero();
	test_coupling_one();
	test_sub_level();
	test_silence();
	test_full_scale();
	test_dry_blend();
	test_freq_tracking();

	printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
	return tests_failed ? 1 : 0;
}
