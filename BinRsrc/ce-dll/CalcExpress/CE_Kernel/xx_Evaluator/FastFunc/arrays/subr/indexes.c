/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =========================================
//	ir( [idx1 [,idx2]] )
// =========================================
MgErr	_ir_idx(	_dbl_operand	*in,
			int32		qty,
			_dbl_operand	*out )
{
	int32	index1 = 0, index2 = 0x7fffffff;

	if (qty >= 1) {
		if (in->ResType != OPTYPE_DBL) { out->erMsg = err_InvArgType1_needScal; goto out; }
		index1 = _DblToI32(floor(in->scalar));
	}
	if (qty >= 2) {
		in += 1;
		if (in->ResType != OPTYPE_DBL) { out->erMsg = err_InvArgType2_needScal; goto out; }
		index2 = _DblToI32(floor(in->scalar));
	}
	out->ResType	= OPTYPE_IDXRANGE;
	out->nDims	= 0;
	out->size1	= out->size2 = 1;
	_PackIdxRangeToFloat(&(out->scalar), index1, index2);
out:
	return	noErr;
}

// =========================================
//	irr( [idx1 [,idx2]] )
// =========================================
MgErr	_irr_idx(	_dbl_operand	*in,
			int32		qty,
			_dbl_operand	*out )
{
	int32	index1 = 0x7fffffff, index2 = 0;

	if (qty >= 1) {
		if (in->ResType != OPTYPE_DBL) { out->erMsg = err_InvArgType1_needScal; goto out; }
		index1 = _DblToI32(floor(in->scalar));
	}
	if (qty >= 2) {
		in += 1;
		if (in->ResType != OPTYPE_DBL) { out->erMsg = err_InvArgType2_needScal; goto out; }
		index2 = _DblToI32(floor(in->scalar));
	}
	out->ResType	= OPTYPE_IDXRANGE;
	out->nDims	= 0;
	out->size1	= out->size2 = 1;
	_PackIdxRangeToFloat(&(out->scalar), index1, index2);
out:
	return	noErr;
}

// =========================================================================
//	ir1( idxRange )
// =========================================================================
MgErr	f_irb1(	_CE_Instance	*ip,
		_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
		int32		arg_qty,	//кол-во аргументов
		LVBoolean	*err_f)
{
	_Operand	*arg;
	float64		idx;
	MgErr		err = noErr;

	// *** т.к. функция одного аргумента
	arg = OpdStack_EPnt(ip, ip->OpdNum);	// указатель на аргумент, он же результат

	// *** Получить значение аргумента
	err = ff_GetArgumentValue(ip, arg, OPTYPE_IDXRANGE,
				"Invalid argument type (Index Range expected)", err_f);

	// *** определить значение
	idx = _UnpackIdxRangeFromFloat(&(arg->num), 0);

	// *** сохранить результат
	arg->type = OPTYPE_DBL;
	arg->num  = idx;

	// Размер стека не меняем, т.к. функция одного аргумента
	return err;
}
// =========================================================================
//	ir2( idxRange )
// =========================================================================
MgErr	f_irb2(	_CE_Instance	*ip,
		_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
		int32		arg_qty,	//кол-во аргументов
		LVBoolean	*err_f)
{
	_Operand	*arg;
	float64		idx;
	MgErr		err = noErr;

	// *** т.к. функция одного аргумента
	arg = OpdStack_EPnt(ip, ip->OpdNum);	// указатель на аргумент, он же результат

	// *** Получить значение аргумента
	err = ff_GetArgumentValue(ip, arg, OPTYPE_IDXRANGE,
				"Invalid argument type (Index Range expected)", err_f);

	// *** определить значение
	idx = _UnpackIdxRangeFromFloat(&(arg->num), 1);

	// *** сохранить результат
	arg->type = OPTYPE_DBL;
	arg->num  = idx;

	// Размер стека не меняем, т.к. функция одного аргумента
	return err;
}
