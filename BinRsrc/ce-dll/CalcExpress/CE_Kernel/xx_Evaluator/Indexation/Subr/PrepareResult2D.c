/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#ifndef	__PrepareResult2D_C__
#define	__PrepareResult2D_C__

// ==========================================================================
// 	Подготавливает результат индексации 2D Array (DBL, CDB, STR)
// ==========================================================================
MgErr _index_a2_init_res(
				_gen_operand	*arr,
				_idx_info		*iip1,
				_idx_info		*iip2,
				_gen_operand	*res,
				_idx_HowToProc	*ihtp)
{
	int32		nDims1, size1;
	int32		nDims2, size2;
	int32		iri1, iri2;
	int32		elSize;
	MgErr		err = noErr;

	res->nDims   = 0;
	res->size1   = res->size2 = 1;
	res->ResType = OPTYPE_DBL;
	res->scalar  = 0;

	res->alloc   = FALSE;
	res->array   = NULL;
	res->erMsg   = NULL;

	// ===============================================
	// *****  Определить параметры 1-го индекса  *****
	// ===============================================
	switch (iip1->nDims) {

	case -1 :	// Диапазон индексов
		nDims1 = 1;
		if (arr->size1 <= 0) {
			size1  = 0;
			break;
		}
		iri1 = iip1->size1;	// первый индекс диапазона
		iri2 = iip1->size2;	// второй индекс диапазона

		if (iri1 < 0) iri1 = 0;
		if (iri2 < 0) iri2 = 0;
		if (iri1 >= arr->size1) iri1 = arr->size1 - 1;
		if (iri2 >= arr->size1) iri2 = arr->size1 - 1;

		iip1->scalar = iri1;

		size1	= iri2 - iri1;
		if (size1 >= 0)		{		ihtp->i1type= idxIncremental;	}
		else {	size1 = abs(size1);	ihtp->i1type= idxDecremental;	}
		size1 += 1;
		break;

	case 0 :	// Индекс - скаляр
		ihtp->i1type = idxScalar;
		nDims1 = 0;
		size1  = 1;
		break;
	case 1 :	// Индекс - массив
		ihtp->i1type = idxArray;
		nDims1 = 1;
		size1  = iip1->size1;
		break;
	default:
		res->erMsg = "Internal Error: 2D Array is met as the 1-st index in "__FUNCTION__"()";
		goto out;
	}

	// ===============================================
	// *****  Определить параметры 2-го индекса  *****
	// ===============================================
	switch (iip2->nDims) {

	case -1 :	// Диапазон индексов
		nDims2 = 1;
		if (arr->size2 <= 0) {
			size2  = 0;
			break;
		}
		iri1 = iip2->size1;	// первый индекс диапазона
		iri2 = iip2->size2;	// второй индекс диапазона

		if (iri1 < 0) iri1 = 0;
		if (iri2 < 0) iri2 = 0;
		if (iri1 >= arr->size2) iri1 = arr->size2 - 1;
		if (iri2 >= arr->size2) iri2 = arr->size2 - 1;

		iip2->scalar = iri1;

		size2	= iri2 - iri1;
		if (size2 >= 0) {		ihtp->i2type= idxIncremental;	}
		else {	size2 = abs(size2);	ihtp->i2type= idxDecremental;	}
		size2 += 1;
		break;

	case 0 :	// Индекс - скаляр
		ihtp->i2type = idxScalar;
		nDims2 = 0;
		size2  = 1;
		break;
	case 1:		// Индекс - массив
		ihtp->i2type = idxArray;
		nDims2 = 1;
		size2  = iip2->size1;
		break;
	default:
		res->erMsg = "Internal Error: 2D Array is met as the 2-nd index in "__FUNCTION__"()";
		goto out;
	}

	// =============================================
	// *****  Определить параметры результата  *****
	// =============================================
	switch(nDims1 + nDims2) {
		case 0 :	// Both indexes are scalars
				res->nDims = 0;
				res->size1 = res->size2 = 1;
				break;

		case 1 :	// One of indexes is scalar
				res->nDims = 1;
				res->size1 = nDims1 > nDims2 ? size1 : size2;
				res->size2 = 1;
				break;

		case 2 :	// Both indexes are 1D Array, idxIncremental, or idxDecremental
				if (ihtp->compound || (ihtp->idxAmount < 2)) {
					res->nDims = 2;
					res->size1 = size1;
					res->size2 = size2;
				}
				else {
					res->nDims = 1;
					res->size1 = size1 <= size2 ? size1 : size2;
					res->size2 = 1;
				}
				break;

		default:
				res->erMsg = "Internal Error: cumulative number of index dimensions exceeds 2 in "__FUNCTION__"()";
				goto out;
	}
	res->ResType = _operand_get_TargetType(arr->ResType, res->nDims);

	// ===========================================
	// *****  Выделить память под результат  *****
	// ===========================================
	if (ihtp->allocArray) {
		switch(res->ResType) {
			case OPTYPE_DBL :	goto out;
			case OPTYPE_A1DBL :
			case OPTYPE_A2DBL :	elSize = sizeof(float64);	break;
			case OPTYPE_CDB :
			case OPTYPE_A1CDB :
			case OPTYPE_A2CDB :	elSize = sizeof(float64) * 2;	break;
			case OPTYPE_STR :
			case OPTYPE_A1STR :
			case OPTYPE_A2STR :	elSize = sizeof(LStr*);		break;
			default:
				res->erMsg = "Internal Error: Unexpected datatype is met in "__FUNCTION__"()";
				goto out;
		}
		res->array = ce_DSNewPtr(res->size1 * res->size2 * elSize);
		res->alloc = (res->array != NULL);
		if (!res->alloc) err = mFullErr;
	}
out:
	return err;
}

#endif
