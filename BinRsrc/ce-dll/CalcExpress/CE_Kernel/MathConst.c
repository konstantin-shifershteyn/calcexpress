/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include "MathConst.h"

// Некотрые математические константы
// =================================
// Они инициализируются в CINLoad
//

float64	_NaN_;
float64	_Inf_P_;
float64	_Inf_N_;

//float64	_LN2_	= 0.69314718055994531L;		// Ln(2)

float64	_SQRT_PI_;		// square root of pi
float64	_SQRT_2_PI_;	// square root of 2*pi

//float64 _LN_PI_;		// log(pi)

float64 _LN_SQRT_PI_;	// log(sqrt(pi))
float64 _LN_SQRT_2_PI_;	// log(sqrt(2*pi))

float64	_GAMMA_05_;		// gamma(0.5) == sqrt(pi)
float64	_LN_GAMMA_05_;	// log(gamma(0.5))

float64	FactrTable[FACTR_MAX_INT + 1];		// table of factorials


// ===============================================
// Функция инициализирует математические константы
// ===============================================

MgErr MathConst_Initialize(void)
{
	int32		i;
	union {
		uInt64	u64;
		float64	dbl;
	} dbl_u64_cast;

	// ### Special constants
	dbl_u64_cast.u64  = 0x7FF8000000000000ULL;		// NaN as U64 bitmap of float64
	_NaN_		= dbl_u64_cast.dbl;
	dbl_u64_cast.u64  = 0x7FF0000000000000ULL;		// +Inf as U64 bitmap of float64
	_Inf_P_		= dbl_u64_cast.dbl;
	dbl_u64_cast.u64  = 0xFFF0000000000000ULL;		// -Inf as U64 bitmap of float64
	_Inf_N_		= dbl_u64_cast.dbl;

	// ### Misc. math constants
	_SQRT_PI_	= sqrt(_PI_);
	_SQRT_2_PI_	= sqrt(_2PI_);

//	_LN2_		= log(2.0);
//	_LN_PI_		= log(_PI_);

	_LN_SQRT_PI_	= log(_SQRT_PI_);
	_LN_SQRT_2_PI_	= log(_SQRT_2_PI_);

	_GAMMA_05_		= _SQRT_PI_;
	_LN_GAMMA_05_	= _LN_SQRT_PI_;


	// Init table of factorials
	// ------------------------
	i = 0;
	FactrTable[i]	= 1.0;
	while(++i <= FACTR_MAX_INT)
		FactrTable[i] = FactrTable[i-1]*(float64)i;

	return noErr;
}
