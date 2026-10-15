/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <Index_Routines.h>


// >>> #CE_3.0.4 2026-07-04
//==================
void	_dispose_index_array(_idx_info *idx)
{
	if (!(idx->alloc))
		return;
//	DbgPrintf(__FUNCTION__"()\n");
	DSDisposePtr(idx->values);
	idx->values		= NULL;
	idx->alloc		= FALSE;
	idx->allocVals	= 0;
}
//==================
MgErr	_allocate_index_array(_idx_info *idx, int32 idxArrSize)
{
	if (idx->alloc) {
		if (idx->allocVals >= idxArrSize) {
			return	noErr;	// do nothing if an Index Array is already allocated and has enough size
		}
		_dispose_index_array(idx);	// if existing array is too small, we need dispose it and allocate new one then
	}
	// allocate new Index Array
//	DbgPrintf(__FUNCTION__"(): allocate size=%d\n", idxArrSize);
	idx->values		= (int32*)ce_DSNewPtr( idxArrSize * sizeof(idx->values));
	idx->alloc		= (idx->values != NULL);
	idx->allocVals	= idx->alloc ? idxArrSize : 0;
	return (idx->alloc ? noErr : mFullErr);
}
// <<< #CE_3.0.4 2026-07-04


// >>> #CE_3.0.4 2026-07-04
// ===========================================================
// Подготавливает индексы для операций индексации массивов
// ===========================================================
MgErr	_prepare_index(_Operand *arg, _idx_info *idx)
{
	int32	*ipnt, size;
	float64	*fpnt;

	idx->erMsg	= NULL;

	switch(arg->type)
	{
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
					idx->scalar	= _DblToI32(arg->num);
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
	// Index is 1D Array: allocate (or reuse) Index Array
	size = idx->size1 * idx->size2;
	if (_allocate_index_array(idx, size))
		return mFullErr;

	fpnt = (float64*)ipnt;
	ipnt = idx->values;
	while(size--) {
		*ipnt++ = _DblToI32(*fpnt++);		//*ipnt++ = (int32)*fpnt++;
	}
	return noErr;
}
// <<< #CE_3.0.4 2026-07-04

// --------------------------------------------
// Функция упаковывает два индекса в DBL Scalar
// --------------------------------------------
void	_PackIdxRangeToFloat(float64 *buf, int32 i1, int32 i2)
{
	int32	*idxp = (int32*)buf;

	*idxp++	= (i1 < 0) ? 0 : i1;
	*idxp	= (i2 < 0) ? 0 : i2;
}

// ---------------------------------------
// Функция извлекает индексы из DBL Scalar
// ---------------------------------------
int32	_UnpackIdxRangeFromFloat(float64 *buf, int32 idxNum)
{
	int32	idx;

	if	(idxNum > 1)	idxNum = 1;
	else if	(idxNum < 0)	idxNum = 0;

	idx = *((int32*)buf + idxNum);
	return (idx < 0) ? 0 : idx;
}

// --------------------------------------
// Функция создает упакованный IndexRange
// с максимальными крайними границами
// --------------------------------------
float64	_MakeIndexRange(Bool32	incremental)
{
	float64	idxRange;

	if (incremental)	_PackIdxRangeToFloat(&idxRange, _IdxRange_MIN, _IdxRange_MAX);
	else				_PackIdxRangeToFloat(&idxRange, _IdxRange_MAX, _IdxRange_MIN);
	return idxRange;
}


