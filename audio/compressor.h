//
// Compressor Effect
//

static struct {
	// Pot values (pre-computed for step calculation convenience)
	float level, attack, release, ratio, boost;

	// Compressor state
	float env, compression;
} compressor = {
	.level = 1.0,
	.compression = 1.0
};

static float comp_pot0_level(signed char pot)
{
	return linear_pot(pot, -40.0f, 0.0f);	// -40dB to 0dB.
}

static float comp_pot1_attack(signed char pot)
{
	return linear_pot(pot, 2.0f, 100.0f);	// 2ms to 100ms
}

static float comp_pot2_release(signed char pot)
{
	return linear_pot(pot, 50.0f, 500.0f); // 50ms to 500ms
}

static float comp_pot3_ratio(signed char pot)
{
	return linear_pot(pot, 1.0f, 20.0f);	// 1.0 to 20.0
}

static float comp_pot4_boost(signed char pot)
{
	return linear_pot(pot, 0.0f, 24.0f);	// 0dB to +24dB
}

static inline void compressor_init(signed char pot[10])
{
	float level_db = comp_pot0_level(pot[0]);
	compressor.level = db_to_level(level_db);

	float attack_ms = comp_pot1_attack(pot[1]);
	compressor.attack = time_constant(attack_ms);

	float release_ms = comp_pot2_release(pot[2]);
	compressor.release = time_constant(release_ms);

	float ratio = comp_pot3_ratio(pot[3]);
	compressor.ratio = 1.0f - (1.0f / ratio);

	float boost_db = comp_pot4_boost(pot[4]);
	compressor.boost = db_to_level(boost_db);
}

// This may be overkill, but we're doing this by the book
static inline float mypow(float a, float b)
{
	return pow2(log2f(a) * b);
}

static struct effect compressor_effect;

static inline float compressor_step(float in)
{
	// Envelope follower
	float abs_val = fabsf(in);
	float coef = (abs_val > compressor.env) ? compressor.attack : compressor.release;
	compressor.env = linear(coef, abs_val, compressor.env);

	// Compression calculation.
	//
	// When we go over the compression level, we calculate a
	// target compression multiplier:
	//
	//	target = (level / env) ^ (1 - 1/ratio)
	//
	float target = 1.0f;
	if (compressor.env > compressor.level) {
		target = mypow(compressor.level / compressor.env, compressor.ratio);
		compressor_effect.intense = 1;
	}

	// ... and then we smooth the compression factor to prevent clicks
	compressor.compression = linear(0.01f, compressor.compression, target);

	return in * compressor.compression * compressor.boost;
}

static struct effect compressor_effect = {
	.name = "Compressor",
	.short_name = "CMPR",
	.init = compressor_init,
	.step = compressor_step,
	.pots = {
		{ "Level", desc_dB, comp_pot0_level, 0 },	// -20dB
		{ "Attack", desc_ms, comp_pot1_attack, -70 },	// ~15ms
		{ "Release", desc_ms, comp_pot2_release, -50 },	// ~150ms
		{ "Ratio", desc_x, comp_pot3_ratio, -60 },	// ~4.8 ratio
		{ "Boost", desc_dB, comp_pot4_boost, -50 },	// ~6dB makeup
	}
};
