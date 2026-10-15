/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ========================================================================================
// ===================  Выполняет операции с не Numeric аргументами  ======================
// ========================================================================================

MgErr CpxOperation(	_CE_Instance	*ip,
					_Operation		*op,
					_OperandBase	*Lopd,
					_OperandBase	*Ropd,
					LVBoolean		*err_f)
{
	MgErr	err;

//	if (*err_f)	return noErr;

	// Если Запрещены такие операции
	if (!(ip->procPars.en_cpx)) {
		err = CE_Error(ip, "Operand(s) must be a Numeric scalar", NULL,NULL);
		*err_f = LVTRUE;
		return err;
	}

	if (op->n_ops == 1)
		err = op_opd1( ip, op1_names[op->code].exec_dbl, op1_names[op->code].exec_cdb, Lopd, err_f);
	else
		err = op_opd2( ip, op2_names[op->code].exec_dbl, op2_names[op->code].exec_cdb, Lopd, Ropd, err_f);

	return err;
}
// ========================================================================================
// ======================  ОБРАБОТЧИК ОПЕРАЦИЙ С 1-м АРГУМЕНТОМ  ==========================
// ========================================================================================

MgErr	op_opd1(_CE_Instance	*ip,
				_op_dbl			exec_dbl,
				_op_cdb			exec_cdb,
				_OperandBase	*arg,		// Указатель на обрабытываемую структуру
				LVBoolean		*err_f)
{
	_dbl_operand	opinfo;
	Bool32			itComplex;
	MgErr			err = noErr;

	itComplex		= FALSE;	// признак комплексного операнда
	opinfo.alloc	= FALSE;	// массив не выделен
	opinfo.erMsg	= NULL;		// нет ошибок

	// ================================
	// Определить обрабатываемые данные
	// ================================
	if (arg->type == OPTYPE_DBL) {
		opinfo.ResType	= OPTYPE_DBL;
		opinfo.nDims	= 0;
		opinfo.size1	= opinfo.size2 = 1;
		opinfo.scalar	= arg->num;
	}
	else 	if (_prepare_operand( arg, &opinfo, &itComplex)) {
			opinfo.erMsg = "Invalid operand datatype";
			goto end_proc;
		}

	// ===========================
	// ===  Вызов обработчика  ===
	// ===========================
	if (itComplex)	err = exec_cdb( &opinfo );
	else			err = exec_dbl( &opinfo );

end_proc:
	arg->type = opinfo.ResType;	// ===  СОХРАНИТЬ ТИП РЕЗУЛЬТАТА
	arg->num  = opinfo.scalar;	// сохранить DBL scalar

	// =============================
	// Если выполняли выделение памяти под результат, то скопировать
	// результат и освободить память.
	// -----------------------------
	// Это возможно в случае если тип, размер или размерность
	// результата и аргумента отличаются.
	// Как пример - функции комплексных чисел
	// -----------------------------
	// Если выделение не выполнялось, то результат уже на месте или DBL scalar.
	// =============================
	if (opinfo.alloc) {
		_save_array( &opinfo, &(arg->str));	// сохранить
		DSDisposePtr(opinfo.array);		// освободить
	}

	// =====================================
	// ===  если ошибка этапа выполнения ===
	// =====================================
	if (opinfo.erMsg) {
		err = CE_Error( ip, "%s", (LStrHandle)opinfo.erMsg, NULL );
		*err_f = LVTRUE;
	}

	return err;
}

// ============================================================================================
// =====================  ОБРАБОТЧИК МАТ. ОПЕРАЦИЙ С 2-мя АРГУМЕНТАМИ  ========================
// ============================================================================================

MgErr	op_opd2(_CE_Instance	*ip,
				_op_dbl			exec_dbl,
				_op_cdb			exec_cdb,
				_OperandBase	*arg1,		//Указатель на левый операнд
				_OperandBase	*arg2,		//Указатель на правый операнд
				LVBoolean		*err_f)
{
	MgErr			err = noErr;
	_OperandBase	*res;		// Указатель на имя ф-ии, он-же и результат
	_dbl_operand	in1, in2, out;
	Bool32			itComplex;

	itComplex = FALSE;
	in1.alloc = in2.alloc = out.alloc = FALSE;	// массивы не выделены
	out.erMsg = NULL;				// сообщения об ошибках нет

	res  = arg1;

	// ==================================
	// Определить параметры 1-го операнда
	// ==================================
	if (arg1->type == OPTYPE_DBL) {
		in1.ResType	= OPTYPE_DBL;
		in1.nDims	= 0;
		in1.size1	= in1.size2 = 1;
		in1.scalar	= arg1->num;
	}
	else	if (_prepare_operand( arg1, &in1, &itComplex))
			goto bad_operand;

	// ==================================
	// Определить параметры 2-го операнда
	// ==================================
	if (arg2->type == OPTYPE_DBL) {
		in2.ResType	= OPTYPE_DBL;
		in2.nDims	= 0;
		in2.size1	= in2.size2 = 1;
		in2.scalar	= arg2->num;
	}
	else	if (_prepare_operand( arg2, &in2, &itComplex)) {
bad_operand:
		out.erMsg = "Invalid operand datatype";
		goto end_proc;
	}

	// =========================================
	// 	Вызов обработчика
	// =========================================
	if (itComplex)	err = exec_cdb( &in1, &in2, &out);
	else			err = exec_dbl( &in1, &in2, &out);


	// Этот фрагмент преднамеренно отличается от аналогичного
	// в функции одного аргумента !!!
	// Если выделение не выполнялось, то результат уже на месте.
	// ==========================
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
//out:
	//освободить память под массивы
	if (in1.alloc)	DSDisposePtr(in1.array);
	if (in2.alloc)	DSDisposePtr(in2.array);
	if (out.alloc)	DSDisposePtr(out.array);

	return err;
}

