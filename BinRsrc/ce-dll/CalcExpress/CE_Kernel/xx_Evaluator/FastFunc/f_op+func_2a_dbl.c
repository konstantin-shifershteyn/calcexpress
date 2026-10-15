/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ===== low-level functions ===============

// --- Random(a,b) ---
float64	rnd_dbl_low(float64 op1, float64 op2) {	float64 rnd; RandomGen(&rnd); return rnd*op1+op2; }

// --- Logical XOR operator ---
float64	LogXOR_low(float64 op1, float64 op2)  {	return LogXOR( _DblToI32Bits(op1), _DblToI32Bits(op2) ); }

// --- u32sub(x1,x2) : Целочисленное беззнаковое вычитание ---
//float64	u32sub_low(float64 op1, float64 op2)  { return (uInt32)op1 - (uInt32)op2; }
float64	u32sub_low(float64 op1, float64 op2)  { return _DblToU32Bits(op1) - _DblToU32Bits(op2); }

// --- u32mod(x1,x2) : Целочисленный беззнаковый остаток ---
//float64	u32mod_low(float64 op1, float64 op2)  { return (uInt32)op1 % (uInt32)op2; }
float64	u32mod_low(float64 op1, float64 op2)  { return _DblToU32Bits(op1) % _DblToU32Bits(op2); }

// ============================================


// =====================================================
// 		Functions of Binary Operators
// =====================================================

_op2a_dbl (_pow_dbl		, pow		, _op2a_dbl_fdir	); // pow(x,y)
_op2a_dbl (_mul_dbl		, *			, _op2a_dbl_opdbl	); // Умножение
_op2a_dbl (_div_dbl		, / 		, _op2a_dbl_opdbl	); // Деление
_op2a_dbl (_mod_dbl		, fmod		, _op2a_dbl_fdir	); // Модуль
_op2a_dbl (_add_dbl		, +			, _op2a_dbl_opdbl	); // Сложение
_op2a_dbl (_sub_dbl		, -			, _op2a_dbl_opdbl	); // Вычитание

_op2a_dbl (_great_dbl	, >			, _op2a_dbl_cmpdbl	); // Больше
_op2a_dbl (_greateq_dbl	, >=		, _op2a_dbl_cmpdbl	); // Больше или равно
_op2a_dbl (_less_dbl	, <			, _op2a_dbl_cmpdbl	); // Меньше
_op2a_dbl (_lesseq_dbl	, <=		, _op2a_dbl_cmpdbl	); // Меньше или равно

_op2a_dbl (_and_dbl		, &			, _op2a_dbl_opi32	); // Bitwise-AND
_op2a_dbl (_or_dbl		, |			, _op2a_dbl_opi32	); // Bitwise-OR
_op2a_dbl (_xor_dbl		, ^			, _op2a_dbl_opi32	); // Bitwise-XOR
#pragma warning (push)
#pragma warning (disable: 6297)		// x << y causes this warning, in CE help we warn end-users about undefined result if y is out of 0..31
_op2a_dbl (_lshift_dbl	, <<		, _op2a_dbl_opi32	); // Bitwise-Shift-Left
#pragma warning (pop)
_op2a_dbl (_rshift_dbl	, >>		, _op2a_dbl_opi32	); // Bitwise-Shift-Right

_op2a_dbl (_land_dbl	, &&		, _op2a_dbl_opi32	); // Logical AND
_op2a_dbl (_lor_dbl		, ||		, _op2a_dbl_opi32	); // Logical OR
_op2a_dbl (_lxor_dbl	, LogXOR_low, _op2a_dbl_fdir	); // Logical XOR


// ---------  ФУНКЦИИ ----------

_op2a_dbl (_atan2_dbl	, atan2			, _op2a_dbl_fdir	); // atan2(x,y)
_op2a_dbl (_log_dbl		, log			, _op2a_dbl_f2rev	); // log(x,y)
_op2a_dbl (_random_dbl	, rnd_dbl_low	, _op2a_dbl_fdir	); // Random(scale,offset)

_op2a_dbl (_u32mod_dbl	, u32mod_low	, _op2a_dbl_fdir	); // u32mod(x1,x2)
_op2a_dbl (_u32sub_dbl	, u32sub_low	, _op2a_dbl_fdir	); // u32sub(x1,x2)


//_op2a_dbl (_ldexp_dbl	, ldexp			, _op2a_dbl_fdir	); // ldexp(x,exp)
_op2a_dbl (_ldexp_dbl	, ldexp			, _op2a_dbl_frint	); // ldexp(dbl x, int exp)

// =================================================================================

