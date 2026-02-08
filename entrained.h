//
// Entrained modulation effect
//
// Multi-voice chorus where the LFO modulation sources are
// coupled via the Kuramoto model. At K=0 the voices modulate
// independently (standard chorus). As K increases, the LFOs
// synchronize and the modulation pattern transitions from
// complex/shimmery to coherent/pulsing.
//
// pot[0]: coupling K (0 = free chorus, 1 = locked unison)
// pot[1]: rate (0.2 - 5 Hz base LFO rate)
// pot[2]: depth (0 - 100% modulation depth)
// pot[3]: mix (0 = dry, 1 = wet)
//
static struct {
	struct coupled_lfo_group group;
	float delay_base;
	float depth;
	float mix;
} entrained;

#define ENTRAINED_VOICES 3
#define ENTRAINED_DELAY_MS 15	// center delay

static inline void entrained_describe(float pot[4])
{
	float rate = linear(pot[1], 0.2, 5);
	fprintf(stderr, " K=%g", pot[0]);
	fprintf(stderr, " rate=%g Hz", rate);
	fprintf(stderr, " depth=%g", pot[2]);
	fprintf(stderr, " mix=%g\n", pot[3]);
}

static inline void entrained_init(float pot[4])
{
	float rate = linear(pot[1], 0.2, 5);

	entrained.group.count = ENTRAINED_VOICES;
	entrained.group.coupling = pot[0];

	// Voices are detuned by ±15% from base rate
	for (int i = 0; i < ENTRAINED_VOICES; i++) {
		float detune = 1.0 + (i - ENTRAINED_VOICES / 2) * 0.15;
		set_lfo_freq(&entrained.group.lfos[i], rate * detune);
	}

	entrained.delay_base = ENTRAINED_DELAY_MS * SAMPLES_PER_MSEC;
	entrained.depth = pot[2];
	entrained.mix = pot[3];
}

static inline float entrained_step(float in)
{
	float wet = 0;

	for (int i = 0; i < ENTRAINED_VOICES; i++) {
		float lfo = coupled_lfo_step(&entrained.group, i, lfo_sinewave);
		float d = entrained.delay_base * (1 + lfo * entrained.depth * 0.5);
		if (d < 1)
			d = 1;
		wet += sample_array_read(d);
	}
	wet /= ENTRAINED_VOICES;

	sample_array_write(in);

	return in * (1 - entrained.mix) + wet * entrained.mix;
}
