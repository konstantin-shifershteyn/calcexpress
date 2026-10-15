/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// тело в конце файла
Bool32 Proc_Comma( _CE_Instance	*ip );

// =====================================
// Выполняется если входная операция ','
// =====================================
MgErr PO_Comma(	_CE_Instance	*ip,
		int32		*expIdx,
		_Operation	*op,
		Bool32		*prev_operand,
		LVBoolean	*err_f)
{
	_Operation	*otp;		// temporary
	MgErr		err = noErr;

	if (!(*prev_operand)) {
		err = CE_Error(ip, "Missing operand before ','", NULL,NULL);
		*err_f = LVTRUE;
		goto out;
	}
	*prev_operand = FALSE;

	// ================================================
	// Вытолкнуть из стека все операции кроме '(' и '['
	// ================================================
	for(;;) {
		otp = OpsStack_TopPnt(ip);	// операция в вершине стека
		if (!otp)			// если стек пуст, то поместить ',' в стек
			goto SaveOp;

		if (op->prior <= otp->prior) {	// НЕ '(' или '['
			OpsStack_DecLen(ip);	// уменьшить размер стека
			macro_PutToExpression(otp);	// перенести из стека в выражение
			continue;
		}
		// НАШЛИ '(' или '[' .  !!! оставляем их в стеке !!!

		if ( Proc_Comma(ip) ) {
SaveOp:			err = OpsStack_Push(ip, op);	// заносим ',' в стек
		}
		break;
	}
out:
	return	err;
}

// #######################################################

Bool32 Proc_Comma( _CE_Instance	*ip )
{
	_FuncIdxOp	*fip;

	fip = FIOStack_TopPnt(ip);
	if (!fip)		return TRUE;	//Если не в функции, то операцию в стек
	if (fip->brLevel > 1)	return TRUE;	//Если уровень скобок > 1, то операцию в стек

	fip->w_arg	= TRUE;			// Признак наличия аргументов
	fip->nArg	+= 1;			// количество аргументов

	if (fip->type == FIO_TYPE_IDXOP) {	// для оператора индексирования
		fip->misc	= IDX_SUBSCRIPT_2D;	// результат - 2D
	}
	return FALSE;
}
