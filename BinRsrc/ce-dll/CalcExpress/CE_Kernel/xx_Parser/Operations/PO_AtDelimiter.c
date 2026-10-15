/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =====================================
// Выполняется если входная операция '@'
// =====================================
MgErr PO_AtDelimiter(	_CE_Instance	*ip,
			int32		*expIdx,
			_Operation	*op,
			Bool32		*prev_operand,
			LVBoolean	*err_f)
{
	int32		i;
	_Operation	*otp;		// temporary
	_FuncIdxOp	*fip;
	MgErr		err = noErr;

	if (!(*prev_operand)) {
		err = CE_Error(ip, "Missing operand before '@'", NULL,NULL);
		*err_f = LVTRUE;
		goto out;
	}
	*prev_operand = FALSE;

	// ================================================
	// Вытолкнуть из стека все операции кроме '(' и '['
	// ================================================
	for(;;) {
		otp = OpsStack_TopPnt(ip);	// операция в вершине стека

		if (!otp) {			// если стек пуст, то ошибка
err1:			err = CE_Error(ip, "Unexpected '@'; it may only appear within [ ]", NULL,NULL);
			*err_f = LVTRUE;
			goto out;
		}

		if (op->prior <= otp->prior) {	// НЕ '(' или '['
			OpsStack_DecLen(ip);	// уменьшить размер стека
			macro_PutToExpression(otp);	// перенести из стека в выражение
			continue;
		}
		// ==========================
		//	НАШЛИ '(' или '['
		// ==========================

		if (otp->category != CAT_OINDEX) {
err2:		// ОШИБКА. Нашли не '['
			// --------------------
			// пробуем все-таки найти [ для определения типа ошибки
			for (i=OpsStack_GetLen(ip); i>0; i--) {
				if (!(otp = OpsStack_GetPnt(ip, i)))
					break;
				if (otp->category == CAT_OINDEX) {	// нашли '['
					err = CE_Error(ip, "Unexpected '@'. Missing ')' before '@'", NULL,NULL);
					*err_f = LVTRUE;
					goto out;
				}

			}
			goto err1;	// так и не нашли [ в стеке
		}

		// =======================
		//	Если нашли '['
		// =======================
		if (!(fip = FIOStack_TopPnt(ip))) {		// стек пуст
			CE_Error(ip, "!!! ", NULL,NULL);	// ТАКОГО БЫТЬ НЕ ДОЛЖНО !!!
			goto err1;
		}
		if (fip->type != FIO_TYPE_IDXOP) {		//не [..]
			CE_Error(ip, "!!! ", NULL,NULL);	// ТАКОГО БЫТЬ НЕ ДОЛЖНО !!!
			goto err2;
		}
		fip->w_arg	= TRUE;			// Признак наличия аргументов
		fip->nArg	+= 1;			// количество аргументов
		fip->misc	= IDX_SUBSCRIPT_1D;	// результат - 1D
		break;
	}
out:
	return	err;
}
