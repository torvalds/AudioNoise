#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SAMPLES_PER_SEC (48000.0)

#include "../util.h"
#include "../lfo.h"
#include "../coupled_lfo.h"

static int failures = 0;

static void check(const char *name, int ok)
{
	if (ok) {
		printf("  PASS: %s\n", name);
	} else {
		printf("  FAIL: %s\n", name);
		failures++;
	}
}

//
// Test 1: phase_sin accuracy against libm sin()
//
static void test_phase_sin_accuracy(void)
{
	printf("Test 1: phase_sin accuracy\n");

	double maxerr = 0;
	u32 worst = 0;
	int steps = 100000;

	for (int i = 0; i < steps; i++) {
		u32 phase = (u32)((double)i / steps * TWO_POW_32);
		float got = phase_sin(phase);
		double exact = sin((double)phase / TWO_POW_32 * 2 * M_PI);
		double err = fabs(got - exact);
		if (err > maxerr) {
			maxerr = err;
			worst = phase;
		}
	}

	printf("  Max phase_sin error: %.8f at phase %u\n", maxerr, worst);
	// Same table as LFO sinewave, expect ~5 digits of precision
	check("phase_sin error < 1e-4", maxerr < 1e-4);

	// Also verify phase_cos
	double maxerr_cos = 0;
	for (int i = 0; i < steps; i++) {
		u32 phase = (u32)((double)i / steps * TWO_POW_32);
		float got = phase_cos(phase);
		double exact = cos((double)phase / TWO_POW_32 * 2 * M_PI);
		double err = fabs(got - exact);
		if (err > maxerr_cos)
			maxerr_cos = err;
	}
	printf("  Max phase_cos error: %.8f\n", maxerr_cos);
	check("phase_cos error < 1e-4", maxerr_cos < 1e-4);
}

//
// Test 2: Zero coupling produces identical output to standalone LFO
//
static void test_zero_coupling_equivalence(void)
{
	printf("Test 2: Zero coupling equivalence\n");

	struct lfo_state standalone;
	struct coupled_lfo_group group;

	memset(&standalone, 0, sizeof(standalone));
	memset(&group, 0, sizeof(group));

	set_lfo_freq(&standalone, 1.0);
	set_lfo_freq(&group.lfos[0], 1.0);
	set_lfo_freq(&group.lfos[1], 1.5);	// different freq, shouldn't matter
	group.count = 2;
	group.coupling = 0;			// zero coupling

	double maxerr = 0;
	int samples = 48000;	// 1 second

	for (int i = 0; i < samples; i++) {
		float ref = lfo_step(&standalone, lfo_sinewave);
		float got = coupled_lfo_step(&group, 0, lfo_sinewave);
		double err = fabs(ref - got);
		if (err > maxerr)
			maxerr = err;
	}

	printf("  Max difference from standalone: %.12f\n", maxerr);
	check("K=0 output identical to standalone LFO", maxerr == 0.0);
}

//
// Test 3: Two identical-frequency LFOs synchronize with coupling
//
static void test_synchronization(void)
{
	printf("Test 3: Synchronization of identical frequencies\n");

	struct coupled_lfo_group group;
	memset(&group, 0, sizeof(group));

	group.count = 2;
	group.coupling = 0.3;

	// Same frequency, different starting phases
	set_lfo_freq(&group.lfos[0], 2.0);
	set_lfo_freq(&group.lfos[1], 2.0);
	group.lfos[1].idx = (u32)(0.25 * TWO_POW_32);	// 90° offset

	float r_initial = coupled_lfo_order_parameter(&group);

	// Run for 5 seconds
	int samples = 48000 * 5;
	for (int i = 0; i < samples; i++) {
		coupled_lfo_step(&group, 0, lfo_sinewave);
		coupled_lfo_step(&group, 1, lfo_sinewave);
	}

	float r_final = coupled_lfo_order_parameter(&group);

	printf("  Initial order parameter: %.4f\n", r_initial);
	printf("  Final order parameter:   %.4f\n", r_final);
	check("r_initial < 0.9 (started offset)", r_initial < 0.9);
	check("r_final > 0.95 (converged)", r_final > 0.95);
}