// =================================================================================
// ВНИМАНИЕ !!! Таблицы упорядочены в порядке кодов операций
// =================================================================================
_OpsNames	op1_names[] =
{
//	Имя операции	isAssign
//	----------------------------------------
	{ "'!'"			, FALSE	, _lnot_dbl	, ___noCpx_a1	}
,	{ "'~'"			, FALSE	, _bnot_dbl	, ___noCpx_a1	}
,	{ "(unary +)"	, FALSE	, _pos_dbl	, _pos_cdb		}
,	{ "(unary -)"	, FALSE	, _neg_dbl	, _neg_cdb		} 
// >>> #CE_3.0 2025-03-18
,	{ "'++var'"		,TRUE	, _inc_dbl	, ___noCpx_a1	} 
,	{ "'var++'"		,TRUE	, _inc_dbl	, ___noCpx_a1	}
,	{ "'--var'"		,TRUE	, _dec_dbl	, ___noCpx_a1	} 
,	{ "'var--'"		,TRUE	, _dec_dbl	, ___noCpx_a1	}
// <<< #CE_3.0 2025-03-18
};

_OpsNames	op2_names[] =
{
//	Имя операции	isAssign
//	----------------------------------------
	{ "'*='"	,TRUE	, _mul_dbl	, _mul_cdb		} ,
	{ "'*'"		, FALSE	, _mul_dbl	, _mul_cdb		} ,
	{ "'/='"	,TRUE	, _div_dbl	, _div_cdb		} ,
	{ "'/'"		, FALSE	, _div_dbl	, _div_cdb		} ,
	{ "'%='"	,TRUE	, _mod_dbl	, ___noCpx_a2	} ,
	{ "'%'"		, FALSE	, _mod_dbl	, ___noCpx_a2	} ,
	{ "'+='"	,TRUE	, _add_dbl	, _add_cdb		} ,
	{ "'+'"		, FALSE	, _add_dbl	, _add_cdb		} ,
	{ "'-='"	,TRUE	, _sub_dbl	, _sub_cdb		} ,
	{ "'-'"		, FALSE	, _sub_dbl	, _sub_cdb		} ,

	{ "'<<='"	,TRUE	, _lshift_dbl	, ___noCpx_a2	} ,
	{ "'<<'"	, FALSE	, _lshift_dbl	, ___noCpx_a2	} ,
	{ "'<='"	, FALSE	, _lesseq_dbl	, ___noCpx_a2	} ,
	{ "'<'"		, FALSE	, _less_dbl	, ___noCpx_a2		} ,
	{ "'>>='"	,TRUE	, _rshift_dbl	, ___noCpx_a2	} ,
	{ "'>>'"	, FALSE	, _rshift_dbl	, ___noCpx_a2	} ,
	{ "'>='"	, FALSE	, _greateq_dbl	, ___noCpx_a2	} ,
	{ "'>'"		, FALSE	, _great_dbl	, ___noCpx_a2	} ,

	{ "'!='"	, FALSE	, NULL		, NULL		} ,
	{ "'=='"	, FALSE	, NULL		, NULL		} ,
	{ "'='"		,TRUE	, NULL		, NULL		} ,

	{ "'&&'"	, FALSE	, _land_dbl	, ___noCpx_a2	} ,
	{ "'&='"	,TRUE	, _and_dbl	, ___noCpx_a2	} ,
	{ "'&'"		, FALSE	, _and_dbl	, ___noCpx_a2	} ,
	{ "'||'"	, FALSE	, _lor_dbl	, ___noCpx_a2	} ,
	{ "'|='"	,TRUE	, _or_dbl	, ___noCpx_a2	} ,
	{ "'|'"		, FALSE	, _or_dbl	, ___noCpx_a2	} ,
	{ "'^='"	,TRUE	, _xor_dbl	, ___noCpx_a2	} ,
	{ "'^'"		, FALSE	, _xor_dbl	, ___noCpx_a2	} ,

	{ "','"		, FALSE	, NULL		, NULL		} ,

	//--- new for CE 2.0 ---
	{ "'**'"	, FALSE	, _pow_dbl	, _pow_cdb	} ,
	{ "'**='"	,TRUE	, _pow_dbl	, _pow_cdb	} ,
	{ "'^^'"	, FALSE	, _lxor_dbl	, ___noCpx_a2	} ,
	{ "':'"		, FALSE	, NULL		, NULL		} ,
};
