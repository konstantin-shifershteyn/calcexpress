/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ============================================================================================
// ======================  ОБРАБОТЧИК МАТ. ФУНКЦИЙ С 1-м АРГУМЕНТОМ  ==========================
// ============================================================================================

MgErr	f_math_arg1(_CE_Instance	*ip,
					_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
					int32			arg_qty,	//кол-во аргументов
					LVBoolean		*err_f)
{
	_Operand	*arg;
	MgErr		err;

	arg = OpdStack_EPnt(ip, ip->OpdNum);		// >>> #CE_3.0.7 2026-07-25 : указатель на аргумент

	// Определение значения переменной
	if (arg->type == OPTYPE_IDENT) {
		err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
		if (err || *err_f)	return err;
	}
	return  op_opd1( ip, fInfo->exec_dbl, fInfo->exec_cdb, (_OperandBase*)arg, err_f);
}

// ============================================================================================
// =====================  ОБРАБОТЧИК МАТ. ФУНКЦИЙ ТИПА pi() или e()  ==========================
// ============================================================================================

MgErr	f_math_arg01(_CE_Instance	*ip,
					_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
					int32			arg_qty,	//кол-во аргументов
					LVBoolean		*err_f)
{
	MgErr	err;

	// если функция без аргументов
	if (!arg_qty) {
		if (err = ff_PseudoArg(ip, 1))
			return err;
	}
	return f_math_arg1( ip, fInfo, 1, err_f);
}

// ==============================================================================================
// =============================  Заглушка для комплексных чисел  ===============================
// ==============================================================================================
MgErr ___noCpx_a1 ( _dbl_operand *arg)
{
	arg->erMsg = "Invalid parameter type (complex numbers are not allowed)";
	return noErr;
}


