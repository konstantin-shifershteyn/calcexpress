/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// тело в конце файла
MgErr Proc_CIndex( _CE_Instance *ip, _Operation *op, LVBoolean *err_f);

// =====================================
// ¬ыполн€етс€ если входна€ операци€ ']'
// =====================================

MgErr PO_CIndex(
			_CE_Instance	*ip,
			int32			*expIdx,
			_Operation		*op,
			Bool32			*prev_operand,	// *may_be_func
			LVBoolean		*err_f)
{
	_Operation	*otp;		// temporary
	MgErr		err = noErr;

	if ( !(*prev_operand) ) {
		goto unexpected;
	}
	*prev_operand	= TRUE;

	// ==========================
	// ѕоиск '[' в стеке операций
	// ==========================
	for(;;) {
		otp = OpsStack_TopPnt(ip);			// операци€ в вершине стека
		if (!otp) {							// если стек пуст, то ќшибка
unexpected:
			err = CE_Error(ip, "Unexpected ']'", NULL,NULL);
			*err_f = LVTRUE;
			goto out;
		}
		OpsStack_DecLen(ip);				// уменьшить размер стека

		if (op->prior > otp->prior)			// нашли '(' или '['
			break;
		macro_PutToExpression( otp );		// перенести из стека в выражение
	}

	// Ќашли операцию с нулевым приоритетом: '(' или '['
	// ------------------------------------------------
	err = Proc_CIndex( ip, otp, err_f);	// !!! *otp будет изменен !!!
	if (err || *err_f)
		goto out;
	macro_PutToExpression( otp );			// !!! *otp изменен в Proc_CBracket !!!
out:
	return	err;
}

// #######################################################

MgErr Proc_CIndex(	_CE_Instance	*ip,
					_Operation		*op,
					LVBoolean		*err_f)
{
	register _FuncIdxOp	*fip;

	if (op->category != CAT_OINDEX)
		goto nopair;

	if (!(fip = FIOStack_TopPnt(ip))) {
		*err_f	= LVTRUE;
		return	CE_Error(ip, "Unexpected ']'", NULL, NULL);
	}

	if (fip->type != FIO_TYPE_IDXOP) {
nopair:
		*err_f	= LVTRUE;
		return	CE_Error(ip, "Missing ')' before ']'", NULL, NULL);
	}

	// ”далить оператор индексации из стека
	// ------------------------------------
	FIOStack_DecLen(ip);

	// ===============================
	// —формировать параметры операции
	// ===============================
	op->category	= CAT_INDEX;
	op->prior		= 0;
	op->direction	= DIR_LR;
	op->code		= 0;
	op->n_ops		= (uInt16)fip->nArg;
//	op->n_ops		= fip->nArg;

	op->misc		= fip->misc;		// TYPE OF SUBSCRIPT

	if (!(fip->w_arg)) {		// если нет аргументов, то ошибка
		*err_f	= LVTRUE;
		return	CE_Error(ip, "Missing operand within a subscript operator [ ]", NULL, NULL);
	}

	op->n_ops += 1;				// последний индекс

	if (op->n_ops > 3) {		// array[idx1,idx2] - всего 3 операнда
		*err_f	= LVTRUE;
		return	CE_Error(ip, "Too many indexes within subscript operator [ ]", NULL, NULL);
	}

//	if (fip->w_arg)
//		op->n_ops++;
//	else {
//		*err_f	= LVTRUE;
//		return	CE_Error(ip, "Missing argument(s) in the array indexing operator '[]'", NULL, NULL);
//	}
	return noErr;
}
