/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
 

// ==========================================================================================
// =============================  Логарифмические Функции   =================================
// ==========================================================================================

// ===============================
// ===== EXP =====================
// ===============================
MgErr _exp_dbl ( _dbl_operand *arg)
{
	register	int32	size;
	register	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= exp(*nums);
	else		arg->scalar	= exp(arg->scalar);
	return noErr;
}
// =================================
// ===== POW10 =====================
// =================================
MgErr _pow10_dbl ( _dbl_operand *arg)
{
	register	int32	size;
	register	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= pow(10.0, *nums);
	else		arg->scalar	= pow(10.0, arg->scalar);
	return noErr;
}
// ================================
// ===== POW2 =====================
// ================================
MgErr _pow2_dbl ( _dbl_operand *arg)
{
	register	int32	size;
	register	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= pow(2.0, *nums);
	else		arg->scalar	= pow(2.0, arg->scalar);
	return noErr;
}
// ==============================
// ===== LN =====================
// ==============================
MgErr _ln_dbl ( _dbl_operand *arg)
{
	register	int32	size;
	register	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= log(*nums);
	else		arg->scalar	= log(arg->scalar);
	return noErr;
}
// =================================
// ===== LOG10 =====================
// =================================
MgErr _log10_dbl ( _dbl_operand *arg)
{
	register	int32	size;
	register	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= log10(*nums);
	else		arg->scalar	= log10(arg->scalar);
	return noErr;
}
// ================================
// ===== LOG2 =====================
// ================================
MgErr _log2_dbl ( _dbl_operand *arg)
{
	register	int32	size;
	register	float64	*nums;
			float64	__ln2__;

// При использовании _LN2_ как константы или переменной,
// предварительно вычесленной как _LN2_=log(2.0), результат получается
// отличным чем результат аналогичной LabVIEW функции.

	__ln2__ = log(2.0);

	if ( arg->nDims)
		for( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= log(*nums)       / __ln2__;
	else		arg->scalar	= log(arg->scalar) / __ln2__;
/*
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= log(*nums)       / _LN2_;//log(2);
	else		arg->scalar	= log(arg->scalar) / _LN2_;//log(2);
*/
	return noErr;
}

// =====================================================================================
//                                  Комплексные числа
// =====================================================================================

	_func1a_cdb	( _exp_cdb	, exp_cdb	);
	_func1a_cdb	( _pow2_cdb	, pow2_cdb	);
	_func1a_cdb	( _pow10_cdb	, pow10_cdb	);
	_func1a_cdb	( _ln_cdb	, ln_cdb	);
	_func1a_cdb	( _log2_cdb	, log2_cdb	);
	_func1a_cdb	( _log10_cdb	, log10_cdb	);

