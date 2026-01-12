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

	/* "Helper" effects */
	EFF(am),
	EFF(fm),
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
		UPDATE(effect_delay);

		float val = process_input(sample);

		// Run through effect chain
		for (int i = 0; i < chain_len; i++)
			val = chain[i]->step(val);

		sample = process_output(val);
		if (fwrite(&sample, 4, 1, stdout) != 1)
			return 1;
	}
	return 0;
}
