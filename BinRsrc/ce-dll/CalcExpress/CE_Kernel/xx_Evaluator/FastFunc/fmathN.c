/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// fills the operand structure with default values
// is used for handling functions with variable number of arguments
// =================================================================
__inline void	fill_operand(register _dbl_operand *arg) {
	arg->ResType	= OPTYPE_DBL;
	arg->scalar	= 0.0;
	arg->nDims	= 0;
	arg->size1	= arg->size2	= 1;
	arg->array	= NULL;
	arg->alloc	= FALSE;
	arg->erMsg	= NULL;
}

// ============================================================================================
// ===========  ОБРАБОТЧИК ФУНКЦИЙ С ПРОИЗВОЛЬНЫМ КОЛИЧЕСТВОМ АРГУМЕНТОВ (1...N) ==============
// ============================================================================================

MgErr	f_math_argN(	_CE_Instance	*ip,
						_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
						int32			arg_qty,	//кол-во аргументов
						LVBoolean		*err_f)
{
		MgErr			err = noErr;
		_Operand		*opd;		// Указатель на исходный операнд
		_Operand		*res;		// Указатель на имя ф-ии, он-же и результат
		_Operand		*arg;		// Указатель на обрабытываемую структуру
		int32			idx;
		_dbl_operand	in, out, tmp;
		Bool32			itComplex = FALSE;

	fill_operand(&in);
	fill_operand(&out);
	fill_operand(&tmp);

	idx = ip->OpdNum - arg_qty + 1;		// индекс первого операнда, он-же - результат
	res = &((*(ip->OpdStk))->data[idx]);	// указатель на результат
	opd = res;

	// ================================================
	//    цикл по аргументам. Выполняется arg_qty+1 раз
	// ================================================
	for (idx=0; idx <= arg_qty; idx++, opd++) {
		if (idx < arg_qty) {

			// Определить обрабатываемые данные
			// ================================
			arg = opd;
			if (opd->type == OPTYPE_IDENT) {
				arg = &(ip->LOpd);
				err = GetVariable( ip, opd, (_OperandBase*)arg, err_f);
				if (err || *err_f)	goto out;
			}
			// =============================
			// Определить параметры операнда
			// =============================
			if (arg->type == OPTYPE_DBL) {
				in.ResType	= OPTYPE_DBL;
				in.nDims	= 0;
				in.size1	= in.size2 = 1;
				in.scalar	= arg->num;
			}
			else {
				if (_prepare_operand( (_OperandBase*)arg, &in, &itComplex)) {
					out.erMsg = "Invalid type of parameter(s)";
					goto error;
				}
			}
		} /*if*/
		// ===  Вызов обработчика  ===
		if (itComplex)	err = fInfo->exec_cdb( &in, &out, &tmp, idx, arg_qty );
		else		err = fInfo->exec_dbl( &in, &out, &tmp, idx, arg_qty );
		if (err)	goto out;
		if (in.alloc)	{ DSDisposePtr(in.array); in.array = NULL; in.alloc = FALSE; }

		// ===  если ошибка этапа выполнения  ===
		if (out.erMsg) {
error:			err = CE_Error( ip, "%s", (LStrHandle)out.erMsg, NULL );
			*err_f = LVTRUE;
			goto end_proc;
		}

	} /* for */

	// Сохранить результат
	// Если выделение не выполнялось, то результат уже на месте.
	// ===================
	res->type = out.ResType;
	res->num  = out.scalar;
	if ( out.ResType != OPTYPE_DBL)
		if (out.alloc)
			_save_array( &out, &(res->str));

end_proc:

	// ========================================
	// === СТЕК ОПЕРАНДОВ. КОЛ-ВО ЭЛЕМЕНТОВ ===
	ip->OpdNum -= arg_qty - 1;

out:
	//освободить память под массив
	if (in.alloc)	DSDisposePtr(in.array);
	if (out.alloc)	DSDisposePtr(out.array);
	if (tmp.alloc)	DSDisposePtr(tmp.array);

	return err;
}
// ==============================================================================================
// =============================  Заглушка для комплексных чисел  ===============================
// ==============================================================================================
MgErr ___noCpx_aN( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	out->erMsg = "Invalid parameter type (complex numbers are not allowed)";
//	out->erMsg = "argument(s) can't be a Complex Number or Array";
	return noErr;
}

