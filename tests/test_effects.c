/*
 * Effect integration tests
 * Tests each effect with sine wave, silence, and full-scale input
 *
 * NOTE: This test must be compiled as part of convert.c's compilation unit
 * since effects use shared global state. We re-include everything here
 * to create a standalone test binary.
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
#include "../flanger.h"
#include "../echo.h"
#include "../fm.h"
#include "../am.h"
#include "../phaser.h"
#include "../discont.h"
#include "../distortion.h"
/* tube.h excluded — requires FIR.raw file */
#include "../growlingbass.h"
#include "../pll.h"

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

/* Reset shared state between effect tests */
static void reset_shared_state(void)
{
	memset(sample_array, 0, sizeof(sample_array));
	sample_array_index = 0;
}

struct test_effect {
	const char *name;
	void (*init)(float[4]);
	float (*step)(float);
	float pots[4];
};

static struct test_effect test_effects[] = {
	{ "flanger",      flanger_init,      flanger_step,      { 0.6, 0.6, 0.6, 0.6 } },
	{ "echo",         echo_init,         echo_step,         { 0.3, 0.3, 0.3, 0.3 } },
	{ "fm",           fm_init,           fm_step,           { 0.25, 0.25, 0.5, 0.5 } },
	{ "am",           am_init,           am_step,           { 0.25, 0.25, 0.5, 0.5 } },
	{ "phaser",       phaser_init,       phaser_step,       { 0.3, 0.3, 0.5, 0.5 } },
	{ "discont",      discont_init,      discont_step,      { 0.8, 0.1, 0.2, 0.2 } },
	{ "distortion",   distortion_init,   distortion_step,   { 0.5, 0.6, 0.8, 0.0 } },
	{ "growlingbass",  growlingbass_init, growlingbass_step,  { 0.4, 0.35, 0.0, 0.4 } },
	{ "pll",          pll_init,          pll_step,          { 0.25, 0.5, 0.5, 0.5 } },
};

#define N_EFFECTS (sizeof(test_effects) / sizeof(test_effects[0]))

static void test_effect_sine_wave(void)
{
	printf("  Testing effects with 440Hz sine wave (48000 samples)...\n");

	for (int e = 0; e < (int)N_EFFECTS; e++) {
		struct test_effect *eff = &test_effects[e];
		reset_shared_state();
		eff->init(eff->pots);

		int all_finite = 1;
		float max_out = 0;

		for (int i = 0; i < 48000; i++) {
			float in = 0.5f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
			float out = eff->step(in);

			if (isnan(out) || isinf(out)) {
				all_finite = 0;
				break;
			}
			if (fabsf(out) > max_out) max_out = fabsf(out);
		}

		char msg[80];
		snprintf(msg, sizeof(msg), "%s: output should be finite", eff->name);
		ASSERT(all_finite, msg);

		snprintf(msg, sizeof(msg), "%s: output should be bounded (max=%.2f)", eff->name, max_out);
		ASSERT(max_out < 100.0f, msg);

		printf("    %s: max_out=%.4f %s\n", eff->name, max_out,
			all_finite ? "OK" : "FAIL");
	}
}

static void test_effect_silence(void)
{
	printf("  Testing effects with silence...\n");

	for (int e = 0; e < (int)N_EFFECTS; e++) {
		struct test_effect *eff = &test_effects[e];
		reset_shared_state();
		eff->init(eff->pots);

		float max_out = 0;
		int all_finite = 1;

		for (int i = 0; i < 48000; i++) {
			float out = eff->step(0.0f);
			if (isnan(out) || isinf(out)) {
				all_finite = 0;
				break;
			}
			if (fabsf(out) > max_out) max_out = fabsf(out);
		}

		char msg[80];
		snprintf(msg, sizeof(msg), "%s silence: output should be finite", eff->name);
		ASSERT(all_finite, msg);

		/* FM and AM are generators — they produce output even with silence input */
		if (strcmp(eff->name, "fm") != 0 && strcmp(eff->name, "am") != 0 &&
		    strcmp(eff->name, "pll") != 0) {
			snprintf(msg, sizeof(msg), "%s silence: output should be small (max=%.4f)", eff->name, max_out);
			ASSERT(max_out < 1.0f, msg);
		}

		printf("    %s: silence max_out=%.6f %s\n", eff->name, max_out,
			all_finite ? "OK" : "FAIL");
	}
}

static void test_effect_full_scale(void)
{
	printf("  Testing effects with full-scale input...\n");

	for (int e = 0; e < (int)N_EFFECTS; e++) {
		struct test_effect *eff = &test_effects[e];
		reset_shared_state();
		eff->init(eff->pots);

		int all_finite = 1;
		float max_out = 0;

		for (int i = 0; i < 48000; i++) {
			float in = sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
			float out = eff->step(in);

			if (isnan(out) || isinf(out)) {
				all_finite = 0;
				break;
			}
			if (fabsf(out) > max_out) max_out = fabsf(out);
		}

		char msg[80];
		snprintf(msg, sizeof(msg), "%s full-scale: output should be finite", eff->name);
		ASSERT(all_finite, msg);

		printf("    %s: full-scale max_out=%.4f %s\n", eff->name, max_out,
			all_finite ? "OK" : "FAIL");
	}
}

static void test_distortion_modes(void)
{
	printf("  Testing distortion modes...\n");

	float pots_soft[]  = { 0.5, 0.6, 0.8, 0.0 };   /* mode 0: soft clip */
	float pots_hard[]  = { 0.5, 0.6, 0.8, 0.5 };   /* mode 1: hard clip */
	float pots_asym[]  = { 0.5, 0.6, 0.8, 1.0 };   /* mode 2: asymmetric */

	float *pot_sets[] = { pots_soft, pots_hard, pots_asym };
	const char *mode_names[] = { "soft", "hard", "asymmetric" };

	for (int m = 0; m < 3; m++) {
		reset_shared_state();
		distortion_init(pot_sets[m]);

		int all_finite = 1;
		float max_out = 0;
		float sum_pos = 0, sum_neg = 0;

		for (int i = 0; i < 48000; i++) {
			float in = 0.8f * sinf(2.0f * M_PI * 440.0f * i / SAMPLES_PER_SEC);
			float out = distortion_step(in);

			if (isnan(out) || isinf(out)) { all_finite = 0; break; }
			if (fabsf(out) > max_out) max_out = fabsf(out);
			if (out > 0) sum_pos += out;
			else sum_neg += out;
		}

		char msg[80];
		snprintf(msg, sizeof(msg), "distortion %s: output finite", mode_names[m]);
		ASSERT(all_finite, msg);

		/* Asymmetric mode should have DC offset (sum_pos != -sum_neg) */
		float dc_ratio = (sum_pos + sum_neg) / (sum_pos - sum_neg);
		printf("    %s: max=%.4f, DC ratio=%.4f\n", mode_names[m], max_out, dc_ratio);

		if (m == 2) {
			snprintf(msg, sizeof(msg), "asymmetric should have DC offset");
			ASSERT(fabsf(dc_ratio) > 0.01f, msg);
		}
	}
}

int main(int argc, char **argv)
{
	printf("=== Effect Integration Tests ===\n");

	test_effect_sine_wave();
	test_effect_silence();
	test_effect_full_scale();
	test_distortion_modes();

	printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
	return tests_failed ? 1 : 0;
}
