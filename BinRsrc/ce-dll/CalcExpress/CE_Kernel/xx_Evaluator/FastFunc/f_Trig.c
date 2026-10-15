/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#define __sign(x)	( !(x) ? 0: (x)>0 ? 1:-1			)

#define	__acot(x)	( _PI_2_ - atan(x) )

// *** эти две функции убрал из help
#define	__asec(x)	( atan((x)/sqrt((x)*(x)-1)) + __sign((x)-1) * _2ATAN1_		)
#define	__acsc(x)	( atan((x)/sqrt((x)*(x)-1)) + (__sign(x)-1) * _2ATAN1_		)
//#define	__acot(x)	( atan(x) + _2ATAN1_						)


#define	__acosh(x)	( log( (x)+sqrt((x)*(x)-1) )			)
#define	__asinh(x)	( log( (x)+sqrt((x)*(x)+1) )			)
#define	__atanh(x)	( log( (1+(x))/(1-(x)) ) / 2			)
#define	__acoth(x)	( log( ((x)+1)/((x)-1) ) / 2 			)

// *** эти две функции убрал из help
#define	__asech(x)	( log( (sqrt(-(x)*(x)+1) + 1)/(x) )		)
#define	__acsch(x)	( log( (__sign(x) * sqrt((x)*(x)+1)+1) / (x) )	)

#define	__sinc(x)	( x==0.0 ? 1 : sin(x)/(x)			)


// ==============================================================================================
// ================================  Тригонометрические Функции   ===============================
// ==============================================================================================

	_func1a_dbl(	_sin_dbl	, sin	);	// sin
	_func1a_dbl(	_cos_dbl	, cos	);	// cos
	_func1a_dbl(	_tan_dbl	, tan	);	// tan
	_func1a_dbl(	_sec_dbl	, 1/cos	);	// secant
	_func1a_dbl(	_csc_dbl	, 1/sin	);	// cosecant
	_func1a_dbl(	_cot_dbl	, 1/tan	);	// cotangent

	_func1a_dbl(	_asin_dbl	, asin	);	// asin
	_func1a_dbl(	_acos_dbl	, acos	);	// acos
	_func1a_dbl(	_atan_dbl	, atan	);	// atan
	_func1a_dbl(	_asec_dbl	, __asec );	// inverse secant
	_func1a_dbl(	_acsc_dbl	, __acsc );	// inverse cosecant
	_func1a_dbl(	_acot_dbl	, __acot );	// inverse cotangent

	_func1a_dbl(	_sinh_dbl	, sinh	);	// sinh
	_func1a_dbl(	_cosh_dbl	, cosh	);	// cosh
	_func1a_dbl(	_tanh_dbl	, tanh	);	// tanh
	_func1a_dbl(	_sech_dbl	, 1/cosh );	// hyperbolic secant
	_func1a_dbl(	_csch_dbl	, 1/sinh );	// hyperbolic cosecant
	_func1a_dbl(	_coth_dbl	, 1/tanh );	// hyperbolic cotangent

	_func1a_dbl(	_asinh_dbl	, __asinh );	// inverse hyperbolic sine
	_func1a_dbl(	_acosh_dbl	, __acosh );	// inverse hyperbolic cosine
	_func1a_dbl(	_atanh_dbl	, __atanh );	// inverse hyperbolic tangent
	_func1a_dbl(	_asech_dbl	, __asech );	// inverse hyperbolic secant
	_func1a_dbl(	_acsch_dbl	, __acsch );	// inverse hyperbolic cosecant
	_func1a_dbl(	_acoth_dbl	, __acoth );	// inverse hyperbolic cotangent

	_func1a_dbl(	_sinc_dbl	, __sinc );	// sinc


// ===============================================================================
// 			Комплексные числа
// ===============================================================================
#line 1 "Trig-Complex"

	_func1a_cdb	( _sin_cdb	, sin_cdb	);
	_func1a_cdb	( _cos_cdb	, cos_cdb	);
	_func1a_cdb	( _tan_cdb	, tan_cdb	);
	_func1a_cdb	( _sec_cdb	, sec_cdb	);
	_func1a_cdb	( _csc_cdb	, csc_cdb	);
	_func1a_cdb	( _cot_cdb	, cot_cdb	);

	_func1a_cdb	( _sinh_cdb	, sinh_cdb	);
	_func1a_cdb	( _cosh_cdb	, cosh_cdb	);
	_func1a_cdb	( _tanh_cdb	, tanh_cdb	);
	_func1a_cdb	( _sech_cdb	, sech_cdb	);
	_func1a_cdb	( _csch_cdb	, csch_cdb	);
	_func1a_cdb	( _coth_cdb	, coth_cdb	);

	_func1a_cdb	( _asin_cdb	, asin_cdb	);
	_func1a_cdb	( _acos_cdb	, acos_cdb	);
	_func1a_cdb	( _atan_cdb	, atan_cdb	);

	_func1a_cdb	( _asinh_cdb	, asinh_cdb	);
	_func1a_cdb	( _acosh_cdb	, acosh_cdb	);
	_func1a_cdb	( _atanh_cdb	, atanh_cdb	);

	_func1a_cdb	( _sinc_cdb	, sinc_cdb	);
