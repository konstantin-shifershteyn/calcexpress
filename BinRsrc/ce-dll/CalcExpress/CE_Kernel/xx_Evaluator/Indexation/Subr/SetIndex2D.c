/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =====================================================================
//	Устанавливает значение индексированных элементов 2D массивов
// =====================================================================

void	_set_idxEl_2d_arr(_gen_operand *arr, _indexes_info *idxs, _gen_operand *val)
{
	// тип: указатель на функцию индексирования
	typedef void (*_idx2dfunc)();
	// Структура описывает индекс со значением -1
//	static	_idx_info	idxStub = {-1, 0, 0x7FFFFFFF, 0, &idxStub.scalar, FALSE, NULL };
	static	_idx_info	idxStub = {-1, 0, 0x7FFFFFFF, 0, &idxStub.scalar, FALSE, 0, NULL };	// >>> #CE_3.0.4 2026-07-04

	_idx_info	*ip1,	*ip2;		// указатели на структуры описания индексов
	int32		*i1Arr, *i2Arr;		// указатели на массивы индексов

	int32		i,j;
	int32		idx1, idx2;
	int32		size1, size2;

	_gen_operand	res;
	_idx_HowToProc	ProcInf;

	_idx2dfunc	_getfunc1, _getfunc2;	// указатели на функции индексирования
	_idx2dfunc	_setfunc;		// указатель на функцию присваивания
	char		buf[2*sizeof(float64)];
//-----------

	if (arr->nDims != 2) {
		arr->erMsg = "Internal Error: incorrect call of the function (not 2D array) in "__FUNCTION__"()";
		return;
	}

	ip1	= &(idxs->idx1);
	if (idxs->qty < 2)	ip2 = &(idxStub);
	else			ip2 = &(idxs->idx2);

	// параметры для инициализации
	// ---------------------------
	ProcInf.compound	= idxs->compound;
	ProcInf.idxAmount	= idxs->qty;
	ProcInf.allocArray	= FALSE;

	// подготовить структуру результата
	// --------------------------------
	_index_a2_init_res(arr, ip1, ip2, &res, &ProcInf);
	if (res.erMsg) {
		arr->erMsg = res.erMsg;
		return;
	}
	// ---
	// размерность присваемого значения (источник) не может превосходить
	// размерность области массива (приемник)
	// ---
	if (res.nDims < val->nDims) {
		arr->erMsg = "Incompatible operand dimensions in assignment";
		return;
	}

	// Зависимости от типа данных
	// ==========================
	switch (_operand_get_BaseType(arr->ResType)) {
		case OPTYPE_DBL :	_setfunc  = set_element_of_A2DBL;
					_getfunc1 = get_element_of_A1DBL;
					_getfunc2 = get_element_of_A2DBL;
					break;
		case OPTYPE_CDB :	_setfunc  = set_element_of_A2CDB;
					_getfunc1 = get_element_of_A1CDB;
					_getfunc2 = get_element_of_A2CDB;
					break;
		case OPTYPE_STR :	_setfunc  = set_element_of_A2STR;
					_getfunc1 = get_element_of_A1STR;
					_getfunc2 = get_element_of_A2STR;
					break;
	}

	// ================================
	// *****  присвоить значения  *****
	// ================================

	i1Arr	= ip1->values;
	i2Arr	= ip2->values;

	switch(res.nDims) {

	case 0:	// *** Результат - скаляр
		// ======================
		_getfunc1(val, 0, buf);
		_setfunc(arr, *i1Arr, *i2Arr, buf);
		return;

		// ---------------------
	case 1:	// *** Результат - 1D Array
		// ========================
		size1 = res.size1;	// !!!
		if (val->nDims) {
			if (val->size1 < size1)
				size1 = val->size1;
		}
		else
			_getfunc1(val, 0, buf);		// scalar value
		idx1 = *i1Arr;
		idx2 = *i2Arr;
		for (i=0; i < size1; i++) {
			switch(ProcInf.i1type) {
				case idxArray :		idx1 = i1Arr[i];	break;
				case idxIncremental :	idx1 = *i1Arr + i;	break;
				case idxDecremental :	idx1 = *i1Arr - i;	break;
			}
			switch(ProcInf.i2type) {
				case idxArray :		idx2 = i2Arr[i];	break;
				case idxIncremental :	idx2 = *i2Arr + i;	break;
				case idxDecremental :	idx2 = *i2Arr - i;	break;
			}
			if (val->nDims)	_getfunc1(val, i, buf);
			_setfunc(arr, idx1, idx2, buf);
		}
		return;

		// ---------------------
	case 2:	// *** Результат - 2D Array
		// ========================
		size1 = res.size1;	// !!!
		size2 = res.size2;	// !!!
		switch(val->nDims) {
			case 1:	if (val->size1 < size2)	size2 = val->size1;
				break;
			case 2:	if (val->size1 < size1)	size1 = val->size1;
				if (val->size2 < size2)	size2 = val->size2;
				break;
			default:
				_getfunc1(val, 0, buf);		// scalar value
				break;
		}
		idx1 = *i1Arr;
		idx2 = *i2Arr;
		for(i=0; i < size1; i++) {
			switch(ProcInf.i1type) {
				case idxArray :		idx1 = i1Arr[i];	break;
				case idxIncremental :	idx1 = *i1Arr + i;	break;
				case idxDecremental :	idx1 = *i1Arr - i;	break;
			}
			for (j=0; j < size2; j++) {
				switch(ProcInf.i2type) {
				case idxArray :		idx2 = i2Arr[j];	break;
				case idxIncremental :	idx2 = *i2Arr + j;	break;
				case idxDecremental :	idx2 = *i2Arr - j;	break;
				}
				switch(val->nDims) {
					case 1:	_getfunc1(val, j,    buf);	break;
					case 2: _getfunc2(val, i, j, buf);	break;
				}
				_setfunc(arr, idx1, idx2, buf);
			}
		}
	}
}
