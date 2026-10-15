/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__IndexesRoutines_H__
#define	__IndexesRoutines_H__

#include <extcode.h>

#include <ceKernel.h>
#include <iDataTypes.h>

#include <celib\ceMemLib.h>


#define	_IdxRange_MIN	0
#define	_IdxRange_MAX	0x7FFFFFFF

void	_PackIdxRangeToFloat(float64 *buf, int32 i1, int32 i2);
int32	_UnpackIdxRangeFromFloat(float64 *buf, int32 idxNum);
float64	_MakeIndexRange(Bool32	incremental);

MgErr	_prepare_index(_Operand *arg, _idx_info *idx );
MgErr	_allocate_index_array(_idx_info *idx, int32 idxArrSize);
void	_dispose_index_array(_idx_info *idx);


// ### Функция создает структуру Индексной Информации
__inline MgErr __fastcall	_IndexInfo_Create( _Operand *opd)
{
//	if (opd->iip) {
//		DbgPrintf(__FUNCTION__"(): opd->iip is already allocated\n");
//		return noErr;
//	}
	opd->iip = (_indexes_info*)ce_DSNewPClr( sizeof(_indexes_info) );
	if (!(opd->iip))
		return mFullErr;

	opd->iip->qty = 0;
	opd->iip->idx1.alloc  = opd->iip->idx2.alloc  = FALSE;
	opd->iip->idx1.values = opd->iip->idx2.values = NULL;
	return noErr;
}

// ### Функция отменяет индексацию
__forceinline void __fastcall	_IndexInfo_ResetIndexation(_Operand *opd)
{
	if (opd->iip) {
		opd->iip->qty = 0;
	}
}

// ### Функция очищает структуру Индексной Информации
__forceinline void __fastcall	_IndexInfo_FreeValues(_Operand *opd)
{
	_indexes_info	*iip;

	if (iip = opd->iip) {
		if (iip->idx1.alloc) {
			//DbgPrintf(__FUNCTION__"(): iip->idx1.values\n");
			_dispose_index_array(&(iip->idx1));		// >>> #CE_3.0.4 2026-07-04
		}
		if (iip->idx2.alloc) {
			//DbgPrintf(__FUNCTION__"(): iip->idx2.values\n");
			_dispose_index_array(&(iip->idx2));		// >>> #CE_3.0.4 2026-07-04
		}
		iip->qty = 0;
	}
}

// ### Функция уничтожает структуру Индексной Информации
__inline void __fastcall	_IndexInfo_Destroy( _Operand *opd)
{
	if (opd->iip) {
//DbgPrintf(__FUNCTION__"()\n");
		_IndexInfo_FreeValues(opd);
		DSDisposePtr( opd->iip );
		opd->iip = NULL;
	}
//else DbgPrintf(__FUNCTION__"(): Do nothing\n");
}


#endif
