/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =====================================
// Выполняется если входная операция '['
// =====================================

MgErr PO_OIndex(
			_CE_Instance	*ip,
			int32			*expIdx,
			_Operation		*op,
			Bool32			*prev_operand,	// *may_be_func
			LVBoolean		*err_f)
{
	_Operation	*otp;	// >>> #CE_3.0 2025-03-25
	_FuncIdxOp	fi;
	MgErr		err;

	if (!(*prev_operand)) {
		err = CE_Error(ip, "Unexpected '['."
			" Subscript operator has to follow an operand", NULL,NULL);
		*err_f	= LVTRUE;
		return	err;
	}
	*prev_operand	= FALSE;

// >>> #CE_3.0 2025-03-25
	for(;;) {
		if ( !(otp = OpsStack_TopPnt(ip)) )	// операция в вершине стека
			break;							// если стек пуст
		if (otp->category == CAT_INCPOST) {
			OpsStack_DecLen(ip);	// уменьшить размер стека
			macro_PutToExpression(otp);	// перенести из стека в выражение
			//continue;
		}
		else
			break;
	}
// <<< #CE_3.0 2025-03-25

	if (err = OpsStack_Push(ip, op))	// Поместить '[' в вершину стека
		return	err;

	// Операция индексации массива
	fi.type		= FIO_TYPE_IDXOP;
	fi.brLevel	= 1;
	fi.nArg		= 1;
	fi.w_arg	= FALSE;
	fi.misc		= IDX_SUBSCRIPT_2D;	// результат - 2D

	return	FIOStack_Push(ip, &fi);		// занести в стек функций/индексов

// >>> #CE_3.0 2025-03-25
out:
	return err;
// <<< #CE_3.0 2025-03-25
}
