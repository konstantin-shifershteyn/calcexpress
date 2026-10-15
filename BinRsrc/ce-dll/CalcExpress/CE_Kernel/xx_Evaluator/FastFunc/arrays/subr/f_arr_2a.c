/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

 // ==========================================================
 // General handler for array functions with two arguments
 // ==========================================================
MgErr f_arr_1ar2dbl(
			_CE_Instance	*ip,
			_FastFuncInfo	*fInfo,
			int32			arg_qty,	// кол-во аргументов
			LVBoolean		*err_f )
{
	MgErr			err = noErr;
	_Operand		*arg1, *arg2;
	_dbl_operand	opnum;
	_str_operand	opstr;
	Bool32			itComplex = FALSE;
	float64			num = 0;
	int32			idx;

	opnum.alloc	= opstr.alloc	= FALSE;
	opnum.erMsg	= opstr.erMsg	= NULL;

	idx = ip->OpdNum - arg_qty + 1;
	arg1 = OpdStack_EPnt(ip, idx);	// pointer to the 1-st argument, it is alos pointer to the result

	// value of the 1-st argument
	// ----------------------------------
	if (arg1->type == OPTYPE_IDENT) {
		err = GetVariable( ip, arg1, (_OperandBase*)arg1, err_f);
		if (err || *err_f)	goto out;
	}

	// value of the 2-nd argument
	// ----------------------------------
	if (arg_qty >= 2) {
		arg2 = OpdStack_EPnt(ip, ++idx);	// pointer to the 2-nd argument
		if (arg2->type == OPTYPE_IDENT) {
			err = GetVariable( ip, arg2, (_OperandBase*)arg2, err_f);
			if (err || *err_f)	goto out;
		}
		if (arg2->type != OPTYPE_DBL) {
			opnum.erMsg = err_InvArgType2_needScal;
			goto	test_err;
		}
		num = arg2->num;
	}

	// === EXECUTE ===
	switch(arg1->type) {
		case OPTYPE_A2DBL :
		case OPTYPE_A2CDB :
		case OPTYPE_A1DBL :
		case OPTYPE_A1CDB :
			_prepare_operand((_OperandBase*)arg1, &opnum, &itComplex);

			fInfo->exec_dbl(&opnum, NULL, num);

			_save_dimensions_info(&opnum, &(arg1->str));
			arg1->type = opnum.ResType;
			break;

		case OPTYPE_A2STR :
		case OPTYPE_A1STR :
			if (err = _prepare_strings((_OperandBase*)arg1, &opstr))
				goto out;
			fInfo->exec_dbl(NULL, &opstr, num);
			if (err = _save_strings(&opstr, &(arg1->str)))
				goto out;
			arg1->num  = 0;
			arg1->type = opstr.ResType;
			break;
		default:
			opnum.erMsg = err_InvArgType1_needArr;
			break;
	}
test_err:
	if (opnum.erMsg) {
		err = CE_Error( ip, "%s", (LStrHandle)opnum.erMsg, NULL );
		*err_f = LVTRUE;
		goto	out;
	}
	if (opstr.erMsg) {
		err = CE_Error( ip, "%s", (LStrHandle)opstr.erMsg, NULL );
		*err_f = LVTRUE;
		goto	out;
	}
out:
	// *** decrease the stack size ***
	ip->OpdNum -= arg_qty - 1;

	if (opstr.alloc)	DSDisposePtr(opstr.array);
	if (opnum.alloc)	DSDisposePtr(opnum.array);
	return noErr;
}


// =======================================================================
// ======  Частные исполнители функций массивов с двумя аргументами  =====
// =======================================================================


