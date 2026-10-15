/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <celib\cdb.h>


// This definition (if not commented) disables use of 'complex' class to avoid a use of msvcp60.dll
//#define	CDB_CPP_DO_NOT_USE_COMPLEX


#ifdef CDB_CPP_DO_NOT_USE_COMPLEX

#pragma message ( "!!! WARNING: Incomplete complex numbers support !!! " )

// ***************************************************
// Stubs to avoid use of 'complex' class (msvcp60.dll)
// ***************************************************

extern "C" int add_cdb	( register _cdb_ *op1, register _cdb_ *op2, register _cdb_ *res)
{
	res->re = op1->re + op2->re;
	res->im = op1->im + op2->im;
	return 0;
}
extern "C" int sub_cdb	( register _cdb_ *op1, register _cdb_ *op2, register _cdb_ *res)
{
	res->re = op1->re - op2->re;
	res->im = op1->im - op2->im;
	return 0;
}
extern "C" int mul_cdb	( register _cdb_ *op1, register _cdb_ *op2, register _cdb_ *res)
{
	res->re = op1->re * op2->re - op1->im * op2->im;
	res->im = op1->re * op2->im + op1->im * op2->re;
	return 0;
}
extern "C" int div_cdb	( register _cdb_ *op1, register _cdb_ *op2, register _cdb_ *res)
{
	res->re = (op1->re*op2->re + op1->im*op2->im) / (op2->re*op2->re + op2->im*op2->im);
	res->im = (op1->im*op2->re - op1->re*op2->im) / (op2->re*op2->re + op2->im*op2->im);
	return 0;
}

extern "C" int rcpl_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int sqrt_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }

extern "C" int exp_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int pow2_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int pow10_cdb( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int pow_cdb	( register _cdb_ *x, register _cdb_ *y, register _cdb_ *out)	{ return 0; }

extern "C" int ln_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int log2_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int log10_cdb( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int log_cdb	( register _cdb_ *x, register _cdb_ *y, register _cdb_ *out)	{ return 0; }

extern "C" int sin_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int cos_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int tan_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int cot_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int sec_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int csc_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }

extern "C" int sinh_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int cosh_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int tanh_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int coth_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int sech_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int csch_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }

extern "C" int asin_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int acos_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int atan_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int atan2_cdb( register _cdb_ *x, register _cdb_ *y, register _cdb_ *out)	{ return 0; }

extern "C" int asinh_cdb( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int acosh_cdb( register _cdb_ *x, register _cdb_ *out)			{ return 0; }
extern "C" int atanh_cdb( register _cdb_ *x, register _cdb_ *out)			{ return 0; }

extern "C" int sinc_cdb	( register _cdb_ *x, register _cdb_ *out)			{ return 0; }



#else

// *********************************************************
// Implementation of complex functions using 'complex' class
// *********************************************************

// disable VC6.0 specific warning: 
// non – DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
// this warning did not happen in VC5.0
#pragma	warning( disable : 4275 )

#include <complex>
using namespace std;


// ====================
// ===== add(x,y) =====
// ====================
extern "C" int add_cdb( register _cdb_ *x, register _cdb_ *y, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re,x->im) + complex<long double>(y->re,y->im);
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ====================
// ===== mul(x,y) =====
// ====================
extern "C" int mul_cdb( register _cdb_ *x, register _cdb_ *y, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re,x->im) * complex<long double>(y->re,y->im);
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ====================
// ===== sub(x,y) =====
// ====================
extern "C" int sub_cdb( register _cdb_ *x, register _cdb_ *y, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re,x->im) - complex<long double>(y->re,y->im);
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ====================
// ===== div(x,y) =====
// ====================
extern "C" int div_cdb( register _cdb_ *x, register _cdb_ *y, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re,x->im) / complex<long double>(y->re,y->im);
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// ===== RCPL(x) =====
// ===================
extern "C" int rcpl_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num = 1.0L/complex<long double>(x->re, x->im);
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// ===== SQRT(x) =====
// ===================
extern "C" int sqrt_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re, x->im);
	num = sqrt(num);
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}

// ==========================================================================

