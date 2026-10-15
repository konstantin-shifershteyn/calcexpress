/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <celib\ceMemLib.h>

// --------------------
__inline	MgErr OprStack_CreateStack(_ScrParserInst *pi)
{
	MgErr	err = noErr;

	pi->OprStk = (_OprInfoArrayHdl)DSNewHandle(sizeof(_OprInfoArray) + sizeof(_OprInfo) * STM_BLKSIZE);
	if (!(pi->OprStk)) {
		err = mFullErr;
		goto out;
	}
	(*(pi->OprStk))->dimSize = STM_BLKSIZE;	// размер массива
out:
	pi->OprNum = 0;
	return err;
}

// --------------------
__inline	MgErr OprStack_DestroyStack(_ScrParserInst *pi)
{
	MgErr	err = noErr;

	if (pi->OprStk)
		err = DSDisposeHandle(pi->OprStk);
	pi->OprStk = NULL;
	pi->OprNum = 0;
	return err;
}

// --------------------
__forceinline _OprInfo * OprStack_TopPnt(_ScrParserInst *pi)
{
	if (pi->OprNum <= 0)
		return NULL;
	return	&((*(pi->OprStk))->data[pi->OprNum]);
}

// --------------------
__forceinline _OprInfo * OprStack_GetPnt(_ScrParserInst *pi, int32 idx)
{
	if ((idx <= 0) || (idx > pi->OprNum))
		return NULL;
	return	&((*(pi->OprStk))->data[idx]);
}

// --------------------
__forceinline int32  OprStack_TopIndex(_ScrParserInst *pi)
{
	return	pi->OprNum;
}

// --------------------
__forceinline int32 OprStack_DecLen(_ScrParserInst *pi)
{
	if (pi->OprNum > 0) 
		pi->OprNum -= 1;
	return	pi->OprNum;
}

// --------------------
__inline MgErr OprStack_Push(_ScrParserInst *pi, _OprInfo *elp)
{
	MgErr err = noErr;

	pi->OprNum += 1;		// увеличить счетчик элементов

	// Если таблица заполнена, то расширить
	// ------------------------------------
	if (pi->OprNum  >= (*(pi->OprStk))->dimSize) {
		int32	size	= (*(pi->OprStk))->dimSize;
		int32	newSize = size + STM_BLKSIZE;

		if (err = ce_1DArrayResize((UHandle*)&(pi->OprStk), newSize, sizeof(_OprInfo)) )
			goto out;
	}
	// заполнить элемент таблицы
	// -------------------------
	(*(pi->OprStk))->data[pi->OprNum] = *elp;

out:
	return err;

}
