/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <Index_Routines.h>

// the functions from this file are moved to "ce-dll\CalcExpress\CE_Routines\Index_Routines.c"
// declarations are in the file				 "\include\CalcExpress\Index_Routines.h"

/*
// ===========================================================
// Подготавливает индексы для операций индексации массивов
// ===========================================================
MgErr	_prepare_index(_Operand *arg, _idx_info *idx)
{
	int32	*ipnt, size;
	float64	*fpnt;

	if (idx->alloc) {			// to prevent memory leak
		DSDisposePtr(idx->values);
		idx->values = NULL;
		idx->alloc  = FALSE;
	}
	idx->erMsg	= NULL;

	switch(arg->type) {

		case OPTYPE_IDXRANGE:	// Диапазон индексов
					idx->nDims	= -1;
					idx->size1	= _UnpackIdxRangeFromFloat(&(arg->num), 0);
					idx->size2	= _UnpackIdxRangeFromFloat(&(arg->num), 1);
					idx->scalar	= idx->size1;
					idx->values	= &(idx->scalar);
					return	noErr;

		case OPTYPE_DBL :	// Индекс - скаляр
					idx->nDims	= 0;
					idx->size1	= idx->size2 = 1;
					idx->scalar	= _DblToInt(arg->num);
					idx->values	= &(idx->scalar);
					return noErr;

		case OPTYPE_A1DBL :	// Индекс - 1D Array
					ipnt		= (int32*)LStrBuf(*(arg->str));
					idx->nDims	= 1;
					idx->size1	= *ipnt++;
					idx->size2	= 1;
// >>> #CE_3.0 2024-12-14
//#if defined(_M_X64)
//					ipnt++;	// skip padding bytes
//#endif
// <<< #CE_3.0 2024-12-14
					idx->scalar	= 0;
					break;

//		case OPTYPE_A2DBL :	// Индекс - 2D Array
//					ipnt		= (int32*)LStrBuf(*(arg->str));
//					idx->nDims	= 2;
//					idx->size1	= *ipnt++;
//					idx->size2	= *ipnt++;
//					idx->scalar	= 0;
//					break;
	
	default:	idx->nDims	= 0;
					idx->size1	= idx->size2 = 1;
					idx->scalar	= 0;
					idx->values	= &(idx->scalar);
					idx->erMsg	= "Invalid type of index expression";
					return	noErr;	
	}
	size = idx->size1 * idx->size2;
	//DbgPrintf("ce_DSNewPtr( size * sizeof(idx->values))\n");
	idx->values	= ce_DSNewPtr( size * sizeof(idx->values));
	idx->alloc	= (idx->values != NULL);
	if (!(idx->alloc))	return mFullErr;

	fpnt = (float64*)ipnt;
	ipnt = idx->values;
	while(size--) {
		*ipnt++ = _DblToInt(*fpnt++);		//*ipnt++ = (int32)*fpnt++;
	}
	return noErr;
}
*/
