/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#ifndef	__PrepareResult1D_C__
#define	__PrepareResult1D_C__

// ==========================================================================
// 	Подготавливает результат индексации 1D Array (DBL, CDB, STR)
// ==========================================================================
MgErr _index_a1_init_res(
				_gen_operand	*arr,
				_idx_info		*iip,
				_gen_operand	*res,
				_idx_HowToProc	*ihtp)
{
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

	// ==========================================
	// *****  Определить параметры индекса  *****
	// ==========================================
	switch (iip->nDims) {

	case -1 :	// Диапазон индексов
		res->nDims = 1;

		if (arr->size1 <= 0) {		// if empty array 
			res->size1 = 0;
			break;
		}
		iri1 = iip->size1;		// первый индекс диапазона
		iri2 = iip->size2;		// второй индекс диапазона

		if (iri1 < 0)		iri1 = 0;
		if (iri2 < 0)		iri2 = 0;
		if (iri1 >= arr->size1) iri1 = arr->size1 - 1;
		if (iri2 >= arr->size1) iri2 = arr->size1 - 1;

		iip->scalar = iri1;

		res->size1 = iri2 - iri1;
		if   (	res->size1 >= 0)			{	ihtp->i1type= idxIncremental;	}
		else {	res->size1 = abs(res->size1);	ihtp->i1type= idxDecremental;	}
		res->size1 += 1;
		break;

	case 0 :	// Индекс - скаляр
		ihtp->i1type = idxScalar;
		break;

	case 1 :	// Индекс - 1D Array
		ihtp->i1type = idxArray;
		res->nDims = 1;
		res->size1 = iip->size1;
		break;

	default:
		res->erMsg = "Internal Error: 2D Array is met as index in "__FUNCTION__"()";
		goto out;
	}
	res->ResType = _operand_get_TargetType(arr->ResType, res->nDims);

	// ===========================================
	// *****  Выделить память под результат  *****
	// ===========================================
	if (ihtp->allocArray) {
		switch(res->ResType) {
			case OPTYPE_DBL :	goto out;
			case OPTYPE_A1DBL :	elSize = sizeof(float64);	break;
			case OPTYPE_CDB :
			case OPTYPE_A1CDB :	elSize = sizeof(float64)*2;	break;
			case OPTYPE_STR :
			case OPTYPE_A1STR :	elSize = sizeof(LStr*);		break;
			default:
				res->erMsg = "Internal Error: Unexpected datatype is met in "__FUNCTION__"()";
				goto out;
		}
//DbgPrintf("_index_a1_init_res(): Allocate array\n");
		res->array = ce_DSNewPtr(res->size1 * res->size2 * elSize);
		res->alloc = (res->array != NULL);
		if (!res->alloc) err = mFullErr;
	}
out:
	return err;
}

#endif
