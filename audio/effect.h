#include "lfo.h"

typedef float (*pot_convert_fn)(signed char);
typedef const char *(*pot_describe_fn)(signed char);

struct pot_descr {
	const char *label;
	pot_describe_fn describe;
	pot_convert_fn convert;
	signed char def_val;
};

static inline const char *desc_Hz(signed char pot) { return "Hz"; }
static inline const char *desc_kHz(signed char pot) { return "kHz"; }
static inline const char *desc_ms(signed char pot) { return "ms"; }
static inline const char *desc_x(signed char pot) { return "x"; }
static inline const char *desc_dB(signed char pot) { return "dB"; }
static inline const char *desc_none(signed char pot) { return ""; }

//
// Primary interface for audio DSP algorithms.
//
// Each effect plugin defines its runtime processing callbacks and UI
// layout.
//
// Note: effects[] may leave `pots` array with NULL labels, which tells
// the OLED and I2C pollers to omit querying/drawing parameters that the
// hook doesn't use.
//
// We have two set of pot values, because cpu 0 - the UI core - will
// prepare the pot state in the inactive set, and then atomically switch
// that state so that code 1 - the audio core - will never use pot
// values that are in some halfway state. The active state is the LSB of
// the 'seq' value, which is used to tell whether the state has changed.
//
// 'mix' is the current mixing state, and 'target' is the target mixing
// state for fading in and fading out the effect (in fractions of
// EFF_ENABLE_STEPS)
//
struct effect {
	const char *name, *short_name;
	unsigned int mix, target;
	volatile unsigned int seq, last;
	unsigned char intense, active_pot;
	signed char pot_values[2][10];
	void (*graph)(struct effect *, int, signed char[10]);
	void (*init)(signed char[10]);
	void (*load)(struct effect *, signed char[10]);
	void (*save)(struct effect *, signed char[10]);
	float (*step)(float);
	const struct pot_descr pots[10];
};

// Effects
#include "boost.h"
#include "eq.h"
#include "flanger.h"
#include "echo.h"
#include "phaser.h"
#include "pitch.h"
#include "compressor.h"
#include "usb.h"

static inline void generic_effect_describe(struct effect *e, signed char pots[10])
{
	for (int i = 0; i < 10; i++) {
		if (e->pots[i].label) {
			if (e->pots[i].convert) {
				float val = e->pots[i].convert(pots[i]);
				const char *unit = e->pots[i].describe ? e->pots[i].describe(pots[i]) : "";
				fprintf(stderr, " %s=%g %s", e->pots[i].label, val, unit);
			} else {
				const char *text = e->pots[i].describe ? e->pots[i].describe(pots[i]) : "";
				fprintf(stderr, " %s=%s", e->pots[i].label, text);
			}
		}
	}
	fprintf(stderr, "\n");
}

#define EFF(x) &x##_effect

static struct effect *const effects[] = {
	EFF(boost),
	&EQ,
	EFF(phaser),
	EFF(flanger),
	EFF(echo),
	EFF(pitch),
	EFF(compressor),
	EFF(usb),
};

static inline float do_effect_step(struct effect *effect, float val)
{
	if (effect->mix == effect->target) {
		if (effect->mix)
			val = effect->step(val);
	} else {
		int dir = effect->mix < effect->target ? +1 : -1;
		float mix = effect->mix / (float) EFF_ENABLE_STEPS;
		effect->mix += dir;
		float effect_val = effect->step(val);
		val = linear(mix, val, effect_val);
	}
	return val;
}

#include "process.h"
