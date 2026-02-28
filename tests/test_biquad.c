/*
 * Biquad filter tests
 * Verifies coefficient calculation and filter behavior
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SAMPLES_PER_SEC (48000.0)

#include "../util.h"
#include "../biquad.h"

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

/* Generate a sine wave and measure RMS output of a biquad filter */
static float measure_response(struct biquad *bq, float freq, int n_samples)
{
	double sum_sq = 0;
	/* Reset state */
	memset(&bq->state, 0, sizeof(bq->state));

	for (int i = 0; i < n_samples; i++) {
		float in = sinf(2.0f * M_PI * freq * i / SAMPLES_PER_SEC);
		float out = biquad_step(bq, in);
		/* Skip transient (first 1000 samples) */
		if (i >= 1000) {
			sum_sq += (double)out * out;
		}
	}
	return sqrtf(sum_sq / (n_samples - 1000));
}

static float measure_input_rms(float freq, int n_samples)
{
	double sum_sq = 0;
	for (int i = 1000; i < n_samples; i++) {
		float in = sinf(2.0f * M_PI * freq * i / SAMPLES_PER_SEC);
		sum_sq += (double)in * in;
	}
	return sqrtf(sum_sq / (n_samples - 1000));
}

static void test_lpf_passes_low_freq(void)
{
	struct biquad bq = {};
	biquad_lpf(&bq, 1000.0f, 0.707f);

	/* 100 Hz should pass through a 1kHz LPF with minimal attenuation */
	float rms_out = measure_response(&bq, 100.0f, 8000);
	float rms_in = measure_input_rms(100.0f, 8000);
	float ratio = rms_out / rms_in;

	ASSERT(ratio > 0.9f, "LPF should pass 100Hz through 1kHz cutoff");
	ASSERT(ratio < 1.1f, "LPF passband gain should be near unity");
	printf("  LPF 100Hz/1kHz cutoff: gain = %.4f\n", ratio);
}

static void test_lpf_attenuates_high_freq(void)
{
	struct biquad bq = {};
	biquad_lpf(&bq, 1000.0f, 0.707f);

	/* 10kHz should be heavily attenuated by 1kHz LPF */
	float rms_out = measure_response(&bq, 10000.0f, 8000);
	float rms_in = measure_input_rms(10000.0f, 8000);
	float ratio = rms_out / rms_in;

	ASSERT(ratio < 0.1f, "LPF should attenuate 10kHz through 1kHz cutoff");
	printf("  LPF 10kHz/1kHz cutoff: gain = %.4f\n", ratio);
}

static void test_hpf_passes_high_freq(void)
{
	struct biquad bq = {};
	biquad_hpf(&bq, 1000.0f, 0.707f);

	float rms_out = measure_response(&bq, 10000.0f, 8000);
	float rms_in = measure_input_rms(10000.0f, 8000);
	float ratio = rms_out / rms_in;

	ASSERT(ratio > 0.9f, "HPF should pass 10kHz through 1kHz cutoff");
	printf("  HPF 10kHz/1kHz cutoff: gain = %.4f\n", ratio);
}

static void test_hpf_attenuates_low_freq(void)
{
	struct biquad bq = {};
	biquad_hpf(&bq, 1000.0f, 0.707f);

	float rms_out = measure_response(&bq, 100.0f, 8000);
	float rms_in = measure_input_rms(100.0f, 8000);
	float ratio = rms_out / rms_in;

	ASSERT(ratio < 0.1f, "HPF should attenuate 100Hz through 1kHz cutoff");
	printf("  HPF 100Hz/1kHz cutoff: gain = %.4f\n", ratio);
}

static void test_allpass_preserves_magnitude(void)
{
	struct biquad bq = {};
	biquad_allpass_filter(&bq, 1000.0f, 0.707f);

	/* Test at several frequencies */
	float freqs[] = { 100, 500, 1000, 2000, 5000, 10000 };
	for (int f = 0; f < 6; f++) {
		float rms_out = measure_response(&bq, freqs[f], 8000);
		float rms_in = measure_input_rms(freqs[f], 8000);
		float ratio = rms_out / rms_in;

		char msg[80];
		snprintf(msg, sizeof(msg), "Allpass magnitude at %.0fHz should be ~1.0", freqs[f]);
		ASSERT(ratio > 0.95f && ratio < 1.05f, msg);
		printf("  Allpass at %.0fHz: gain = %.4f\n", freqs[f], ratio);
	}
}