// ============================
//	shift(array, n)
// ============================
MgErr _arr_shift(_dbl_operand *nop, _str_operand *sop, float64 n)
{
	int32	elNums, elSize;
	int32	nsh;
	MgErr	err = noErr;

	if (!sop && !nop)	return noErr;

	nsh = _DblToI32(n);
	if (!nsh)	return noErr;

	if (sop) {
		int32	i;

		elNums	= sop->size1 * sop->size2;
		err = RotateBlock(sop->array, elNums, sizeof(LStr*), nsh);

		// clear strings in the tail
		i = 0;
		if (abs(nsh) >= elNums)
			nsh = elNums;
		if (nsh < 0) {
			nsh = abs(nsh);	i = elNums - nsh;
		}
		while (nsh-- ) {
			LStrLen(sop->array[i++]) = 0;
		}
	}
	else {
		elNums	= nop->size1 * nop->size2;
		switch (nop->ResType) {
			case OPTYPE_A2DBL :
			case OPTYPE_A1DBL : elSize =   sizeof(float64);	break;
			case OPTYPE_A2CDB :
			case OPTYPE_A1CDB : elSize = 2*sizeof(float64);	break;
			default	:	return noErr;
		}
		ShiftBlock(nop->array, elNums, elSize, nsh);
	}
	return err;
}

// ============================
//	rotate(array, n)
// ============================
MgErr _arr_rotate(_dbl_operand *nop, _str_operand *sop, float64 n)
{
	int32	elNums, elSize;
	void	*pnt;

	if (!sop && !nop)	return noErr;

	if (sop) {
		pnt	= sop->array;
		elNums	= sop->size1 * sop->size2;
		elSize	= sizeof(LStr*);
	}
	else {
		pnt	= nop->array;
		elNums	= nop->size1 * nop->size2;
		switch (nop->ResType) {
			case OPTYPE_A2DBL :
			case OPTYPE_A1DBL : elSize =   sizeof(float64);	break;
			case OPTYPE_A2CDB :
			case OPTYPE_A1CDB : elSize = 2*sizeof(float64);	break;
			default	:	return noErr;
		}
	}
	return	RotateBlock(pnt, elNums, elSize, _DblToI32(n));
}


// ===============================================================
// sort(array [,cs])	Only 1D Array of DBL & 1D Array of Strings
// ===============================================================

int32 _arr_sort_cmp_dbl(float64 *a, float64 *b);
int32 _arr_sort_cmp_str(LStr **a, LStr **b);	// case sensitive
int32 _arr_sort_CMP_STR(LStr **a, LStr **b);	// case insensitive

MgErr _arr_sort(_dbl_operand *nop, _str_operand *sop, float64 n)
{
	int32	elNums, elSize;
	void	*pnt;

	if (!sop && !nop)	return noErr;

	if (sop) {
		pnt	= sop->array;
		elNums	= sop->size1 * sop->size2;
		elSize	= sizeof(LStr*);
		if (_DblToI32(n))
				QSort(pnt, elNums, elSize, (CompareProcPtr)_arr_sort_CMP_STR);
		else	QSort(pnt, elNums, elSize, (CompareProcPtr)_arr_sort_cmp_str);
	}
	else {
		pnt	= nop->array;
		elNums	= nop->size1 * nop->size2;
		switch (nop->ResType) {
			case OPTYPE_A1DBL : 	elSize = sizeof(float64);
						break;
			case OPTYPE_A2CDB :
			case OPTYPE_A1CDB :
					nop->erMsg = "Can't sort complex numbers";
					return noErr;

			case OPTYPE_A2DBL :
			default	:	nop->erMsg = err_InvArgType_needArr1D;
					return noErr;
		}
		QSort(pnt, elNums, elSize, (CompareProcPtr)_arr_sort_cmp_dbl);
	}
	return noErr;
}

// *** функция сравнения dbls для QSort()
int32 _arr_sort_cmp_dbl(float64 *a, float64 *b)
{
	if (*a > *b)	return  1;
	if (*a < *b)	return -1;
	return 0;
}
// *** функция сравнения strings для QSort() : Case Sensitive
int32 _arr_sort_cmp_str(LStr **a, LStr **b)
{
	return LStrCmp(*a, *b);
}
// *** функция сравнения strings для QSort() : Case Insensitive
int32 _arr_sort_CMP_STR(LStr **a, LStr **b)
{
	int32	l1 = LStrLen(*a);
	int32	l2 = LStrLen(*b);

	if ( !StrNCaseCmp( LStrBuf(*a), LStrBuf(*b), __min(l1,l2)) )
		return 0;
	if (l1 > l2)
		return 1;
	return -1;
}
