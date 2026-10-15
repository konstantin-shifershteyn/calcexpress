
// ===================  Incomplete Gamma Functions ====================

#define	MAX_ITERATIONS	100
#define	ACCURACY	1.0e-15
#define	TINY		1.0e-30

float64 ___gammai_cont(float64 a, float64 x, float64 gamln)
{

	int32	i;
	float64	A,B,C,D,F,dF;

	B  = x + 1.0 - a;
	C  = 1.0/TINY;
	D  = 1.0/B;
	F  = D;

	for (i=1; i<MAX_ITERATIONS; i++) {
		A  = (float64)i * ((float64)i - a);
		B += 2.0;
		C  = B - A/C;	if (fabs(C) < TINY)	C = TINY;
		D  = B - A*D;	if (fabs(D) < TINY)	D = TINY;
		D  = 1.0/D;
		dF = D * C;
		F *= dF;
		if (fabs(dF-1.0) < ACCURACY)	break;
	}
	return	1.0 - exp(-x + a*log(x) - gamln) * F;
}

// ==================================================

float64 ___gammai_ser(float64 a, float64 x, float64 gamln)
{
	int32	i;
	float64	n, sum, ds;

	n  = a;
	ds = sum = 1.0/a;

	for (i = MAX_ITERATIONS; i; i--) {
		n   += 1.0;
		ds  *= x/n;
		sum += ds;
		if (fabs(ds/sum) < ACCURACY)
			break;
	}
	x = sum * exp(a * log(x) - x - gamln);
	return x;
}

// ==================================================

float64 ___gammai(float64 a, float64 x, float64 gamln)
{
	if (a <= 0.0)	return	_NaN_;
	if (x <= 0.0)	return	0.0;

	if (x < (a+1.0))
		return ___gammai_ser (a, x, gamln);
	else
		return ___gammai_cont(a, x, gamln);
}

// ==================================================

float64	__gammai(float64 a, float64 x)
{
	return	___gammai(a, x, ___gamma_ln(a));
}

#undef	MAX_ITERATIONS
#undef	ACCURACY
#undef	TINY