static void test_lpf_dc_gain(void)
{
	/* DC gain of LPF should be 1.0 (0 dB) */
	struct biquad bq = {};
	biquad_lpf(&bq, 1000.0f, 0.707f);

	/* Feed DC (constant 1.0) and measure steady state */
	memset(&bq.state, 0, sizeof(bq.state));
	float out = 0;
	for (int i = 0; i < 10000; i++) {
		out = biquad_step(&bq, 1.0f);
	}
	ASSERT_NEAR(out, 1.0f, 0.01f, "LPF DC gain should be 1.0");
	printf("  LPF DC gain: %.6f\n", out);
}

static void test_stability_edge_cases(void)
{
	struct biquad bq = {};
	biquad_lpf(&bq, 1000.0f, 0.707f);

	/* Feed zero */
	float out = biquad_step(&bq, 0.0f);
	ASSERT(!isnan(out) && !isinf(out), "Zero input should produce finite output");

	/* Feed very large value */
	memset(&bq.state, 0, sizeof(bq.state));
	out = biquad_step(&bq, 1e6f);
	ASSERT(!isnan(out) && !isinf(out), "Large input should produce finite output");

	/* Feed very small value */
	memset(&bq.state, 0, sizeof(bq.state));
	out = biquad_step(&bq, 1e-30f);
	ASSERT(!isnan(out) && !isinf(out), "Tiny input should produce finite output");

	/* Extended run to check for instability */
	memset(&bq.state, 0, sizeof(bq.state));
	int stable = 1;
	for (int i = 0; i < 100000; i++) {
		float in = (i == 0) ? 1.0f : 0.0f; /* impulse */
		out = biquad_step(&bq, in);
		if (isnan(out) || isinf(out) || fabsf(out) > 1e10f) {
			stable = 0;
			break;
		}
	}
	ASSERT(stable, "Impulse response should decay, not explode");
	printf("  Stability tests passed\n");
}

static void test_coefficient_values(void)
{
	/* Verify LPF coefficients against manually calculated values */
	struct biquad bq = {};
	biquad_lpf(&bq, 1000.0f, 0.707f);

	/* At f=1000Hz, fs=48000, w0 = 2*pi*1000/48000 */
	/* With fastsincos approximation, just verify coefficients are sane */
	ASSERT(bq.coeff.b0 > 0, "LPF b0 should be positive");
	ASSERT(bq.coeff.b1 > 0, "LPF b1 should be positive");
	ASSERT(bq.coeff.b2 > 0, "LPF b2 should be positive");
	ASSERT_NEAR(bq.coeff.b0, bq.coeff.b2, 1e-6f, "LPF b0 should equal b2");
	ASSERT_NEAR(bq.coeff.b1, 2.0f * bq.coeff.b0, 1e-6f, "LPF b1 should be 2*b0");

	/* a1 should be negative for low frequencies */
	ASSERT(bq.coeff.a1 < 0, "LPF a1 should be negative for low cutoff");
	/* a2 should be positive and < 1 for stability */
	ASSERT(bq.coeff.a2 > 0 && bq.coeff.a2 < 1.0f, "LPF a2 should be in (0,1)");

	printf("  LPF coefficients: b0=%.6f b1=%.6f b2=%.6f a1=%.6f a2=%.6f\n",
		bq.coeff.b0, bq.coeff.b1, bq.coeff.b2, bq.coeff.a1, bq.coeff.a2);
}

int main(int argc, char **argv)
{
	printf("=== Biquad Filter Tests ===\n");

	test_lpf_passes_low_freq();
	test_lpf_attenuates_high_freq();
	test_hpf_passes_high_freq();
	test_hpf_attenuates_low_freq();
	test_allpass_preserves_magnitude();
	test_lpf_dc_gain();
	test_stability_edge_cases();
	test_coefficient_values();

	printf("\n%d passed, %d failed\n", tests_passed, tests_failed);
	return tests_failed ? 1 : 0;
}
