/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

// ==========================================================
//	vch( [name] )
// ==========================================================
MgErr v_vch (	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,
				int32			arg_qty,	// кол-во аргументов
				LVBoolean		*err_f)
{
	_Operand	*arg;		// Указатель на операнд, он-же будет результатом
	int32		vcHandle = 0;
	uInt16		errc = 0;
	LVBoolean	errf = LVFALSE;
	MgErr		err  = noErr;

	// ------------------------------------------------------------
	// если вызвана без параметров, то возвращает текущий VC Handle
	// ------------------------------------------------------------
	if (!arg_qty) {
		return ff_PseudoArg(ip, (float64)ip->VCHandle);
	}

	arg = OpdStack_EPnt(ip, ip->OpdNum);	// указатель на аргумент, он же результат

	// =======================================
	// Определить имя контекста
	// =======================================
	err = ff_GetArgumentValue(ip, arg, OPTYPE_STR, "Invalid parameter type (String expected)", err_f);
	if (err || *err_f)	goto out;

	if (LStrLen(*(arg->str))) {
		// *** Вызвать VarMan
		err = vmCmd_vcNames_FindName(
				&vcHandle,				// handle
				arg->str,				// name
				&errf, &errc );
	}
	else {	// текущий контекст если имя - пустая строка
		vcHandle = ip->VCHandle;
	}
out:
	arg->type = OPTYPE_DBL;
	if (errf)	arg->num  = __vfunc_err_code(errc);
	else		arg->num  = (float64)vcHandle;

	// Размер стека не меняем, т.к. функция одного аргумента
	return err;
}