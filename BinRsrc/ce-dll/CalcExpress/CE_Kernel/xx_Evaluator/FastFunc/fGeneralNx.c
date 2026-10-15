/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#define	_ARG_ARR_SIZE_	3

// ============================================================================================
// =================    ОБРАБОТЧИК ПРОИЗВОЛЬНЫХ ФУНКЦИЙ С 0..N АРГУМЕНТАМИ  ===================
// ============================================================================================

MgErr	f_general_Nx( register	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
				int32		arg_qty,	//кол-во аргументов
				LVBoolean	*err_f)
{
	MgErr		err = noErr;
	_Operand	*arg, *res;		// Указатель на обрабытываемую структуру
	int32		NArgsInCall;		// кол-во аргументов в вызове
	_gen_operand	out;
	_gen_operand	*opdArr   = NULL;
	_gen_operand	inb[_ARG_ARR_SIZE_];	// буфер
	int32		i;
	Bool32		itComplex = FALSE;
	// -------------

	fill_operand( (_dbl_operand*)&out);

	NArgsInCall	= arg_qty;		

	// *** если вызвана без параметров, то создать параметр (для результата)
	if (!arg_qty) {
		if (err = ff_PseudoArg(ip, 0))
			return err;
		arg_qty = 1;
	}
	res = &((*(ip->OpdStk))->data[ip->OpdNum-arg_qty+1]);	// указатель на результат

	// -----------------------------------------
	// Подготовить array of _gen_operand
	// -----------------------------------------
	if (NArgsInCall <= 0)	goto proc;

	if (NArgsInCall <= _ARG_ARR_SIZE_) {
		opdArr = inb;
	}
	else {
		opdArr = ce_DSNewPtr(NArgsInCall * sizeof(_gen_operand));
		if (!opdArr) {
			err = mFullErr;	goto out;
		}
	}
	// *** инициализировать
	for(i=0; i < NArgsInCall; i++)
		fill_operand( (_dbl_operand*)opdArr + i);
	

	// ------------------
	// Цикл по аргументам
	// ------------------
	for (i=0; i < NArgsInCall; i++) {

		arg = &((*(ip->OpdStk))->data[ip->OpdNum - arg_qty+i+1]);	// указатель на аргумент
		if (arg->type == OPTYPE_IDENT) {
			err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
			if (err || *err_f)	goto out;
		}
		if (_operand_get_BaseType(arg->type) == OPTYPE_STR) {
			err = _prepare_strings((_OperandBase*)arg, (_str_operand*)opdArr + i) ;
		}
		else {
			if (_prepare_operand( (_OperandBase*)arg, (_dbl_operand*)opdArr + i, &itComplex)) {
				err = CE_Error(ip, "Invalid type of parameter %d.", (LStrHandle)(size_t)(i+1), NULL);
				*err_f = LVTRUE;
			}
		}
		if (err || *err_f) goto out;
	}

	// ===	Вызов обработчика  ===
proc:
	err = fInfo->exec_dbl( opdArr, NArgsInCall, &out);

	// ===  Сохранить значение результата
	// Если выделение не выполнялось, то результат уже на месте.
	res->type = out.ResType;
	res->num  = out.scalar;
	if (out.alloc) {
		if (_operand_get_BaseType(res->type) == OPTYPE_STR)
			err = _save_strings( (_str_operand*)&out, &(res->str));
		else	err = _save_array  ( (_dbl_operand*)&out, &(res->str));
	}

	// ===  если ошибка этапа выполнения
	if (out.erMsg) {
		CE_Error( ip, "%s", (LStrHandle)out.erMsg, NULL );
		*err_f = LVTRUE;
	}
out:
	// *** Изменить размер стека ***
	ip->OpdNum -= arg_qty - 1;

	// *** освободить память под массивы
	if (opdArr) {
		for (i=0; i < NArgsInCall; i++) {
			if (opdArr[i].alloc)
				DSDisposePtr(opdArr[i].array);
		}
		if (opdArr != inb)
			DSDisposePtr(opdArr);
		opdArr = NULL;
	}
	if (out.alloc)	DSDisposePtr(out.array);

	return err;
}

#undef	_ARG_ARR_SIZE_
