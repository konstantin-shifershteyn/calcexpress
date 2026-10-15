/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#define	_ARG_ARR_SIZE_	4

// ============================================================================================
// ===================  ОБРАБОТЧИК МАТ. ФУНКЦИЙ С 0..N АРГУМЕНТАМИ (Тип 3) ====================
// ============================================================================================
//
// От типа 2 отличается тем, что устанавливает lvErrorCode (errno())
// Соответственно вызов обработчиков отличается !!!
//
MgErr	f_math_argNxErr(
				_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
				int32			arg_qty,	//кол-во аргументов
				LVBoolean		*err_f)
{
	MgErr			err = noErr;
	_Operand		*arg, *res;		// Указатель на обрабытываемую структуру
	int32			NArgsInCall;		// кол-во аргументов в вызове
	_dbl_operand	out;
	_dbl_operand	*opdArr = NULL;
	_dbl_operand	inb[_ARG_ARR_SIZE_];	// буфер
	int32			i, lvErr = 0;
	Bool32			itComplex = FALSE;
	// -------------

	fill_operand(&out);

	NArgsInCall	= arg_qty;		

	// *** если вызвана без параметров, то создать параметр (для результата)
	if (!arg_qty) {
		if (err = ff_PseudoArg(ip, 0))
			return err;
		arg_qty = 1;
	}
	res = &((*(ip->OpdStk))->data[ip->OpdNum-arg_qty+1]);	// указатель на результат

	// -----------------------------------------
	// Подготовить array of _dbl_operand
	// -----------------------------------------
	if (NArgsInCall <= 0)	goto proc;

	if (NArgsInCall <= _ARG_ARR_SIZE_) {
		opdArr = inb;
	}
	else {
		opdArr = ce_DSNewPtr(NArgsInCall * sizeof(_dbl_operand));
		if (!opdArr) {
			err = mFullErr;	goto out;
		}
	}
	// *** инициализировать
	for(i=0; i < NArgsInCall; i++)
		fill_operand(opdArr + i);

	// ------------------
	// Цикл по аргументам
	// ------------------
	for (i=0; i < NArgsInCall; i++) {

		arg = &((*(ip->OpdStk))->data[ip->OpdNum - arg_qty+i+1]);	// указатель на аргумент
		if (arg->type == OPTYPE_IDENT) {
			err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
			if (err || *err_f)	goto out;
		}
		if (_prepare_operand( (_OperandBase*)arg, opdArr+i, &itComplex))	{
			err = CE_Error(ip, "Invalid type of parameter %d.", (LStrHandle)(size_t)(i+1), NULL);
			*err_f = LVTRUE;
			goto out;
		}

	}
	// ===	Вызов обработчика  ===
proc:
	if (itComplex)	err = fInfo->exec_cdb( opdArr, NArgsInCall, &out, &lvErr);
	else			err = fInfo->exec_dbl( opdArr, NArgsInCall, &out, &lvErr);

	// ===  Установить значение lvErrorCode
	rtErrCode_Set(ip->VCHandle, lvErr);

	// ===  Сохранить значение результата
	// Если выделение не выполнялось, то результат уже на месте.
	res->type = out.ResType;
	res->num  = out.scalar;
	if (out.ResType != OPTYPE_DBL)
		if (out.alloc)
			_save_array( &out, &(res->str));

	// ===  если ошибка этапа выполнения
	if (out.erMsg) {
		err = CE_Error( ip, "%s", (LStrHandle)out.erMsg, NULL );
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

// ============================================================================================
// ============================  Заглушка для комплексных чисел  ==============================
// ============================================================================================
MgErr ___noCpx_aNxErr ( _dbl_operand *opdArr, int32 qty, _dbl_operand *out, int32 *lvErr)
{
	out->erMsg = "Invalid parameter type (complex numbers are not allowed)";
//	out->erMsg = "arguments can't be a Complex Number or Array";
	return noErr;
}

#undef	_ARG_ARR_SIZE_
