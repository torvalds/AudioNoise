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
#include "phaser.h"
#include "discont.h"

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
	EFF(discont), EFF(phaser), EFF(flanger), EFF(echo), EFF(fm),
	EFF(magnitude),
};

#define UPDATE(x) x += 0.001 * (target_##x - x)

#define MAX_CHAIN 8

static struct effect *find_effect(const char *name)
{
	for (int i = 0; i < ARRAY_SIZE(effects); i++) {
		if (!strcmp(name, effects[i].name))
			return &effects[i];
	}
	return NULL;
}

int main(int argc, char **argv)
{
	float pot[4];
	struct effect *chain[MAX_CHAIN];
	int chain_len = 0;
	s32 sample;

	if (argc < 6)
		return 1;

	// Parse effect chain: "effect1+effect2+effect3" or just "effect1"
	char *chain_str = strdup(argv[1]);
	char *token = strtok(chain_str, "+");

	while (token && chain_len < MAX_CHAIN) {
		struct effect *eff = find_effect(token);
		if (eff) {
			chain[chain_len++] = eff;
		} else {
			fprintf(stderr, "Unknown effect: %s\n", token);
		}
		token = strtok(NULL, "+");
	}
	free(chain_str);

	if (chain_len == 0) {
		fprintf(stderr, "No valid effects specified\n");
		return 1;
	}

	for (int i = 0; i < 4; i++)
		pot[i] = atof(argv[2+i]);

	// Initialize all effects in chain with same pots
	// (In future, could support per-effect pots)
	fprintf(stderr, "Chain: ");
	for (int i = 0; i < chain_len; i++) {
		if (i > 0) fprintf(stderr, " -> ");
		fprintf(stderr, "%s", chain[i]->name);
		chain[i]->init(pot[0], pot[1], pot[2], pot[3]);
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "Pots: %f, %f, %f, %f\n", pot[0], pot[1], pot[2], pot[3]);

	while (fread(&sample, 4, 1, stdin) == 1) {
		float val = sample / (float)0x80000000;
		UPDATE(effect_delay);

		// Run through effect chain
		for (int i = 0; i < chain_len; i++)
			val = chain[i]->step(val);

		sample = (int)(val * 0x80000000);
		if (fwrite(&sample, 4, 1, stdout) != 1)
			return 1;
	}
	return 0;
}