//
// Test 4: Similar frequencies entrain, dissimilar ones don't fully lock
//
static void test_partial_synchronization(void)
{
	printf("Test 4: Partial synchronization (different frequencies)\n");

	// Close frequencies: should entrain
	struct coupled_lfo_group close;
	memset(&close, 0, sizeof(close));
	close.count = 3;
	close.coupling = 0.5;
	set_lfo_freq(&close.lfos[0], 1.0);
	set_lfo_freq(&close.lfos[1], 1.05);
	set_lfo_freq(&close.lfos[2], 0.95);

	// Far frequencies: should not fully lock
	struct coupled_lfo_group far;
	memset(&far, 0, sizeof(far));
	far.count = 3;
	far.coupling = 0.1;	// weak coupling
	set_lfo_freq(&far.lfos[0], 1.0);
	set_lfo_freq(&far.lfos[1], 3.0);
	set_lfo_freq(&far.lfos[2], 7.0);

	int samples = 48000 * 10;	// 10 seconds
	float r_close_max = 0;
	float r_far_sum = 0;
	int r_far_count = 0;

	for (int i = 0; i < samples; i++) {
		for (int j = 0; j < 3; j++) {
			coupled_lfo_step(&close, j, lfo_sinewave);
			coupled_lfo_step(&far, j, lfo_sinewave);
		}
		if (i > samples / 2) {
			float rc = coupled_lfo_order_parameter(&close);
			float rf = coupled_lfo_order_parameter(&far);
			if (rc > r_close_max)
				r_close_max = rc;
			r_far_sum += rf;
			r_far_count++;
		}
	}

	float r_far_avg = r_far_sum / r_far_count;

	printf("  Close freqs max r: %.4f\n", r_close_max);
	printf("  Far freqs avg r:   %.4f\n", r_far_avg);
	check("close frequencies entrain (r > 0.9)", r_close_max > 0.9);
	check("far frequencies don't fully lock (avg r < 0.9)", r_far_avg < 0.9);
}

//
// Test 5: Order parameter is 1.0 for single LFO
//
static void test_single_lfo(void)
{
	printf("Test 5: Single LFO edge case\n");

	struct coupled_lfo_group group;
	memset(&group, 0, sizeof(group));
	group.count = 1;
	group.coupling = 1.0;
	set_lfo_freq(&group.lfos[0], 1.0);

	float r = coupled_lfo_order_parameter(&group);

	// Step it and verify it still works
	for (int i = 0; i < 1000; i++)
		coupled_lfo_step(&group, 0, lfo_sinewave);

	float r2 = coupled_lfo_order_parameter(&group);

	printf("  r (single LFO): %.4f\n", r);
	printf("  r after stepping: %.4f\n", r2);
	check("single LFO r ~= 1.0", r > 0.999f);
	check("single LFO r still ~= 1.0 after stepping", r2 > 0.999f);
}

//
// Test 6: Empty group returns 0
//
static void test_empty_group(void)
{
	printf("Test 6: Empty group edge case\n");

	struct coupled_lfo_group group;
	memset(&group, 0, sizeof(group));
	group.count = 0;
	group.coupling = 1.0;

	float r = coupled_lfo_order_parameter(&group);
	printf("  r (empty): %.4f\n", r);
	check("empty group r == 0", r == 0.0f);
}

//
// Test 7: Coupling preserves average frequency
//
// The Kuramoto coupling redistributes phase velocity but shouldn't
// change the mean frequency of the group (it's conservative).
//
static void test_frequency_conservation(void)
{
	printf("Test 7: Coupling preserves average frequency\n");

	struct coupled_lfo_group coupled, uncoupled;
	memset(&coupled, 0, sizeof(coupled));
	memset(&uncoupled, 0, sizeof(uncoupled));

	coupled.count = uncoupled.count = 3;
	coupled.coupling = 0.3;
	uncoupled.coupling = 0;

	set_lfo_freq(&coupled.lfos[0], 1.0);
	set_lfo_freq(&coupled.lfos[1], 1.2);
	set_lfo_freq(&coupled.lfos[2], 0.8);
	set_lfo_freq(&uncoupled.lfos[0], 1.0);
	set_lfo_freq(&uncoupled.lfos[1], 1.2);
	set_lfo_freq(&uncoupled.lfos[2], 0.8);

	// Track total phase advancement over 10 seconds
	int samples = 48000 * 10;
	double coupled_total = 0, uncoupled_total = 0;

	for (int i = 0; i < samples; i++) {
		for (int j = 0; j < 3; j++) {
			u32 before_c = coupled.lfos[j].idx;
			u32 before_u = uncoupled.lfos[j].idx;

			coupled_lfo_step(&coupled, j, lfo_sinewave);
			coupled_lfo_step(&uncoupled, j, lfo_sinewave);

			// Phase advanced (wrapping-safe via unsigned subtraction)
			coupled_total += (double)(coupled.lfos[j].idx - before_c);
			uncoupled_total += (double)(uncoupled.lfos[j].idx - before_u);
		}
	}

	double ratio = coupled_total / uncoupled_total;
	printf("  Total phase ratio (coupled/uncoupled): %.6f\n", ratio);
	// Should be very close to 1.0 — coupling redistributes but doesn't
	// create or destroy phase velocity
	check("avg frequency preserved (ratio within 5%)", fabs(ratio - 1.0) < 0.05);
}

