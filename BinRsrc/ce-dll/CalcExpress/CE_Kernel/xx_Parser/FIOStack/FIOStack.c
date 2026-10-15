/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr FIOStack_CreateStack(_CE_Instance *ip)
{
	MgErr	err = noErr;

	// ### create array
	// >>> #CE_3.0.4 2026-07-05
	ip->FIOStk = NULL;
	if (err = ce_CreateOrResizeAlignedArray((UHandle*)&(ip->FIOStk), FIOSTACK_BLKSIZE, sizeof(_FuncIdxOp), sizeof(int32), offsetof(_FuncIdxOp_Array,data)))
		goto out;
//	(*(ip->FIOStk))->dimSize = FIOSTACK_BLKSIZE;	// no need, ce_CreateOrResizeAlignedArray sets DimSize
	ip->FIOStkDat	= &((*(ip->FIOStk))->data[0]);	// >>> #CE_3.0.2 2026-06-20
	ip->FIOStkLen	= FIOSTACK_BLKSIZE;				// >>> #CE_3.0.2 2026-06-20
out:
	ip->FIONum = 0;
	return err;
}

MgErr FIOStack_DestroyStack(_CE_Instance *ip)
{
	MgErr	err = noErr;

	if (ip->FIOStk)
		err = DSDisposeHandle(ip->FIOStk);

	ip->FIOStkDat = NULL;		// >>> #CE_3.0.2 2026-06-20
	ip->FIOStkLen = 0;			// >>> #CE_3.0.2 2026-06-20
	ip->FIOStk = NULL;
	ip->FIONum = 0;
	return err;
}

__forceinline	_FuncIdxOp * FIOStack_TopPnt(_CE_Instance *ip)
{
	if (ip->FIONum <= 0)
		return NULL;
//	return	&((*(ip->FIOStk))->data[ip->FIONum]);
	return	&(ip->FIOStkDat[ip->FIONum]);		// >>> #CE_3.0.2 2026-06-19	
}

__forceinline	int32 FIOStack_DecLen(_CE_Instance *ip) {
	if (ip->FIONum > 0) 
		ip->FIONum -= 1;
	return	ip->FIONum;
}

__forceinline	void FIOStack_Init(_CE_Instance *ip)
{
	ip->FIONum = 0;
}

__forceinline	void FIOStack_Pop(_CE_Instance *ip, _FuncIdxOp *elp, LVBoolean *err_f)
{
	if (*err_f)	return;
	if (ip->FIONum <= 0) {
		*err_f = LVTRUE;
	}
	else {
//		*elp = (*(ip->FIOStk))->data[ip->FIONum];
		*elp = ip->FIOStkDat[ip->FIONum];			// >>> #CE_3.0.2 2026-06-20
		ip->FIONum -= 1;
	}
}

__forceinline	void FIOStack_GetTop(_CE_Instance *ip, _FuncIdxOp *elp, LVBoolean *err_f)
{
	if (*err_f)	return;
	if (ip->FIONum <= 0) {
		*err_f = LVTRUE;
	}
	else {
//		*elp = (*(ip->FIOStk))->data[ip->FIONum];
		*elp = ip->FIOStkDat[ip->FIONum];			// >>> #CE_3.0.2 2026-06-20
	}
}

__forceinline	void FIOStack_SetTop(_CE_Instance *ip, _FuncIdxOp *elp, LVBoolean *err_f)
{
	if (*err_f)	return;
	if (ip->FIONum <= 0) {
		*err_f = LVTRUE;
	}
	else {
//		(*(ip->FIOStk))->data[ip->FIONum] = *elp;
		ip->FIOStkDat[ip->FIONum] = *elp;			// >>> #CE_3.0.2 2026-06-20
	}
}

__forceinline	void FIOStack_SetArgFlag(_CE_Instance *ip, LVBoolean *err_f)
{
	if (*err_f)	return;
	if (ip->FIONum <= 0) {
		*err_f = LVTRUE;
	}
	else {
//		((*(ip->FIOStk))->data[ip->FIONum]).w_arg = TRUE;
		ip->FIOStkDat[ip->FIONum].w_arg = TRUE;		// >>> #CE_3.0.2 2026-06-20
	}
}

// ==========================================
__forceinline	MgErr FIOStack_ResizeStack(_CE_Instance *ip, int32 newsize)
{
	MgErr err;

	if (newsize == FIOStack_Size(ip))
		return noErr;

//	DbgPrintf("FIOStack_ResizeStack (%s): %d\n", newsize < FIOStack_Size(ip) ? "compress" : " expand ", newsize);

	// >>> #CE_3.0.4 2026-07-05
	if (err = ce_CreateOrResizeAlignedArray((UHandle*)&(ip->FIOStk), newsize, sizeof(_FuncIdxOp), sizeof(int32), offsetof(_FuncIdxOp_Array,data)))
		goto out;
//	(*(ip->FIOStk))->dimSize = newsize;				// no need, ce_1DArrayResize() sets this value
	ip->FIOStkDat	= &((*(ip->FIOStk))->data[0]);	// >>> #CE_3.0.2 2026-06-20
	ip->FIOStkLen	= newsize;						// >>> #CE_3.0.2 2026-06-20
out:
	return	err;
}
// ==========================================
__forceinline	MgErr FIOStack_Push(_CE_Instance *ip, _FuncIdxOp *elp)
{
	MgErr err;

	ip->FIONum += 1;		// увеличить счетчик элементов

	// Если таблица заполнена, то расширить
	// ------------------------------------
	if (ip->FIONum  >= FIOStack_Size(ip)) {
		if (err = FIOStack_ResizeStack(ip, FIOStack_Size(ip) + FIOSTACK_BLKSIZE))
			return err;
	}
	// заполнить элемент таблицы
	// -------------------------
//	(*(ip->FIOStk))->data[ip->FIONum] = *elp;
	ip->FIOStkDat[ip->FIONum] = *elp;			// >>> #CE_3.0.2 2026-06-20

	return noErr;
}
