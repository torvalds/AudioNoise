struct {
	struct lfo_state lfo;
	struct biquad_coeff coeff;
	float s0[2], s1[2], s2[2], s3[2];
	float center_f, octaves, Q, feedback;
} phaser;

static float phaser_pot0(signed char pot) { return frequency_pot(pot, 25, 2000); }
static float phaser_pot1(signed char pot) { return linear_pot(pot, 0, 0.75); }
static float phaser_pot2(signed char pot) { return frequency_pot(pot, 220, 6460); }
static float phaser_pot3(signed char pot) { return linear_pot(pot, 0.25, 2); }

void phaser_init(signed char pot[10])
{
	float ms = phaser_pot0(pot[0]);		// 25ms .. 2s
	set_lfo_ms(&phaser.lfo, ms);
	phaser.feedback = phaser_pot1(pot[1]);

	phaser.center_f = phaser_pot2(pot[2]);		// 220Hz .. 6.5kHz
	phaser.octaves = 0.5;				// 155Hz .. 9kHz
	phaser.Q = phaser_pot3(pot[3]);
}

float phaser_step(float in)
{
	float lfo = lfo_step(&phaser.lfo, lfo_triangle);
	float freq = pow2(lfo*phaser.octaves) * phaser.center_f;
	float out;

	_biquad_allpass_filter(&phaser.coeff, _w0(freq), phaser.Q);

	out = in + phaser.feedback * phaser.s3[0];
	out = biquad_step_df1(&phaser.coeff, out, phaser.s0, phaser.s1);
	out = biquad_step_df1(&phaser.coeff, out, phaser.s1, phaser.s2);
	out = biquad_step_df1(&phaser.coeff, out, phaser.s2, phaser.s3);

	return limit_value(in + out);
}

static struct effect phaser_effect = {
	.name = "Phaser",
	.short_name = "PSR",
	.init = phaser_init,
	.step = phaser_step,
	.pots = {
		{ "LFO", desc_ms, phaser_pot0 },
		{ "Feedback", desc_none, phaser_pot1 },
		{ "Freq", desc_Hz, phaser_pot2 },
		{ "Q", desc_none, phaser_pot3 },
	}
};
