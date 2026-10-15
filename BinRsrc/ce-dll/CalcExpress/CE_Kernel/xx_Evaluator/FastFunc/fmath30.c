/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ============================================================================================
// ===================  ОБРАБОТЧИК МАТ. ФУНКЦИЙ С 3-мя (0..3) АРГУМЕНТАМИ  ====================
// ============================================================================================

MgErr	f_math_arg03( 
				_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
				int32			arg_qty,	//кол-во аргументов
				LVBoolean		*err_f)
{
	MgErr			err = noErr;
	_Operand		*arg, *res;			// Указатель на обрабытываемую структуру
	_dbl_operand	in1, in2, in3, out;
	Bool32			itComplex;

	itComplex = FALSE;
	in1.alloc = in2.alloc = in3.alloc = out.alloc = FALSE;	// массивы не выделены
	out.erMsg = NULL;					// сообщения об ошибках нет
	out.ResType = OPTYPE_DBL;


	if (!arg_qty) {
		if (err = ff_PseudoArg(ip, 0))
			return err;
		arg_qty = 1;
		res = &((*(ip->OpdStk))->data[ip->OpdNum]);	// указатель на результат

		// признак пропущенного аргумента
		in1.ResType = in2.ResType = in3.ResType = OPTYPE_SKIPPED_ARG;
		goto do_proc;
	}

	// *** Определение значения 1-го операнда ***
	// ==========================================
	arg = &((*(ip->OpdStk))->data[ip->OpdNum-arg_qty+1]);	// указатель на аргумент
	res = arg;						// указатель на результат
	if (arg->type == OPTYPE_IDENT) {
		err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
		if (err || *err_f)	
			goto out;
	}
	if (_prepare_operand( (_OperandBase*)arg, &in1, &itComplex))	{
		out.erMsg = "Invalid type of the 1st parameter";
		goto end_proc;
	}

	// *** Определение значения 2-го операнда ***
	// ==========================================
	if (arg_qty < 2) {
		// признак пропущенного аргумента
		in2.ResType = in3.ResType = OPTYPE_SKIPPED_ARG;
		goto do_proc;
	}
	arg = &((*(ip->OpdStk))->data[ip->OpdNum-arg_qty+2]);	// указатель на аргумент
	if (arg->type == OPTYPE_IDENT) {
		err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
		if (err || *err_f)
			goto out;
	}
	if (_prepare_operand( (_OperandBase*)arg, &in2, &itComplex))	{
		out.erMsg = "Invalid type of the 2nd parameter";
		goto end_proc;
	}

	// *** Определение значения 3-го операнда ***
	// ==========================================
	if (arg_qty < 3) {
		// признак пропущенного аргумента
		in3.ResType = OPTYPE_SKIPPED_ARG;
		goto do_proc;
	}

	arg = &((*(ip->OpdStk))->data[ip->OpdNum]);	// указатель на аргумент
	if (arg->type == OPTYPE_IDENT) {
		err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
		if (err || *err_f)	goto out;
	}
	if (_prepare_operand( (_OperandBase*)arg, &in3, &itComplex))	{
		out.erMsg = "Invalid type of the 3rd parameter";
		goto end_proc;
	}


	// ===	Вызов обработчика  ===
do_proc:
	if (itComplex)	err = fInfo->exec_cdb( &in1, &in2, &in3, &out);
	else		err = fInfo->exec_dbl( &in1, &in2, &in3, &out);

	// ===  Сохранить значение результата
	// Если выделение не выполнялось, то результат уже на месте.
	res->num = out.scalar;
	if (out.ResType != OPTYPE_DBL)
		if (out.alloc)
			_save_array( &out, &(res->str));

end_proc:
	// ===  СОХРАНИТЬ ТИП РЕЗУЛЬТАТА
	res->type = out.ResType;

	// ===  если ошибка этапа выполнения
	if (out.erMsg) {
		err = CE_Error( ip, "%s", (LStrHandle)out.erMsg, NULL );
		*err_f = LVTRUE;
	}
out:
	// *** Изменить размер стека ***
	ip->OpdNum -= arg_qty - 1;

	// *** освободить память под массивы
	if (in1.alloc)	DSDisposePtr(in1.array);
	if (in2.alloc)	DSDisposePtr(in2.array);
	if (in3.alloc)	DSDisposePtr(in3.array);
	if (out.alloc)	DSDisposePtr(out.array);

	return err;
}

// ============================================================================================
// ============================  Заглушка для комплексных чисел  ==============================
// ============================================================================================
MgErr ___noCpx_a3 ( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *in3, _dbl_operand *out)
{
	out->erMsg = "Invalid parameter type (complex numbers are not allowed)";
//	out->erMsg = "arguments can't be a Complex Number or Array";
	return noErr;
}

// ============================================================================================
// ===============================  Заготовки для Функций   ===================================
// ============================================================================================

