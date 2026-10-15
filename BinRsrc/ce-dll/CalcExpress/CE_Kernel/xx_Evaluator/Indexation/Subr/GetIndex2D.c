/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =========================================================
//		Индексирует 2D массивы
// =========================================================
MgErr	_index_2d_arr(	_gen_operand	*opd,
						_indexes_info	*idxs,
						Bool32			onlyDblScalar	// >>> #CE_3.0 2025-04-22
						)
{
	// тип: указатель на функцию индексирования
	typedef void (*_idx2dfunc)();
	// Структура описывает индекс со значением -1
//	static	_idx_info	idxStub = {-1, 0, 0x7FFFFFFF, 0, &idxStub.scalar, FALSE, NULL };
	static	_idx_info	idxStub = {-1, 0, 0x7FFFFFFF, 0, &idxStub.scalar, FALSE, 0, NULL };		// >>> #CE_3.0.4 2026-07-04

	_idx_info		*iip1,	*iip2;		// указатели на структуры описания индексов
	int32			*i1Arr, *i2Arr;		// указатели на массивы индексов
	_gen_operand	res;				// структура результата
	void			*rsArr;				// указатель на массив результата
	int32			i,j, size;
	int32			idx1, idx2, elSize;
	_idx_HowToProc	ProcInf;
	_idx2dfunc		_getfunc;			// указатель на функцию индексирования
	MgErr			err = noErr;
	//--------------

	if (opd->nDims != 2) {
		opd->erMsg = "Internal Error: incorrect call of the function (not 2D array) in "__FUNCTION__"()";
		return err;
	}

	iip1 = &(idxs->idx1);
	if (idxs->qty < 2)	iip2 = &(idxStub);
	else				iip2 = &(idxs->idx2);

	// параметры для инициализации результата
	// --------------------------------------
	ProcInf.compound	= idxs->compound;
	ProcInf.idxAmount	= idxs->qty;
//	ProcInf.allocArray	= TRUE;
	ProcInf.allocArray	= ! onlyDblScalar;	// >>> #CE_3.0 2025-04-22

	// ***** подготовить структуру результата
	// ======================================
	if (err = _index_a2_init_res(opd, iip1, iip2, &res, &ProcInf))
		goto out;
	if (res.erMsg)
		goto out;

	// Зависимости от типа данных
	// ==========================
	switch (_operand_get_BaseType(opd->ResType)) {
		case OPTYPE_DBL :	elSize = sizeof(float64);
							_getfunc = get_element_of_A2DBL;
							break;
		case OPTYPE_CDB :	elSize = sizeof(float64) * 2;
							_getfunc = get_element_of_A2CDB;
							break;
		case OPTYPE_STR:	elSize = sizeof(LStr*);
							_getfunc = get_element_of_A2STR;
							break;
	}

	// ==================================
	// =====  Выполнить индексацию  =====
	// ==================================
	i1Arr	= iip1->values;
	i2Arr	= iip2->values;
	rsArr	= res.array;	// is NULL if output buffer was not allocated

	switch(res.nDims)
	{
	case 0:	// *** Результат - скаляр
		// ======================
		if (res.ResType == OPTYPE_DBL) {
			_getfunc(opd, *i1Arr, *i2Arr, &(opd->scalar));
			opd->ResType = OPTYPE_DBL;
			opd->nDims   = 0;
			opd->size1   = opd->size2 = 1;
			return err;
		}
		if (!rsArr)	break;	// >>> #CE_3.0 2025-04-22
		_getfunc(opd, *i1Arr, *i2Arr, rsArr);
		break;
		// ---------------------
	case 1:	// *** Результат - 1D Array
		//=========================
		if (!rsArr)	break;	// >>> #CE_3.0 2025-04-22
		size = res.size1;	// * res.size2;
		idx1 = *i1Arr;
		idx2 = *i2Arr;
		for (i=0; i<size; i++) {
			switch(ProcInf.i1type) {
				case idxArray :			idx1 = i1Arr[i];	break;
				case idxIncremental :	idx1 = *i1Arr + i;	break;
				case idxDecremental :	idx1 = *i1Arr - i;	break;
			}
			switch(ProcInf.i2type) {
				case idxArray :			idx2 = i2Arr[i];	break;
				case idxIncremental :	idx2 = *i2Arr + i;	break;
				case idxDecremental :	idx2 = *i2Arr - i;	break;
			}
			_getfunc(opd, idx1, idx2, rsArr);
			(char*)rsArr += elSize;
		}
		break;
		// ---------------------
	case 2:	// *** Результат - 2D Array
		// ========================
		if (!rsArr)	break;	// >>> #CE_3.0 2025-04-22
		idx1 = *i1Arr;
		idx2 = *i2Arr;
		for(i=0; i < res.size1; i++) {
			switch(ProcInf.i1type) {
			case idxArray :			idx1 = i1Arr[i];	break;
			case idxIncremental :	idx1 = *i1Arr + i;	break;
			case idxDecremental :	idx1 = *i1Arr - i;	break;
			}
			for (j=0; j < res.size2; j++) {
				switch(ProcInf.i2type) {
				case idxArray :			idx2 = i2Arr[j];	break;
				case idxIncremental :	idx2 = *i2Arr + j;	break;
				case idxDecremental :	idx2 = *i2Arr - j;	break;
				}
				_getfunc(opd, idx1, idx2, rsArr);
				(char*)rsArr += elSize;
			}
		}
		break;
	}
	// *** РЕЗУЛЬТАТ
out:
	if (opd->alloc)
		err = DSDisposePtr(opd->array);
	*opd = res;
	return	err;
}
