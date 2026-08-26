#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include "sh1106.h"

#define SAMPLES_PER_SEC (48000.0)

// Core utility functions and helpers
#include "util.h"
#include "biquad.h"
#include "effect.h"

#define BLOCKSIZE 200
static inline int make_one_noise(int in, int out, struct effect *eff)
{
	s32 input[BLOCKSIZE], output[BLOCKSIZE];
	int nr = read(in, input, sizeof(input));
	if (nr <= 0)
		return nr;

	nr /= 4;
	for (int i = 0; i < nr; i++) {
		u32 sample = input[i];
		float val = process_input(sample);

		val = eff->step(val);

		output[i] = process_output(val, sample);
	}
	write(out, output, nr * 4);
	return nr * 4;
}

static int pot_control = -1;
static signed char pots[10];

static void describe(struct effect *eff, signed char pots[10])
{
	printf("%s:\n", eff->name);
	for (int i = 0; i < 10; i++) {
		const struct pot_descr *desc = eff->pots+i;
		if (!desc->label)
			break;
		printf("  %s: %f %s\n", desc->label,
			desc->convert(pots[i]),
			desc->describe(pots[i]));
	}
}

static void *modify_pots(void *arg)
{
	struct effect *eff = arg;

	for (;;) {
		char buf[5];
		int n = read(pot_control, buf, sizeof(buf));
		if (n <= 0)
			return NULL;
		switch (buf[0]) {
		case 'p':
			unsigned int idx = buf[1]-'0';
			unsigned int d1 = buf[2]-'0';
			unsigned int d2 = buf[3]-'0';
			if (idx > 3 || d1 > 9 || d2 > 9)
				break;
			pots[idx] = 2*(d1*10+d2) - 100;
			describe(eff, pots);
			break;
		}
	}
}

int main(int argc, char **argv)
{
	struct effect *eff = NULL;
	int input = -1, output = -1;
	int potnr = 0;

	for (int i = 1; i < argc; i++) {
		const char *arg = argv[i];
		char *endptr;

		if (!strncmp(arg, "--control=", 10)) {
			pot_control = strtol(arg+10, &endptr, 0);
			if (endptr != arg+10)
				continue;
			fprintf(stderr, "Bad control fd input (%s)\n", arg);
			exit(1);
		}

		// Is the argument a floating point number?
		// The we assume it's a default pot value
		float val = strtof(arg, &endptr);
		if (endptr != arg) {
			if (potnr < 10) {
				pots[potnr++] = (int) rintf(200*val)-100;
				continue;
			}
			fprintf(stderr, "Too many pot values\n");
			exit(1);
		}

		// Is it the name of an effect and we don't have one yet?
		if (!eff) {
			for (int i = 0; i < ARRAY_SIZE(effects); i++) {
				if (!strcasecmp(arg, effects[i]->name))
					eff = effects[i];
			}
			if (eff)
				continue;
		}

		if (input < 0) {
			// We assume first filename is an input file
			if (!strcmp(arg, "-")) {
				input = 0;
				continue;
			}

			int fd = open(arg, O_RDONLY);
			if (fd < 0) {
				perror(arg);
				exit(1);
			}
			input = fd;
			continue;
		}

		if (output < 0) {
			// We assume second filename is an output file
			if (!strcmp(arg, "-")) {
				output = 1;
				continue;
			}

			int fd = open(arg, O_CREAT | O_WRONLY, 0666);
			if (fd < 0) {
				perror(arg);
				exit(1);
			}
			output = fd;
			continue;
		}

		fprintf(stderr, "Unrecognized option '%s'\n", arg);
		exit(1);
	}

	if (input < 0)
		input = 0;

	if (output < 0)
		output = 1;

#ifdef F_SETPIPE_SZ
	// Limit the output buffer size if we are
	// writing to a pipe. At least on Linux,
	// because I have no idea how to do it for
	// anything else
	fcntl(output, F_SETPIPE_SZ, 4096);
#endif

	describe(eff, pots);

	pthread_t pot_thread;
	if (pot_control >= 0)
		pthread_create(&pot_thread, NULL, modify_pots, eff);

	for (;;) {
		eff->init(pots);
		if (make_one_noise(input, output, eff) <= 0)
			break;
	}

	if (pot_control >= 0)
		pthread_cancel(pot_thread);
	return 0;
}