// Инициализации структуры Результата
// ----------------------------------
MgErr res3_init(	_dbl_operand	*in1,
			_dbl_operand	*in2,
			_dbl_operand	*in3,
			_dbl_operand	*out
		)
{
	// Размерность и тип - по большему операнду
	out->ResType	= __max( __max( in1->ResType, in2->ResType ),	in3->ResType);
	out->nDims	= __max( __max( in1->nDims,   in2->nDims ),	in3->nDims );

	// ========================
	// Both operands are scalar
	// ========================
	if (!out->nDims) {
		out->size1 = out->size2 = 1;
		return noErr;
	}
	// ==========================
	// No one operand is 2D Array
	// ==========================
	if (out->nDims == 1) {
		out->size2 = 1;
		out->size1 = 0x7FFFFFFF;
		if (in1->nDims)	out->size1 = __min( out->size1, in1->size1 );
		if (in2->nDims)	out->size1 = __min( out->size1, in2->size1 );
		if (in3->nDims)	out->size1 = __min( out->size1, in3->size1 );
		goto L1;
	}
	// ================================
	// At least one operand is 2D Array
	// ================================
	// !!! 1D Array is row !!!
	// ================================
	// ***  Number of rows  ***
	out->size1 = 0x7FFFFFFF;
	if (in1->nDims == 2)	out->size1 = __min( out->size1, in1->size1 );
	if (in2->nDims == 2)	out->size1 = __min( out->size1, in2->size1 );
	if (in3->nDims == 2)	out->size1 = __min( out->size1, in3->size1 );

	// ***  Number of columns ***
	out->size2 = 0x7FFFFFFF;
	switch(in1->nDims) {
		case 2:	out->size2 = __min( out->size2, in1->size2 ); break;
		case 1: out->size2 = __min( out->size2, in1->size1 ); break;
	}
	switch(in2->nDims) {
		case 2:	out->size2 = __min( out->size2, in2->size2 ); break;
		case 1: out->size2 = __min( out->size2, in2->size1 ); break;
	}
	switch(in3->nDims) {
		case 2:	out->size2 = __min( out->size2, in3->size2 ); break;
		case 1: out->size2 = __min( out->size2, in3->size1 ); break;
	}
L1:
	// не выделять память если тип и размер
	// результата совпадают с первым аргументом
	// ----------------------------------------
	if (out->ResType == in1->ResType) {
		if (out->size1 == in1->size1)
			if (out->size2 == in1->size2) {
				out->array = in1->array;
				return noErr;
			}
	}
	// Подготовить массив результата
	// -----------------------------
	out->array = ce_DSNewPtr( sizeof(float64) * (out->size1 * out->size2));
	if (!out->array)
		return mFullErr;
	out->alloc = TRUE;

	return noErr;
}

// ========================================================================
// Обобщенная функция для математических операций и функций трех аргументов
// ========================================================================
// ---------------------------------
typedef float64	(*_f_arg3_dbl_low) (float64 op1, float64 op2, float64 op3);
// ---------------------------------

MgErr math_arg3_template(	_dbl_operand	*in1,
				_dbl_operand	*in2,
				_dbl_operand	*in3,
				_dbl_operand	*out,
		register	_f_arg3_dbl_low	_func_		// pointer to target function
		)
{
	int32	i, row, col;
	float64	*pnt, *p1, *p2, *p3;
	MgErr	err;

	// --- подготовить структуру результата
	if (err = res3_init(in1,in2,in3,out))	goto out;

	// ---  Все операнды - Скаляры
	if (!out->nDims) {
		out->scalar = _func_( in1->scalar, in2->scalar, in3->scalar);
		goto out;
	}

	pnt	= out->array;
	p1	= (in1->nDims) ? in1->array : &(in1->scalar);
	p2	= (in2->nDims) ? in2->array : &(in2->scalar);
	p3	= (in3->nDims) ? in3->array : &(in3->scalar);

	// --- Только скаляры и 1D Arrays (Нет 2D Arrays)
	// --- или два скаляра и один 2D Array
	// ==============================================
	if (	(out->nDims == 1)	||			// скаляры и 1D Array(s)
		((in1->nDims + in2->nDims + in3->nDims) == 2))	// скаляры и один 2D Array
	{
		for (i = out->size1 * out->size2; i; i--) {
			*pnt++ = _func_(*p1, *p2, *p3);
			if (in1->nDims)	p1++;
			if (in2->nDims)	p2++;
			if (in3->nDims)	p3++;
		}
		goto out;
	}

	// --- Массивы разных размерностей или более одного 2D Arrays
	// ==========================================================
	for (row=0; row < out->size1; row++) {
		switch(in1->nDims) {
			case 1:	p1 = in1->array;			break;
			case 2: p1 = in1->array + row * in1->size2;	break;
		}
		switch(in2->nDims) {
			case 1:	p2 = in2->array;			break;
			case 2: p2 = in2->array + row * in2->size2;	break;
		}
		switch(in3->nDims) {
			case 1:	p3 = in3->array;			break;
			case 2: p3 = in3->array + row * in3->size2;	break;
		}
		for (col=0; col < out->size2; col++) {
			*pnt++ = _func_(*p1, *p2, *p3);
			if (in1->nDims)	p1++;
			if (in2->nDims)	p2++;
			if (in3->nDims)	p3++;
		}
	}
out:
	return err;
}
