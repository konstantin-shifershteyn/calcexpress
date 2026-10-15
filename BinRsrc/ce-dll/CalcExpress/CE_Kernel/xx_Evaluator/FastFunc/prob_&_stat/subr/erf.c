
#define ERF_MAX_ATTEMPTS	90
#define	ERF_ACCURACY		1.0e-15

// ==============
// Error Function
// ==============
float64 __erf(float64 x)
{
	int32	i;
	float64	ap, sum, ds, xx;

	if (x> 5.75)	return  1.0;
	if (x<-5.75)	return -1.0;

	xx = x*x;

	if (xx==0.0)	return  0.0;

	ap = 0.5;
	ds = sum = 2.0;

	for (i = ERF_MAX_ATTEMPTS; i; i--) {
		ap  += 1.0;
		ds  *= xx/ap;
		sum += ds;
		if (fabs(ds/sum) < ERF_ACCURACY) {
			xx = sum * exp(log(xx)/2 - xx - _LN_GAMMA_05_);
			goto out;
		}
	}
	xx = 1.0;
out:
	if (x<0.0)
		return -xx;
	return	xx;
}

// ============================
// Complementary Error Function
// ============================
float64 __erfc(float64 x)
{
	return 1.0 - erf(x);
}

#undef	ERF_MAX_ATTEMPTS
#undef	ERF_ACCURACY