// ===================
// =====  EXP(x) =====
// ===================
extern "C" int exp_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num = exp(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ====================
// ===== Pow10(x) =====
// ====================
extern "C" int pow10_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num = pow(10.0L, complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// ===== Pow2(x) =====
// ===================
extern "C" int pow2_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num = pow(2.0L, complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// =====  Pow(x) =====
// ===================
extern "C" int pow_cdb( register _cdb_ *x, register _cdb_ *y, register _cdb_ *out)
{
	complex<long double>		num = pow(complex<long double>(x->re, x->im), complex<long double>(y->re, y->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}

// ==========================================================================

// ===================
// =====  LN(x)  =====
// ===================
extern "C" int ln_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num = log(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ====================
// ===== Log10(x) =====
// ====================
extern "C" int log10_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num = log10(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// ===== Log2(x) =====
// ===================
#line	1 "Log2"
extern "C" int log2_cdb( register _cdb_ *x, register _cdb_ *out)
{
//	complex<long double>		num= log(complex<long double>(x->re, x->im))/_LN2_;
	complex<long double>		num= log(complex<long double>(x->re, x->im));

	num /= log(2.0);

	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// =====  Log(x) =====
// ===================
extern "C" int log_cdb( register _cdb_ *x, register _cdb_ *y, register _cdb_ *out)
{
	complex<long double>		num= log(complex<long double>(y->re, y->im))/log(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}

// ============================================================================

// ===================
// =====  Sin(x) =====
// ===================
extern "C" int sin_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num= sin(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// =====  Cos(x) =====
// ===================
extern "C" int cos_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num= cos(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// =====  Tan(x) =====
// ===================
extern "C" int tan_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num= sin(complex<long double>(x->re, x->im))/cos(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// =====  Cot(x) =====
// ===================
extern "C" int cot_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num= cos(complex<long double>(x->re, x->im))/sin(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// =====  Sec(x) =====
// ===================
extern "C" int sec_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num= 1.0L/cos(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// =====  Csc(x) =====
// ===================
extern "C" int csc_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num= 1.0L/sin(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}

// =======================================================================

// ===================
// ===== Sinh(x) =====
// ===================
extern "C" int sinh_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num= sinh(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// ===== Cosh(x) =====
// ===================
extern "C" int cosh_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num= cosh(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// ===== Tanh(x) =====
// ===================
extern "C" int tanh_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num = sinh(complex<long double>(x->re, x->im));
					num/= cosh(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// ===== Sech(x) =====
// ===================
extern "C" int sech_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num = 1.0L/cosh(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// ===== Csch(x) =====
// ===================
extern "C" int csch_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num = 1.0L/sinh(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// ===== Coth(x) =====
// ===================
extern "C" int coth_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>		num = cosh(complex<long double>(x->re, x->im));
					num/= sinh(complex<long double>(x->re, x->im));
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}

// ====================================================

// ===================
// ===== aSin(x) =====
// ===================
extern "C" int asin_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re, x->im);
	complex<long double>	tmp;

	tmp = num * complex<long double>(0,1) + sqrt( 1.0L - num * num );
	
	num = complex<long double>(0,-1) * log( tmp );

	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// ===== aCos(x) =====
// ===================
extern "C" int acos_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re, x->im);
	complex<long double>	tmp;

	tmp = num  + sqrt( num * num - 1.0L );
	
	num = complex<long double>(0,-1) * log( tmp );

	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ===================
// ===== aTan(x) =====
// ===================
extern "C" int atan_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re, x->im);
	complex<long double>	i	= complex<long double>(0,1);

	num  = log( (1.0L + i * num) / (1.0L - i * num) );
	num /= 2.0L * i;

	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ======================
// ===== aTan2(x,y) =====
// ======================
extern "C" int atan2_cdb( register _cdb_ *x, register _cdb_ *y, register _cdb_ *out)
{
	_cdb_	val;
	complex<long double>	num = complex<long double>(x->re, x->im) / complex<long double>(y->re, y->im);

	val.re = (double)num.real();
	val.im = (double)num.imag();
	return atan_cdb( &val, out);
}

// ====================================================

// ====================
// ===== aSinh(x) =====
// ====================
extern "C" int asinh_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re, x->im);

	num += sqrt( num * num + 1.0L);
	num  = log( num );
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ====================
// ===== aCosh(x) =====
// ====================
extern "C" int acosh_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re, x->im);

	num += sqrt( num * num - 1.0L);
	num  = log( num );
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}
// ====================
// ===== aTanh(x) =====
// ====================
extern "C" int atanh_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re, x->im);

	num  = log( (1.0L + num) / (1.0L - num) ) / 2.0L;
	out->re = (double)num.real();
	out->im = (double)num.imag();
	return 0;
}

// ======================================================

// ===================
// ===== Sinc(x) =====
// ===================
extern "C" int sinc_cdb( register _cdb_ *x, register _cdb_ *out)
{
	complex<long double>	num = complex<long double>(x->re, x->im);

	if ((x->re == 0.0) && (x->im == 0.0)) {
		out->re = 1.0L;
		out->im = 0.0L;
	}
	else {
		num = sin(num)/num;
		out->re = (double)num.real();
		out->im = (double)num.imag();
	}
	return 0;
}

#endif
