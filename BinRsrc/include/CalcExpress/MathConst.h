/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__MATH_CONST__
#define	__MATH_CONST__

#include <extcode.h>

#define	_USE_MATH_DEFINES	//!!!
#include <math.h>

MgErr MathConst_Initialize(void);


//#define	_NaN_			(log(-1.0))					// NaN

#define	_E_				M_E			//2.71828182845904523L		// e
#define _1_E_			(1/M_E)		//0.367879441171442322L		// 1/e
#define _LOG10_E_		M_LOG10E	//0.434294481903251828L		// log10(e)
#define _LOG2_E_	    M_LOG2E		//1.44269504088896340736L	// log2(e)
#define	_LN_10_			M_LN10		//2.30258509299404568L		// ln(10)
#define	_LN_2_			M_LN2		//0.693147180559945309L		// ln(2)

#define	_PI_			M_PI		//3.14159265358979324L		// pi
#define	_2PI_			(M_PI*2.0)	//6.28318530717958647L		// 2*pi
#define	_PI_2_			M_PI_2		//	1.57079632679489662L	// pi/2
#define _1_PI_			M_1_PI		//0.318309886183790671L		// 1/pi
#define _LN_PI_			1.14472988584940017						// ln(pi)

#define	_180_dev_PI_	57.2957795130823228			// 180/pi
#define	_PI_dev_180_	0.0174532925199432955		// pi/180

#define	_ATAN1_			0.785398163397448279		// atan(1)
#define	_2ATAN1_		1.57079632679489656			// 2 * atan(1)

// ---------------------

extern	float64	_NaN_;			// NaN value
extern	float64	_Inf_P_;		// +Inf value
extern	float64	_Inf_N_;		// -Inf value

extern	float64	_SQRT_PI_;		// square root of pi
extern	float64	_SQRT_2_PI_;	// square root of 2*pi

//extern	float64	_LN2_;			// log(2.0)
//extern	float64 _LN_PI_;		// log(pi)

extern	float64 _LN_SQRT_PI_;	// log(sqrt(pi))
extern	float64 _LN_SQRT_2_PI_;	// log(sqrt(2*pi))

extern	float64	_GAMMA_05_;		// gamma(0.5) == sqrt(pi)
extern	float64	_LN_GAMMA_05_;	// log(gamma(0.5))

// ---------------------

#define	FACTR_MAX_INT	40				// 
extern	float64	FactrTable[FACTR_MAX_INT + 1];		// table of factorials


#endif
