/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ========================================================================================
//	vector( x1 [,x2 ... ,xn] )	===================================================
// ========================================================================================
MgErr	f_arr_vector(	_CE_Instance	*ip,
			_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
			int32		arg_qty,	//кол-во аргументов
			LVBoolean	*err_f)
{
	MgErr		err = noErr;
	_Operand	*opd;		// ”казатель на исходные операнды
	_Operand	*res;		// ”казатель на результат
	_gen_operand	tmp, out;

	int32		idx, size;
	int32		baseType = OPTYPE_DBL, elSize = sizeof(float64);
	char		*pnt;
	Bool32		itComplex = FALSE;

//------------------------

	tmp.alloc = out.alloc = FALSE;
	tmp.erMsg = out.erMsg = NULL;

	idx = ip->OpdNum - (arg_qty - 1);		// индекс первого операнда, он-же - результат
	res = &((*(ip->OpdStk))->data[idx]);	// указатель на результат

	// ===============================================
	// 1-й цикл по аргументам. ѕодсчет общего размера.
	// ===============================================
	size = 0;
	for (idx=0, opd=res; idx < arg_qty; idx++, opd++) {

		// ***** определить параметры аргумента
		if (opd->type == OPTYPE_IDENT) {
			err = GetVariable( ip, opd, (_OperandBase*)opd, err_f);
			if (err || *err_f)
				goto out;
		}
		_general_operand_info((_OperandBase*)opd, &tmp);

		if (!idx) {		// ***** по первому операнду опрдел€ем тип массива
			switch(baseType = _operand_get_BaseType(tmp.ResType)) {
				case OPTYPE_DBL :	elSize = sizeof(float64);		break;
				case OPTYPE_CDB :	elSize = sizeof(float64) * 2;	break;
				case OPTYPE_STR :	elSize = sizeof(LStr*);			break;
				default :			elSize = 0;						goto invType;
			}
		}
		else {			// ***** все аргументы должны иметь один базовый тип
			if (baseType != _operand_get_BaseType(tmp.ResType)) {
invType:		err = CE_Error(ip, "Invalid type of parameter %d.", (LStrHandle)(size_t)(idx+1), NULL);
				*err_f = LVTRUE;
				goto out;
			}
		}
		switch (tmp.nDims) {	// ***** подсчет размера массива
			case 0 : size += 1;			break;
			case 1 : size += tmp.size1;	break;
			default: goto invType;
		}
	}
	// -------------------------------
	// ѕодготовка структуры результата
	// -------------------------------
	out.nDims	= 1;
	out.size1	= size;
	out.size2	= 1;
	out.ResType	= _operand_get_TargetType(baseType, out.nDims);
	out.array	= ce_DSNewPtr(size * elSize);
	out.alloc	= (out.array != NULL);
	if (!out.alloc) { 
		err = mFullErr; 
		goto out; 
	}
	// ==============================================
	// ¬торой цикл по аргументам. «аполнение массива.
	// ==============================================
	for (idx=0, opd=res, pnt=out.array; idx < arg_qty; idx++, opd++) {

		// получить указатель (dbl,cdb) или создать массив указателей (str) на данные
		if (baseType == OPTYPE_STR)	_prepare_strings((_OperandBase*)opd, (_str_operand*)&tmp);
		else						_prepare_operand((_OperandBase*)opd, (_dbl_operand*)&tmp, &itComplex);

		// сохранить
		_MoveBlock(tmp.array, pnt, tmp.size1 * elSize);
		pnt += tmp.size1 * elSize;

		// освободить пам€ть (если выдел€лась)
		if (tmp.alloc) {
			DSDisposePtr(tmp.array);
			tmp.alloc = FALSE;
		}
	}
	// ===================================
	// —охранение данных и типа результата
	// ===================================
	res->type = out.ResType;
	if (baseType == OPTYPE_STR)	err = _save_strings( (_str_operand*)&out, &(res->str));
	else						err = _save_array  ( (_dbl_operand*)&out, &(res->str));
out:
	ip->OpdNum -= arg_qty - 1;
	if (tmp.alloc)	DSDisposePtr(tmp.array);
	if (out.alloc)	DSDisposePtr(out.array);
	return err;
}
