
// ===================  Incomplete Beta Functions ====================

#define	MAX_I		100.0
#define	ACCURACY	1.0e-15
#define	TINY		1.0e-30

float64 ___betai_cont(float64 a, float64 b, float64 x)
{
	float64	M;
	float64	C, D, F, dF;
	float64	A, AB, APM2;

	AB = a + b;

	C  = 1.0;
	D  = 1.0 - (x * AB)/(a+1.0);	if (fabs(D) < TINY)	D = TINY;
	D  = 1.0/D;
	F  = D;

	for (M=1.0; M <= MAX_I; M += 1.0) {
		APM2 = a + (2.0 * M);

		// *** Even Part ***
		A  = (M*(b-M)*x) / (APM2*(APM2-1.0));
		C  = 1.0 + A / C;	if (fabs(C) < TINY)	C = TINY;
		D  = 1.0 + A * D;	if (fabs(D) < TINY)	D = TINY;
		D  = 1.0/D;
		F *= D*C;

		// *** Odd Part ***
		A  = -( ((a+M)*(AB+M)*x) / (APM2*(APM2+1.0)) );
		C  = 1.0 + A / C;	if (fabs(C) < TINY)	C = TINY;
		D  = 1.0 + A * D;	if (fabs(D) < TINY)	D = TINY;
		D  = 1.0/D;
		dF = D*C;
		F *= dF;
		if (fabs(dF-1.0) < ACCURACY) break;
	}
	return F;
}
#undef	MAX_I
#undef	ACCURACY
#undef	TINY

float64 __betai(float64 a, float64 b, float64 x)
{
	float64	pref;

	if (a <  0.0 || b <  0.0)	return _NaN_;
	if (x <= 0.0)			return 0.0;
	if (x >= 1.0)			return 1.0;

	pref = exp(___gamma_ln(a+b) - ___gamma_ln(a) - ___gamma_ln(b) +
			a*log(x) + b*log(1.0-x));

	if (x < (a+1.0)/(a+b+2.0))
		return	pref * ___betai_cont(a,b,x)/a;
	else
		return	1.0 - pref * ___betai_cont(b,a,1.0-x)/b;
}
