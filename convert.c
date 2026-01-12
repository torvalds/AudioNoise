#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef int s32;
typedef unsigned int u32;
typedef unsigned int uint;

#define SAMPLES_PER_SEC (48000.0)

// Core utility functions and helpers
#include "util.h"
#include "lfo.h"
#include "effect.h"
#include "biquad.h"

// Effects
#include "flanger.h"
#include "echo.h"
#include "fm.h"
#include "am.h"
#include "phaser.h"
#include "discont.h"
#include "distortion.h"

struct {
	float attack, decay, value;
} magnitude;
static inline void magnitude_init(float pot1, float pot2, float pot3, float pot4)
{
	magnitude.attack = pot1;
	magnitude.decay = pot2;
}
static inline float magnitude_step(float in)
{
	float mult, val = magnitude.value;

	in = fabs(in);
	mult = (in > val) ? magnitude.attack : magnitude.decay;
	val += mult * (in - val);
	return magnitude.value = val;
}

#define EFF(x) { #x, x##_init, x##_step }
struct effect {
	const char *name;
	void (*init)(float,float,float,float);
	float (*step)(float);
} effects[] = {
	EFF(discont), EFF(phaser), EFF(flanger), EFF(echo),
	EFF(fm), EFF(am), EFF(distortion),
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
		float in = sample / (float)0x80000000;
		UPDATE(effect_delay);
		float out = eff->step(in);
		sample = (int)(out * 0x80000000);
		if (fwrite(&sample, 4, 1, stdout) != 1)
			return 1;
	}
	return 0;
}
