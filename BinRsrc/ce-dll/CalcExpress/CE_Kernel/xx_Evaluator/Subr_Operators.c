/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#define					__CMP_NUM(n1,n2)		((int32)((n1)==(n2)))
__forceinline	int32	__cmp_num(float64 *n1, float64 *n2)		{ return (*n1 == *n2); }

// ====================================
// compares operand values. operands must have the same datatype
// ====================================
__inline int32	__compare_opd( _OperandBase *Lopd, _OperandBase *Ropd )
{
	_dbl_operand	arg1, arg2;
	float64			*p1, *p2;
	int32			size;
	Bool32			itComplex = FALSE;

	if (_prepare_operand(Lopd, &arg1, &itComplex)) goto nonNum;
	if (_prepare_operand(Ropd, &arg2, &itComplex)) goto nonNum;

	// количество элементов в массиве
	// ==============================
	size = arg1.size1 * arg1.size2;
	if (size != arg2.size1 * arg2.size2)	//если размер разный, то НЕ РАВНЫ
		return 0;

	if (itComplex)	size *= 2;		//для комплексных чисел удваиваем

	p1 = arg1.array;
	p2 = arg2.array;

	while(size--) {
//		if (!__cmp_num(p1++, p2++))
		if (!__CMP_NUM(*p1++, *p2++))
			return 0;
	}
	return 1;
nonNum:
	// Для не числовых типов данных
	// ============================
	return	LStr_Cmp(Lopd->str,Ropd->str);
}

// ------------------------------------------------------
// Функция выполняет проверку на равенство двух операндов
// ------------------------------------------------------
// >>> #CE_3.0.5 2026-07-12
MgErr CompareOperandsForEquality(
			_CE_Instance	*ip,
			_Operand		*Lopd,
			_Operand		*Ropd,
			LVBoolean		*err_f)
{
	uInt32			VMLOCKFLAGS = 0;
	_OperandBase	*lValue, *rValue;
	int32			value = 0;
	LVBoolean		errf = LVFALSE;
	MgErr			err;

	// ### preparation: left operand
	err = OpdStack_OpdVarDataRef(ip, TRUE, Lopd, &lValue, &errf, &VMLOCKFLAGS, OPDVARDATAREF_LOCKSOFT);
	if (err || errf)
		goto out;
	// ### preparation: right operand
	err = OpdStack_OpdVarDataRef(ip, TRUE, Ropd, &rValue, &errf, &VMLOCKFLAGS, OPDVARDATAREF_LOCKSOFT);
	if (err || errf)
		goto out;

	// ### comparison
	if (lValue->type != rValue->type) {
		errf = LVTRUE;
		err = CE_Error(ip, "Operands must have the same datatype in the equality operator",NULL,NULL);
		goto out;
	}
	if (lValue->type == OPTYPE_DBL) {
//		value = __cmp_num( &(lValue->num), &(rValue->num) );
		value = __CMP_NUM( lValue->num, rValue->num );
	}
	else {
		value = __compare_opd(lValue, rValue);
	}
out:
#pragma warning (push)
#pragma warning (disable: 26110)	// MS Code Analyser can't understand the idea
	if (VMLOCKFLAGS != OPDVARDATAREF_NOLOCK)	VARMAN_UNLOCK_SHARED();
#pragma warning (pop)

	Lopd->type = OPTYPE_DBL;
	Lopd->num  = (float64)value;
	// >>> #CE_3.0.5 2025-07-15 : disabled: indexation is only used with OPTYPE_IDENT, but if we keep var name in the Operands Stack, we also must keep its indexation
	//_IndexInfo_ResetIndexation(Lopd);		// CHECK_ME: reset indexation in Lopd ? 
	if (errf) {
		*err_f = errf;
	}
	return err;
}
// <<< #CE_3.0.5 2026-07-12
