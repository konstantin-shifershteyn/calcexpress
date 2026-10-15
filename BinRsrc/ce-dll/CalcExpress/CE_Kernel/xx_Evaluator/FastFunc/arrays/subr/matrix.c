/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ========================================================================================
//	matrix( x1 [,x2 ... ,xn] )	===================================================
// ========================================================================================
MgErr	f_arr_matrix(
			_CE_Instance	*ip,
			_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
			int32			arg_qty,	//кол-во аргументов
			LVBoolean		*err_f)
{
	MgErr			err = noErr;
	_Operand		*opd;		// ”казатель на исходные операнды
	_Operand		*res;		// ”казатель на результат
	_gen_operand	tmp, out;

	int32		idx, i, j;
	int32		rows, cols;
	int32		baseType = OPTYPE_DBL, elSize = sizeof(float64);
	char		*pnt;
	Bool32		itComplex = FALSE;

	void		*stub;
static	LStr	nulStr		= {0, 0};
static	void	*zeroStr	= &nulStr;
static	float64	zeroNum[2]	= {0, 0};

//------------------------

	tmp.alloc = out.alloc = FALSE;
	tmp.erMsg = out.erMsg = NULL;

	idx = ip->OpdNum - (arg_qty - 1);	// индекс первого операнда, он-же - результат
	res = &((*(ip->OpdStk))->data[idx]);	// указатель на результат

	// ===============================================
	// 1-й цикл по аргументам. ѕодсчет общего размера.
	// ===============================================
	rows = cols = 0;
	for (idx=0, opd = res; idx < arg_qty; idx++, opd++) {

		// ***** определить параметры аргумента
		if (opd->type == OPTYPE_IDENT) {
			err = GetVariable( ip, opd, (_OperandBase*)opd, err_f);
			if (err || *err_f)	goto out;
		}
		_general_operand_info((_OperandBase*)opd, &tmp);

		if (!idx) {		// ***** по первому операнду опрдел€ем тип массива
			switch(baseType = _operand_get_BaseType(tmp.ResType)) {
				case OPTYPE_DBL :	elSize = sizeof(float64);		break;
				case OPTYPE_CDB :	elSize = sizeof(float64) * 2;	break;
				case OPTYPE_STR :	elSize = sizeof(LStr*);			break;
				default :		elSize = 0;	goto invType;
			}
		}
		else {			// ***** все аргументы должны иметь один базовый тип
			if (baseType != _operand_get_BaseType(tmp.ResType)) {
invType:			err = CE_Error(ip, "Invalid type of parameter %d.", (LStrHandle)(size_t)(idx+1), NULL);
				*err_f = LVTRUE;
				goto out;
			}
		}
		switch (tmp.nDims) {	// ***** подсчет размеров массива
			case 0:	rows += 1;		cols = __max(cols, 1);		break;
			case 1:	rows += 1;		cols = __max(cols, tmp.size1);	break;
			case 2:	rows += tmp.size1;	cols = __max(cols, tmp.size2);	break;
		}
	}
	// -------------------------------
	// ѕодготовка структуры результата
	// -------------------------------
	out.nDims	= 2;
	out.size1	= rows;
	out.size2	= cols;
	out.ResType	= _operand_get_TargetType(baseType, out.nDims);
	out.array	= ce_DSNewPtr(rows * cols * elSize);
	out.alloc	= (out.array != NULL);
	if (!out.alloc) { err = mFullErr; goto out; }

	// «аглушка дл€ заполнени€ "пустых" элементов массива
	// --------------------------------------------------
	if (baseType == OPTYPE_STR)
		stub = &zeroStr;
	else	stub = &zeroNum;


	// ==============================================
	// ¬торой цикл по аргументам. «аполнение массива.
	// ==============================================
	for (idx=0, opd=res, pnt=out.array; idx < arg_qty; idx++, opd++) {

		// получить указатель (dbl,cdb) или создать массив указателей (str) на данные
		if (baseType == OPTYPE_STR)
				_prepare_strings((_OperandBase*)opd, (_str_operand*)&tmp);
		else	_prepare_operand((_OperandBase*)opd, (_dbl_operand*)&tmp, &itComplex);

		switch(tmp.nDims) {
			case 0 :	// *****  scalar or string
			case 1 :	// *****  1D Array
				// сохранить
				_MoveBlock(tmp.array, pnt, tmp.size1 * elSize);
				pnt += tmp.size1 * elSize;
				// заполнить "пустые" элементы массива
				for(j=tmp.size1; j < cols; j++) {
					_MoveBlock(stub, pnt, elSize);
					pnt += elSize;
				}
				break;

			case 2 :	// *****  2D array
				for (i=0; i < tmp.size1; i++) {	// цикл по строкам
					// сохранить
					_MoveBlock((char*)tmp.array + i*tmp.size2*elSize, pnt, tmp.size2*elSize);
					pnt += tmp.size2 * elSize;
					// заполнить "пустые" элементы массива
					for (j=tmp.size2; j < cols; j++) {
						_MoveBlock(stub, pnt, elSize);
						pnt += elSize;
					}
				}
				break;
		}
		if (tmp.alloc) {	// освободить пам€ть (если выдел€лась)
			DSDisposePtr(tmp.array);
			tmp.alloc = FALSE;
		}
	}
	// ===================================
	// —охранение данных и типа результата
	// ===================================
	res->type = out.ResType;
	if (baseType == OPTYPE_STR)	err = _save_strings( (_str_operand*)&out, &(res->str));
	else				err = _save_array  ( (_dbl_operand*)&out, &(res->str));
out:
	ip->OpdNum -= arg_qty - 1;
	if (tmp.alloc)	DSDisposePtr(tmp.array);
	if (out.alloc)	DSDisposePtr(out.array);
	return err;
}
