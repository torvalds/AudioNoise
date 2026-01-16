struct {
	struct lfo_state lfo;
	struct biquad_coeff coeff;
	float s0[2], s1[2], s2[2], s3[2];
	float center_f, octaves, Q, feedback;
} phaser;

#define linear(pot, a, b) ((a)+pot*((b)-(a)))
#define cubic(pot, a, b) linear((pot)*(pot)*(pot), a, b)

void phaser_init(float pot1, float pot2, float pot3, float pot4)
{
	float ms = cubic(pot1, 25, 2000);		// 25ms .. 2s
	set_lfo_ms(&phaser.lfo, ms);
	phaser.feedback = linear(pot2, 0, 0.75);

	pot3 = 4*pot3*pot3;				// 0..4, center=1
	phaser.center_f = linear(pot3, 220, 880);	// 220Hz .. ~3kHz (center at 880)
	phaser.octaves = 2;				// 55Hz .. ~12kHz
	phaser.Q = linear(pot4, 0.25, 2);

	fprintf(stderr, "phaser:");
	fprintf(stderr, " lfo=%g ms", ms);
	fprintf(stderr, " center_f=%g Hz", phaser.center_f);
	fprintf(stderr, " feedback=%g", phaser.feedback);
	fprintf(stderr, " Q=%g\n", phaser.Q);
}

float phaser_step(float in)
{
	float lfo = lfo_step(&phaser.lfo, lfo_triangle);
	float freq = fastpow(2, lfo*phaser.octaves) * phaser.center_f;
	float out;

	_biquad_allpass_filter(&phaser.coeff, freq, phaser.Q);

	out = in + phaser.feedback * phaser.s3[0];
	out = biquad_step_df1(&phaser.coeff, out, phaser.s0, phaser.s1);
	out = biquad_step_df1(&phaser.coeff, out, phaser.s1, phaser.s2);
	out = biquad_step_df1(&phaser.coeff, out, phaser.s2, phaser.s3);

	return limit_value(in + out);
}
