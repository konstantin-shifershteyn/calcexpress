/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// Исполнитель FastFunc функции которая возвращает тип IDENTIFIER
extern	MgErr v_vnidn(_CE_Instance *ip, _FastFuncInfo *fInfo, int32 arg_qty, LVBoolean *err_f);

// тело в конце файла
extern	MgErr Proc_CBracket( _CE_Instance *ip, _Operation *op, Bool32 *add, Bool32 *maybe_func, LVBoolean *err_f);

// =====================================
// Выполняется если входная операция ')'
// =====================================

MgErr PO_CBracket(	_CE_Instance	*ip,
			int32		*expIdx,
			_Operation	*op,
			Bool32		*prev_operand,
			Bool32		*prev_obracket,
			Bool32		*maybe_func,
			LVBoolean	*err_f)
{
	Bool32		completed_func;
	_Operation	*otp;		// temporary
	MgErr		err = noErr;

	// Если предыдушая лексема не операнд и не '(', то ОШИБКА на 100%
	// Если предыдушая лексема '(' , то контроль ошибки выполняется дальше
	// Флаги *prev_obracket и *prev_operand устанавливаются дальше
	if ( !(*prev_operand || *prev_obracket) ) {
			goto unexpected;
	}
//	*prev_obracket	= FALSE;
//	*prev_operand	= TRUE;

	*maybe_func = FALSE;	// может измениться в Proc_CBracket() если
				// завершается FastFunc возвращающая тип IDENTIFIER

	// ==========================
	// Поиск '(' в стеке операций
	// ==========================
	for(;;) {
		if (!(otp = OpsStack_TopPnt(ip)))	// операция в вершине стека
			goto unexpected;				// если стек пуст, то Ошибка

		OpsStack_DecLen(ip);				// уменьшить размер стека

		if (op->prior > otp->prior)			// нашли '(' или '['
			break;
		macro_PutToExpression( otp );		// перенести из стека в выражение
	}

	// Нашли операцию с нулевым приоритетом: '(' или '['
	// ------------------------------------------------
	// !!! *otp будет изменен и *maybe_func может измениться !!!
	// ---
	err = Proc_CBracket( ip, otp, &completed_func, maybe_func, err_f);
	if (err || *err_f)
		goto out;			// если нашли не '(', то ОШИБКА

	// Если скобка, которая завершает вызов функции
	// --------------------------------------------
	if (completed_func) {
		// 1. Поместить вызов функции в выражение
		macro_PutToExpression( otp );		// !!! *otp изменен в Proc_CBracket !!!
		// 2. Установить признак операнда
		*prev_operand	= TRUE;
		goto out;
	}

	// Если скобка не формирует вызов функции, то
	// 1. не изменяем признак операнда, он наследуется от предыдущей лексемы
	// 2. выполняем окончательный контроль ошибки
	// ---------------
	if (*prev_obracket) {
		err = CE_Error(ip, "Empty expression within parentheses ()", NULL,NULL);
		*err_f = LVTRUE;
		goto out;
	}
	if (!(*prev_operand)) {
unexpected:	err = CE_Error(ip, "Unexpected ')'", NULL,NULL);
		*err_f = LVTRUE;
	}
out:
	*prev_obracket	= FALSE;
	return	err;
}

// #######################################################

MgErr Proc_CBracket(
			_CE_Instance	*ip,
			_Operation		*op,
			Bool32			*add_to_list,
			Bool32			*maybe_func,
			LVBoolean		*err_f)
{
	_FuncIdxOp	*fip;

	*add_to_list = FALSE;

	if (op->category != CAT_OBRACKET)
		goto nopair;

	if (!(fip = FIOStack_TopPnt(ip)))
		goto out;		// выход если нет действующей функции
	if (--(fip->brLevel))
		goto out;		// выход если эта скобка не завершающая

	if (fip->type != FIO_TYPE_FUNC) {
nopair:
		*err_f	= LVTRUE;
		return	CE_Error(ip, "Missing ']' before ')'", NULL, NULL);
	}

	// Удалить функцию из стека
	// ------------------------
	FIOStack_DecLen(ip);

	// ===============================
	// Сформировать параметры операции
	// ===============================
	op->prior	= 0;
	op->direction	= DIR_LR;

	op->code	= fip->FastFuncIdx;	// -1 for non-FastFunc
	op->n_ops	= (uInt16)fip->nArg;
	if (fip->w_arg)	 op->n_ops++;
//------
	op->misc	= fip->misc;	// Index of Expression Element with a FuncVI name !19:18 22-03-02!
//------
	if (fip->FastFuncIdx < 0) {
		op->category	= CAT_FUNC;
	}
	else {
		_FastFuncInfo	*frec;

		frec = &(FuncList[fip->FastFuncIdx]);

		op->category	= CAT_FASTFUNC;

		// Контроль количества аргументов
		// ==============================
		if (op->n_ops < (uInt16)frec->minArg) {
			*err_f = LVTRUE;
			return CE_Error(ip, "Too few parameters in the call of function %.8s()",(LStrHandle)frec->name,NULL);
		}
		if (op->n_ops > (uInt16)frec->maxArg) {
			*err_f = LVTRUE;
			return CE_Error(ip, "Too many parameters in the call of function %.8s()",(LStrHandle)frec->name,NULL);
		}

		// ***	May form indirect function call only
		// ***	if the current function returns an identifier
		*maybe_func = (frec->processor == v_vnidn);

	}

	*add_to_list = TRUE;
out:
	return noErr;
}
