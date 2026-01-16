#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SAMPLES_PER_SEC (48000.0)

// Core utility functions and helpers
#include "util.h"
#include "lfo.h"
#include "effect.h"
#include "biquad.h"
#include "process.h"

// Effects
#include "flanger.h"
#include "echo.h"
#include "fm.h"
#include "am.h"
#include "phaser.h"
#include "discont.h"
#include "distortion.h"
#include "chorus.h"

static void magnitude_init(float pot1, float pot2, float pot3, float pot4) {}
static float magnitude_step(float in) { return u32_to_fraction(magnitude); }

#define EFF(x) { #x, x##_init, x##_step }
struct effect {
	const char *name;
	void (*init)(float,float,float,float);
	float (*step)(float);
} effects[] = {
	EFF(discont),
	EFF(distortion),
	EFF(echo),
	EFF(flanger),
	EFF(phaser),
	EFF(chorus),

	/* "Helper" effects */
	EFF(am),
	EFF(fm),
	EFF(magnitude),
};

#define UPDATE(x) x += 0.001 * (target_##x - x)

int main(int argc, char **argv)
{
	float pot[4];
	struct effect *eff = &effects[0];
	s32 sample;

	if (argc < 6)
		return 1;

	const char *name = argv[1];

	for (int i = 0; i < ARRAY_SIZE(effects); i++) {
		if (!strcmp(name, effects[i].name))
			eff = effects+i;
	}

	for (int i = 0; i < 4; i++)
		pot[i] = atof(argv[2+i]);

	fprintf(stderr, "Playing %s(%f,%f,%f,%f)\n",
		eff->name, pot[0], pot[1], pot[2], pot[3]);

	eff->init(pot[0], pot[1], pot[2], pot[3]);
	while (fread(&sample, 4, 1, stdin) == 1) {
		UPDATE(effect_delay);

		float in = process_input(sample);

		float out = eff->step(in);

		sample = process_output(out);
		if (fwrite(&sample, 4, 1, stdout) != 1)
			return 1;
	}
	return 0;
}
