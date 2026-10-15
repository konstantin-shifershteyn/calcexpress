
static	float64	gam_coef[6] = {
				 7.618009172947146e+1,
				-8.650532032941677e+1,
				 2.401409824083091e+1,
				-1.231739572450155e+0,
				 1.208650973866179e-3,
				-5.395239384953000E-6 };


// =================================================
// Computes Natural Logarithm of Gamma Function of x
// =================================================
float64 ___gamma_ln(float64 xin)
{
	float64	x, sum, gam;
register int32	i,f;

	if (f=(xin < 0.999))	x = -xin;
	else			x = xin-1.0;

	gam = x+5.5;
	gam = (x+0.5)*log(gam) - gam + _LN_SQRT_2_PI_;

	sum = 1.000000000190015e+0;
	for(i=0; i<6; i++)
		sum += gam_coef[i]/(x+=1.0);
	gam += log(sum);

	if (f)
		return _LN_PI_ - ( gam + log(sin(xin * _PI_)) );
	return gam;
}

// =================================================
// The function computes Gamma Function of x
// =================================================
float64 ___gamma_x(float64 xin)
{
	float64	x, sum, gam;
register int32	i,f;

	if (f=(xin < 0.999))	x = -xin;
	else			x = xin-1.0;

	gam = x+5.5;
	gam = pow(gam, x+0.5)* exp(-gam) * _SQRT_2_PI_;

	sum = 1.000000000190015e+0;
	for(i=0; i<6; i++)
		sum += gam_coef[i]/(x+=1.0);
	gam *= sum;

	if (f)
		return _PI_/(gam * sin(xin*_PI_));
	return gam;
}

// =================================================
// Returns Gamma Function of x
// =================================================
float64 __gamma(float64 x)
{
	if (x < 140)
		return ___gamma_x(x);
	else
		return exp(___gamma_ln(x));
}

// ============================
// Returns Beta Function of z,w
// ============================
float64 __beta(float64 z, float64 w)
{
	if ((z>0) && (w>0) && (z+w > 100))
		return exp(___gamma_ln(z) + ___gamma_ln(w) - ___gamma_ln(z+w));
	else
		return ___gamma_x(z) * ___gamma_x(w) / ___gamma_x(z+w);
}


// ======================
// Returns Factorial of x
// ======================
float64 __factr(register int32 x)
{
	if (x < 0)
		return _NaN_;
	if (x > FACTR_MAX_INT)
		return exp(___gamma_ln(1.0+(float64)x));
	if (FactrTable[x] == 0.0)
		MathConst_Initialize();		// выполняется в CINLoad()
	return FactrTable[x];
}

