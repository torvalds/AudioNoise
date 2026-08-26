//
// Calculate biquad coefficients
//
struct biquad_coeff {
	float b0, b1, b2;
	float a1, a2;
};

struct biquad_state {
	float x[2], y[2];
};

struct biquad {
	struct biquad_coeff coeff;
	struct biquad_state state;
};

// Direct form 1 may need more state than the "canonical" DF2,
// but gets noisy much quicker.
static inline float biquad_step_df1(const struct biquad_coeff *c, float in, float x[2], float y[2])
{
	float out = c->b0*in + c->b1*x[0] + c->b2*x[1] - c->a1*y[0] - c->a2*y[1];
	x[1] = x[0]; x[0] = in;
	y[1] = y[0]; y[0] = out;
	return out;
}

// A peaking biquad has a1==b1 and can avoid one multiply
static inline float biquad_peaking_step_df1(const struct biquad_coeff *c, float in, float x[2], float y[2])
{
	float out = c->b0*in + c->b1*(x[0]-y[0]) + c->b2*x[1] - c->a2*y[1];
	x[1] = x[0]; x[0] = in;
	y[1] = y[0]; y[0] = out;
	return out;
}

static inline float _biquad_peaking_step(const struct biquad_coeff *c, struct biquad_state *s, float x0)
{
	return biquad_peaking_step_df1(c, x0, s->x, s->y);
}

static inline float _biquad_step(const struct biquad_coeff *c, struct biquad_state *s, float x0)
{
	return biquad_step_df1(c, x0, s->x, s->y);
}

static inline void _biquad_lpf(struct biquad_coeff *res, const struct sincos w0, float Q)
{
	float alpha = w0.sin/(2*Q);
	float a0_inv = 1/(1 + alpha);
	float b1 = (1 - w0.cos) * a0_inv;

	res->b0 = b1 / 2;
	res->b1 = b1;
	res->b2 = b1 / 2;
	res->a1 = -2*w0.cos	* a0_inv;
	res->a2 = (1 - alpha)	* a0_inv;
}

static inline void _biquad_hpf(struct biquad_coeff *res, const struct sincos w0, float Q)
{
	float alpha = w0.sin/(2*Q);
	float a0_inv = 1/(1 + alpha);
	float b1 = (1 + w0.cos) * a0_inv;

	res->b0 = b1 / 2;
	res->b1 = -b1;
	res->b2 = b1 / 2;
	res->a1 = -2*w0.cos	* a0_inv;
	res->a2 = (1 - alpha)	* a0_inv;
}

static inline void _biquad_notch_filter(struct biquad_coeff *res, const struct sincos w0, float Q)
{
	float alpha = w0.sin/(2*Q);
	float a0_inv = 1/(1 + alpha);

	res->b0 = 1 		* a0_inv;
	res->b1 = -2*w0.cos	* a0_inv;
	res->b2 = 1		* a0_inv;
	res->a1 = -2*w0.cos	* a0_inv;
	res->a2 = (1 - alpha)	* a0_inv;
}

static inline void _biquad_bpf_peak(struct biquad_coeff *res, const struct sincos w0, float Q)
{
	float alpha = w0.sin/(2*Q);
	float a0_inv = 1/(1 + alpha);

	res->b0 = Q*alpha	* a0_inv;
	res->b1 = 0;
	res->b2 = -Q*alpha	* a0_inv;
	res->a1 = -2*w0.cos	* a0_inv;
	res->a2 = (1 - alpha)	* a0_inv;
}

static inline void _biquad_bpf(struct biquad_coeff *res, const struct sincos w0, float Q)
{
	float alpha = w0.sin/(2*Q);
	float a0_inv = 1/(1 + alpha);

	res->b0 = alpha		* a0_inv;
	res->b1 = 0;
	res->b2 = -alpha	* a0_inv;
	res->a1 = -2*w0.cos	* a0_inv;
	res->a2 = (1 - alpha)	* a0_inv;
}

