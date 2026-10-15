/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ============================================================================================
// =====================  ОБРАБОТЧИК МАТ. ФУНКЦИЙ С 2-мя АРГУМЕНТАМИ  =========================
// ============================================================================================

MgErr	f_math_arg2(_CE_Instance	*ip,
					_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
					int32			arg_qty,	//кол-во аргументов
					LVBoolean		*err_f)
{
	MgErr		err = noErr;
	_Operand	*arg1, *arg2;	// Указатель на обрабытываемую структуру


	arg1 = OpdStack_EPnt(ip, ip->OpdNum-1);		// >>> #CE_3.0.7 2026-07-25 : указатель на 1-st аргумент
	arg2 = OpdStack_EPnt(ip, ip->OpdNum);		// >>> #CE_3.0.7 2026-07-25 : указатель на 2-nd аргумент

	// Определение значения 1-го операнда
	if (arg1->type == OPTYPE_IDENT) {
		err = GetVariable( ip, arg1, (_OperandBase*)arg1, err_f);
		if (err || *err_f)	return err;
	}
	// Определение значения 2-го операнда
	if (arg2->type == OPTYPE_IDENT) {
		err = GetVariable( ip, arg2, (_OperandBase*)arg2, err_f);
		if (err || *err_f)	return err;
	}

	ip->OpdNum -= 1;

	return  op_opd2( ip, fInfo->exec_dbl, fInfo->exec_cdb, (_OperandBase*)arg1, (_OperandBase*)arg2, err_f);
}

// ============================================================================================
// ==================  ОБРАБОТЧИК МАТ. ФУНКЦИЙ С 0, 1 или 2-мя АРГУМЕНТАМИ  ===================
// ============================================================================================

MgErr	f_math_arg02(_CE_Instance	*ip,
					_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
					int32			arg_qty,	//кол-во аргументов
					LVBoolean		*err_f)
{
	MgErr	err;

	// ==========================
	// если не хватает аргументов
	// в arg1 заносим 1
	// в arg2 заносим 0
	// ==========================
	while(arg_qty < 2) {
		if (err = ff_PseudoArg(ip, 1 - arg_qty))
			return err;
		arg_qty++;
	}
	return f_math_arg2( ip, fInfo, 2, err_f);
}

// ============================================================================================
// ============================  Заглушка для комплексных чисел  ==============================
// ============================================================================================
MgErr ___noCpx_a2 ( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
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
MgErr res2_init( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	// Размерность и тип - по большему операнду
	out->ResType	= __max( in1->ResType, in2->ResType );
	out->nDims	= __max( in1->nDims,   in2->nDims );

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
L1:
	if (out->ResType == in1->ResType)
		if (out->size1 == in1->size1)
			if (out->size2 == in1->size2) {
				out->array = in1->array;
				return noErr;
			}

	// Подготовить массив результата
	out->array = ce_DSNewPtr( sizeof(float64) * (out->size1 * out->size2));
	if (!out->array)
		return mFullErr;
	out->alloc = TRUE;

	return noErr;
}

