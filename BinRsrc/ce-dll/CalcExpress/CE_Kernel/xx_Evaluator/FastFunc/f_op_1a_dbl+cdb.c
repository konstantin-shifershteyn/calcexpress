/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ============================================================================
// ======================   Функции унарных операций   ========================
// ============================================================================


// >>> #CE_3.0 2025-03-18
// ===== INCREMENT =====================
MgErr _inc_dbl ( _dbl_operand *arg)  {
	register	int32	size;
	register	float64	*nums;
	if ( arg->nDims)
		for ( nums= arg->array, size= arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		+= 1;
	else	arg->scalar	+= 1;
	return noErr;
}
// ===== DECREMENT =====================
MgErr _dec_dbl ( _dbl_operand *arg)  {
	register	int32	size;
	register	float64	*nums;
	if ( arg->nDims)
		for ( nums= arg->array, size= arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		-= 1;
	else	arg->scalar	-= 1;
	return noErr;
}
// <<< #CE_3.0 2025-03-18



// ===== Унарный ПЛЮС =====================
MgErr _pos_dbl ( _dbl_operand *arg)
{
	return noErr;
}

// ===== Унарный МИНУС =====================
MgErr _neg_dbl ( _dbl_operand *arg)  {
	register	int32	size;
	register	float64	*nums;
	if ( arg->nDims)
		for ( nums= arg->array, size= arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= -(*nums);
	else	arg->scalar	= -(arg->scalar);
	return noErr;
}

// ===== Логический NOT =====================
MgErr _lnot_dbl ( _dbl_operand *arg)
{
	register	int32	size;
	register	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= !(_DblToI32Bits(*nums));
	else	arg->scalar	= !(_DblToI32Bits(arg->scalar));
	return noErr;
}

// ===== Поразрядный NOT =====================
MgErr _bnot_dbl ( _dbl_operand *arg)
{
	register	int32	size;
	register	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= ~(_DblToI32Bits(*nums));
	else	arg->scalar	= ~(_DblToI32Bits(arg->scalar));
	return noErr;
}

// ======================================================================================
// =============================  Комплексные числа  ====================================
// ======================================================================================

// ===== УНАРНЫЙ МИНУС =====
MgErr _neg_cdb ( _dbl_operand *arg)  {
	register	int32	size;
	register	float64	*nums;

	for ( nums= arg->array, size= 2*(arg->size1*arg->size2); size; size--, nums++)
		*nums		= -(*nums);
	return noErr;
}

// ===== УНАРНЫЙ ПЛЮС =====
MgErr _pos_cdb ( _dbl_operand *arg)
{
	return noErr;
}