static inline void _biquad_allpass_filter(struct biquad_coeff *res, const struct sincos w0, float Q)
{
	float alpha = w0.sin/(2*Q);
	float a0_inv = 1/(1 + alpha);

	res->b0 = (1 - alpha)	* a0_inv;
	res->b1 = (-2*w0.cos)	* a0_inv;
	res->b2 = 1;		// Same as a0
	res->a1 = res->b1;
	res->a2 = res->b0;
}

static inline void _biquad_peaking(struct biquad_coeff *res, const struct sincos w0, float Q, float gain)
{
	float A = sqrtf(gain);
	float alpha = w0.sin/(2*Q);
	float a0_inv = 1 / (1 + alpha/A);

	res->b0 = (1 + alpha*A)		* a0_inv;
	res->b1 = (-2*w0.cos)		* a0_inv;
	res->b2 = (1 - alpha*A)		* a0_inv;
	res->a1 = res->b1;
	res->a2 = (1 - alpha/A)		* a0_inv;
}

static inline void _biquad_loshelf(struct biquad_coeff *res, const struct sincos w0, float Q, float gain)
{
	float A = sqrtf(gain);
	float alpha = w0.sin/(2*Q);

	float ap1 = A + 1;
	float am1 = A - 1;
	float sqAmin2 = 2 * sqrtf(A) * alpha;
	float a0_inv = 1 / (ap1 + am1*w0.cos + sqAmin2);

	res->b0 =  A * (ap1 - am1*w0.cos + sqAmin2)	* a0_inv;
	res->b1 = 2*A* (am1 - ap1*w0.cos)		* a0_inv;
	res->b2 =  A * (ap1 - am1*w0.cos - sqAmin2)	* a0_inv;
	res->a1 =   -2*(am1 + ap1*w0.cos)		* a0_inv;
	res->a2 =      (ap1 + am1*w0.cos - sqAmin2)	* a0_inv;
}

static inline void _biquad_hishelf(struct biquad_coeff *res, const struct sincos w0, float Q, float gain)
{
	float A = sqrtf(gain);
	float alpha = w0.sin/(2*Q);

	float ap1 = A + 1;
	float am1 = A - 1;
	float sqAmin2 = 2 * sqrtf(A) * alpha;
	float a0_inv = 1 / (ap1 - am1*w0.cos + sqAmin2);

	res->b0 =  A * (ap1 + am1*w0.cos + sqAmin2)	* a0_inv;
	res->b1 =-2*A* (am1 + ap1*w0.cos)		* a0_inv;
	res->b2 =  A * (ap1 + am1*w0.cos - sqAmin2)	* a0_inv;
	res->a1 =    2*(am1 - ap1*w0.cos)		* a0_inv;
	res->a2 =      (ap1 - am1*w0.cos - sqAmin2)	* a0_inv;
}

static inline float biquad_step(struct biquad *bq, float x0)
{ return _biquad_step(&bq->coeff, &bq->state, x0); }

#define _w0(f) fastsincos((f)/SAMPLES_PER_SEC)

#define biquad_lpf(bq,f,Q) _biquad_lpf(&(bq)->coeff,_w0(f),Q)
#define biquad_hpf(bq,f,Q) _biquad_hpf(&(bq)->coeff,_w0(f),Q)
#define biquad_notch_filter(bq,f,Q) _biquad_notch_filter(&(bq)->coeff,_w0(f),Q)
#define biquad_bpf_peak(bq,f,Q) _biquad_bpf_peak(&(bq)->coeff,_w0(f),Q)
#define biquad_bpf(bq,f,Q) _biquad_bpf(&(bq)->coeff,_w0(f),Q)
#define biquad_allpass_filter(bq,f,Q) _biquad_allpass_filter(&(bq)->coeff,_w0(f),Q)
#define biquad_peaking(bq,f,Q,g) _biquad_peaking(&(bq)->coeff,_w0(f),Q,g)
#define biquad_lowshelf(bq,f,Q,g) _biquad_loshelf(&(bq)->coeff,_w0(f),Q,g)
#define biquad_highshelf(bq,f,Q,g) _biquad_hishelf(&(bq)->coeff,_w0(f),Q,g)
