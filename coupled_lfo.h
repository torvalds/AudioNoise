//
// Kuramoto-coupled LFO system
//
// Extends the LFO with oscillator coupling from the Kuramoto model:
//
//   dθᵢ/dt = ωᵢ + (K/N) Σⱼ sin(θⱼ - θᵢ)
//
// Multiple LFOs in a group can entrain (synchronize) when coupling
// K > 0. At K=0 they behave as independent LFOs. As K increases,
// oscillators with similar frequencies lock together.
//
// The coupling uses the existing quarter_sin lookup table for the
// sin(phase_diff) computation - no libm sin() calls.
//

#define MAX_COUPLED_LFOS 8

//
// Compute sin() of a raw u32 phase value using the quarter_sin table.
// This is the same computation as lfo_step() for sinewave, but
// without advancing the phase counter.
//
static inline float phase_sin(u32 phase)
{
	u32 quarter = phase >> 30;
	phase <<= 2;

	if (quarter & 1)
		phase = ~phase;

	u32 idx = phase >> (32 - QUARTER_SINE_STEP_SHIFT);
	float a = quarter_sin[idx];
	float b = quarter_sin[idx + 1];

	phase <<= QUARTER_SINE_STEP_SHIFT;
	float val = a + (b - a) * u32_to_fraction(phase);

	if (quarter & 2)
		val = -val;
	return val;
}

// cos(phase) = sin(phase + π/2)
static inline float phase_cos(u32 phase)
{
	return phase_sin(phase + (1u << 30));
}

struct coupled_lfo_group {
	struct lfo_state lfos[MAX_COUPLED_LFOS];
	int count;
	float coupling;		// K: 0 = independent, 1 = strong coupling
};

//
// Step one LFO in a coupled group.
//
// The coupling adjustment is applied as a fraction of the LFO's own
// step size, so K=1.0 means the coupling can shift the instantaneous
// frequency by up to ±100% of the natural frequency. K=0.1 means ±10%.
//
// Call this once per sample for each LFO in the group. The caller
// should step all LFOs in the group each sample to keep phases current.
//
static inline float coupled_lfo_step(struct coupled_lfo_group *g,
				     int idx, enum lfo_type type)
{
	struct lfo_state *lfo = &g->lfos[idx];

	if (g->coupling > 0 && g->count > 1) {
		float sum = 0;
		for (int j = 0; j < g->count; j++) {
			if (j == idx)
				continue;
			sum += phase_sin(g->lfos[j].idx - lfo->idx);
		}
		float adj = g->coupling * sum / g->count;
		lfo->idx += (s32)(adj * lfo->step);
	}

	return lfo_step(lfo, type);
}

//
// Kuramoto order parameter r ∈ [0,1]
//
// Measures global synchronization:
//   r = |(1/N) Σ e^(iθⱼ)|
//
// r → 1: all oscillators in phase (synchronized)
// r → 0: phases uniformly distributed (desynchronized)
//
static inline float coupled_lfo_order_parameter(struct coupled_lfo_group *g)
{
	if (g->count == 0)
		return 0;

	float cs = 0, sn = 0;
	for (int i = 0; i < g->count; i++) {
		cs += phase_cos(g->lfos[i].idx);
		sn += phase_sin(g->lfos[i].idx);
	}
	cs /= g->count;
	sn /= g->count;
	return sqrtf(cs * cs + sn * sn);
}
