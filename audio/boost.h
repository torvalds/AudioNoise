struct {
	float mult, level, mix;
	struct biquad basscut, highcut;
} boost;

static float boost_db(signed char pot) { return linear_pot(pot, 0, 40); }
static float boost_level(signed char pot) { return linear_pot(pot, -40, 0); }
static float boost_basscut(signed char pot)  { return frequency_pot(pot, 10, 200); }		//  10 - 200 Hz
static float boost_highcut(signed char pot)  { return frequency_pot(pot, 1, 20); }		//  1 - 20 kHz
static float boost_mix(signed char pot) { return linear_pot(pot, 0, 1); }

void boost_init(signed char pot[10])
{
	boost.mult = db_to_level(boost_db(pot[0]));
	boost.level = db_to_level(boost_level(pot[1]));
	biquad_hpf(&boost.basscut, boost_basscut(pot[2]), 0.707);
	biquad_lpf(&boost.highcut, boost_highcut(pot[3])*1000, 0.707);
	boost.mix = boost_mix(pot[4]);
}

static struct effect boost_effect;

static float fold(float in, float level)
{
	float fold_scale = 0.5;
	boost_effect.intense = 1;
	for (;;) {
		float over = (in - level) * fold_scale;

		in = level - over;
		if (in >= -level)
			return in;

		over = (in + level) * fold_scale;
		in = - level - over;
		if (in <= level)
			return in;
	}
}

static float boost_step(float in)
{
	float out = in * boost.mult;

	out = biquad_step(&boost.basscut, out);
	out = biquad_step(&boost.highcut, out);

	if (out > boost.level)
		out = fold(out, boost.level);
	else if (out < -boost.level)
		out = -fold(-out, boost.level);

	return linear(boost.mix, in, out);
}

static struct effect boost_effect = {
	.name = "Boost",
	.short_name = "BO",
	.init = boost_init,
	.step = boost_step,
	.pots = {
		{ "Boost", desc_dB, boost_db, -100, },
		{ "Level", desc_dB, boost_level, 100 },
		{ "Basscut", desc_Hz, boost_basscut, 0 },
		{ "Highcut", desc_kHz, boost_highcut, 0 },
		{ "Mix", desc_none, boost_mix },
	}
};