//
// Test 8: All waveform types work with coupling
//
static void test_all_waveforms(void)
{
	printf("Test 8: All waveform types work\n");

	enum lfo_type types[] = { lfo_sinewave, lfo_triangle, lfo_sawtooth };
	const char *names[] = { "sinewave", "triangle", "sawtooth" };

	for (int t = 0; t < 3; t++) {
		struct coupled_lfo_group group;
		memset(&group, 0, sizeof(group));
		group.count = 2;
		group.coupling = 0.3;
		set_lfo_freq(&group.lfos[0], 1.0);
		set_lfo_freq(&group.lfos[1], 1.1);

		int ok = 1;
		for (int i = 0; i < 48000; i++) {
			float v0 = coupled_lfo_step(&group, 0, types[t]);
			float v1 = coupled_lfo_step(&group, 1, types[t]);
			if (isnan(v0) || isnan(v1) || isinf(v0) || isinf(v1)) {
				ok = 0;
				break;
			}
			// Sinewave and triangle should be in [-1,1]
			// Sawtooth in [0,1]
			if (types[t] != lfo_sawtooth) {
				if (v0 < -1.01 || v0 > 1.01 || v1 < -1.01 || v1 > 1.01) {
					ok = 0;
					break;
				}
			}
		}
		char buf[64];
		snprintf(buf, sizeof(buf), "%s produces valid output", names[t]);
		check(buf, ok);
	}
}

//
// Test 9: Strong coupling doesn't cause numerical blowup
//
static void test_strong_coupling_stability(void)
{
	printf("Test 9: Strong coupling stability\n");

	struct coupled_lfo_group group;
	memset(&group, 0, sizeof(group));
	group.count = MAX_COUPLED_LFOS;
	group.coupling = 1.0;	// maximum coupling

	for (int i = 0; i < MAX_COUPLED_LFOS; i++)
		set_lfo_freq(&group.lfos[i], 0.5 + i * 0.3);

	int ok = 1;
	for (int i = 0; i < 48000 * 5; i++) {
		for (int j = 0; j < MAX_COUPLED_LFOS; j++) {
			float v = coupled_lfo_step(&group, j, lfo_sinewave);
			if (isnan(v) || isinf(v)) {
				ok = 0;
				break;
			}
		}
		if (!ok)
			break;
	}

	float r = coupled_lfo_order_parameter(&group);
	printf("  Final r with K=1.0, %d LFOs: %.4f\n", MAX_COUPLED_LFOS, r);
	check("no NaN/Inf after 5s at K=1.0", ok);
	check("order parameter valid", r >= 0 && r <= 1.0);
}

int main(int argc, char **argv)
{
	printf("Coupled LFO test suite\n");
	printf("======================\n\n");

	test_phase_sin_accuracy();
	printf("\n");
	test_zero_coupling_equivalence();
	printf("\n");
	test_synchronization();
	printf("\n");
	test_partial_synchronization();
	printf("\n");
	test_single_lfo();
	printf("\n");
	test_empty_group();
	printf("\n");
	test_frequency_conservation();
	printf("\n");
	test_all_waveforms();
	printf("\n");
	test_strong_coupling_stability();

	printf("\n======================\n");
	if (failures)
		printf("%d FAILURES\n", failures);
	else
		printf("All tests passed\n");
	return failures ? 1 : 0;
}
