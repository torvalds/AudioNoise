#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAMPLES_PER_SEC 48000.0

#include "sh1106.h"
#include "audio/util.h"
#include "audio/biquad.h"
#include "audio/effect.h"

int main(int argc, char **argv)
{
	signed char pots[10] = { 0 };
	eq_init(pots);

	// Verify each EQ center frequency against expected values
	float freq = 31.25f;
	for (int i = 0; i < 10; i++) {
		float phase = freq / SAMPLES_PER_SEC;
		float angle = phase * (float)(M_PI * 2);
		float expected_sin = sinf(angle);
		float expected_cos = cosf(angle);

		float err_sin = fabsf(EQ_W0[i].sin - expected_sin);
		float err_cos = fabsf(EQ_W0[i].cos - expected_cos);

		if (err_sin > 1e-6f || err_cos > 1e-6f) {
			fprintf(stderr, "EQ band %d (%.2f Hz) mismatch: sin err=%e, cos err=%e\n",
				i, freq, err_sin, err_cos);
			return 1;
		}
		freq *= 2.0f;
	}

	// Verify filter stability: feed an impulse and a test wave
	float out = eq_step(1.0f);
	if (!isfinite(out) || out == 0.0f) {
		fprintf(stderr, "EQ step returned invalid response: %f\n", out);
		return 1;
	}

	for (int i = 0; i < 1000; i++) {
		out = eq_step(sinf(i * 0.1f));
		if (!isfinite(out)) {
			fprintf(stderr, "EQ step diverged at sample %d: %f\n", i, out);
			return 1;
		}
	}

	printf("Graphic EQ table and biquads verified successfully.\n");
	return 0;
}
