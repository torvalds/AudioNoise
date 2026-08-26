//
// Entirely random pitch shifting effect walking the
// sample buffer at varying speeds, and hiding the
// discontinuities in the sequence by picking two
// different delays, and multiplying them with sin/cos
//
// sin/cos are zero at the respective discontinuities,
// and the signal power is proportional to the square
// of the voltage. With sin^2 * cos^2 = 1, the result
// should be a unity signal power gain.
//

#define DISCONT_SHIFT 12
#define DISCONT_STEPS (1 << DISCONT_SHIFT)

struct {
	float step, mix;
	unsigned phase, idx;
	float array[4*DISCONT_STEPS];
} pitch;

static float pitch_octave(signed char pot) { return linear_pot(pot, -2, 2); }
static float pitch_mix(signed char pot) { return linear_pot(pot, 0, 1); }

static void pitch_init(signed char pot[10])
{
	// Which direction do we walk the samples?
	// Walking backwards lowers the pitch
	// Walking forwards raises the pitch
	// Staying at the same delay keeps the pitch the same
	//
	float step = pow2(pitch_octave(pot[0]));	//  0.25 .. 4
	pitch.step = step - 1;				// -0.75 .. 3
	pitch.mix = pitch_mix(pot[1]);
}

// i is discontinuous when sin**2 is 0
// ni is discontinuous when cos**2 (aka 1-sin**2) is 0
static float pitch_step(float in)
{
	const u32 mask = DISCONT_STEPS-1;
	u32 phase = pitch.phase++;

	u32 i = phase & mask;
	u32 ni = (i + DISCONT_STEPS/2) & mask;

	// The 31 is because we only use half the phase,
	// so sin walks 0..0.5 and cos walks 0.25..0.75
	phase <<= 31-DISCONT_SHIFT;
	struct sincos w = fastsincos(u32_to_fraction(phase));

	float step = pitch.step;
	float delay = (step > 0) ? DISCONT_STEPS*3 : 0;

	sample_array_write(in, &pitch.idx, pitch.array);

	float d1 = sample_array_read(delay - i*step, &pitch.idx, pitch.array) * w.sin;
	float d2 = sample_array_read(delay - ni*step, &pitch.idx, pitch.array) * w.cos;

	return linear(pitch.mix, in, d1+d2);
}

static struct effect pitch_effect = {
	.name = "Pitch",
	.short_name = "PI",
	.init = pitch_init,
	.step = pitch_step,
	.pots = {
		{ "Octave", desc_none, pitch_octave },
		{ "Mix", desc_none, pitch_mix },
	}
};
