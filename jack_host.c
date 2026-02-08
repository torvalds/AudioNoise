//
// Minimal JACK host for AudioNoise effects
// Allows testing effects in real-time without hardware
//
// Inspired by prior work from @phstrauss:
//   - jclient.h: https://gist.github.com/phstrauss/e6c449b337fc6b20ebd7937001589f3d
//   - jclient.c: https://gist.github.com/phstrauss/8fdef23b4749b7f6dc883444c4853d3f
//   - jack_passthru.c: https://gist.github.com/phstrauss/...
//
// Build: gcc -o jack_host jack_host.c -ljack -lm
//    or: make jack_host
//
// Usage: ./jack_host <effect> <pot1> <pot2> <pot3> <pot4>
//
// Then connect with: jack_connect system:capture_1 audionoise:input
//                    jack_connect audionoise:output system:playback_1
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <jack/jack.h>

typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;

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
#include "tube.h"
#include "growlingbass.h"

#define EFF(x) { #x, x##_describe, x##_init, x##_step }
struct effect {
	const char *name;
	void (*describe)(float[4]);
	void (*init)(float[4]);
	float (*step)(float);
} effects[] = {
	EFF(discont),
	EFF(distortion),
	EFF(echo),
	EFF(flanger),
	EFF(phaser),
	EFF(tube),
	EFF(growlingbass),

	/* "Helper" effects */
	EFF(am),
	EFF(fm),
};

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

static jack_client_t *client;
static jack_port_t *input_port;
static jack_port_t *output_port;
static struct effect *current_effect = &effects[0];
static float pots[4] = { 0.5, 0.5, 0.5, 0.5 };
static volatile int running = 1;

// JACK process callback - called for each audio buffer
static int process_callback(jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *in = jack_port_get_buffer(input_port, nframes);
	jack_default_audio_sample_t *out = jack_port_get_buffer(output_port, nframes);

	current_effect->init(pots);
	for (jack_nframes_t i = 0; i < nframes; i++) {
		UPDATE(effect_delay);
		out[i] = current_effect->step(in[i]);
	}

	return 0;
}

// Handle JACK shutdown
static void jack_shutdown(void *arg)
{
	fprintf(stderr, "JACK server shut down\n");
	running = 0;
}

// Handle Ctrl+C
static void signal_handler(int sig)
{
	fprintf(stderr, "\nShutting down...\n");
	running = 0;
}

static void print_usage(const char *progname)
{
	fprintf(stderr, "Usage: %s <effect> [pot1] [pot2] [pot3] [pot4]\n\n", progname);
	fprintf(stderr, "Available effects:\n");
	for (int i = 0; i < ARRAY_SIZE(effects); i++) {
		fprintf(stderr, "  %s\n", effects[i].name);
	}
	fprintf(stderr, "\nAfter starting, connect ports with:\n");
	fprintf(stderr, "  jack_connect system:capture_1 audionoise:input\n");
	fprintf(stderr, "  jack_connect audionoise:output system:playback_1\n");
}

int main(int argc, char **argv)
{
	jack_status_t status;
	int potnr = 0;

	if (argc < 2) {
		print_usage(argv[0]);
		return 1;
	}

	// Parse arguments flexibly (same style as convert.c)
	for (int i = 1; i < argc; i++) {
		const char *arg = argv[i];
		char *endptr;

		float val = strtof(arg, &endptr);
		if (endptr != arg) {
			if (potnr < 4) {
				pots[potnr++] = val;
				continue;
			}
			fprintf(stderr, "Too many pot values\n");
			return 1;
		}

		// Try as effect name
		int found = 0;
		for (int j = 0; j < ARRAY_SIZE(effects); j++) {
			if (!strcmp(arg, effects[j].name)) {
				current_effect = &effects[j];
				found = 1;
				break;
			}
		}
		if (found)
			continue;

		fprintf(stderr, "Unknown argument: %s\n", arg);
		print_usage(argv[0]);
		return 1;
	}

	// Initialize effect
	fprintf(stderr, "Playing %s: ", current_effect->name);
	current_effect->describe(pots);
	current_effect->init(pots);

	// Set up signal handler
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	// Open JACK client
	client = jack_client_open("audionoise", JackNullOption, &status);
	if (!client) {
		fprintf(stderr, "Failed to connect to JACK server\n");
		return 1;
	}

	// Check sample rate
	jack_nframes_t sr = jack_get_sample_rate(client);
	if (sr != 48000) {
		fprintf(stderr, "Warning: JACK sample rate is %d, effects expect 48000\n", sr);
	}

	// Set callbacks
	jack_set_process_callback(client, process_callback, NULL);
	jack_on_shutdown(client, jack_shutdown, NULL);

	// Create ports
	input_port = jack_port_register(client, "input",
		JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	output_port = jack_port_register(client, "output",
		JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

	if (!input_port || !output_port) {
		fprintf(stderr, "Failed to create JACK ports\n");
		jack_client_close(client);
		return 1;
	}

	// Activate client
	if (jack_activate(client)) {
		fprintf(stderr, "Failed to activate JACK client\n");
		jack_client_close(client);
		return 1;
	}

	fprintf(stderr, "JACK host running. Connect ports and play audio.\n");
	fprintf(stderr, "Press Ctrl+C to quit.\n");

	// Main loop - just wait for signal
	while (running) {
		sleep(1);
	}

	// Cleanup
	jack_client_close(client);
	fprintf(stderr, "Goodbye!\n");

	return 0;
}
