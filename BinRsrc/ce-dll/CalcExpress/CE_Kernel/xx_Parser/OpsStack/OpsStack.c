/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr OpsStack_CreateStack(_CE_Instance *ip)
{
	MgErr	err = noErr;

	// ### create array
	// >>> #CE_3.0.4 2026-07-05
	ip->OpsStk = NULL;
 	if ( err = ce_CreateOrResizeAlignedArray((UHandle*)&(ip->OpsStk), OPSSTACK_BLKSIZE, sizeof(_Operation), sizeof(int32), offsetof(_OpsArray,data)))
		goto out;
//	(*(ip->OpsStk))->dimSize = OPSSTACK_BLKSIZE;	// no need, ce_CreateOrResizeAlignedArray sets DimSize
	ip->OpsStkDat = &((*(ip->OpsStk))->data[0]);	// >>> #CE_3.0.2 2026-06-20
	ip->OpsStkLen = OPSSTACK_BLKSIZE;				// >>> #CE_3.0.2 2026-06-20
out:
	ip->OpsNum = 0;
	return err;
}

// ================
MgErr OpsStack_DestroyStack(_CE_Instance *ip)
{
	MgErr	err = noErr;

	if (ip->OpsStk)
		err = DSDisposeHandle(ip->OpsStk);
	ip->OpsStkDat = NULL;		// >>> #CE_3.0.2 2026-06-20
	ip->OpsStkLen = 0;			// >>> #CE_3.0.2 2026-06-20	
	ip->OpsStk = NULL;
	ip->OpsNum = 0;
	return	err;
}

// ================
//__inline void OpsStack_Init(register _CE_Instance *ip)
__forceinline void OpsStack_Init(_CE_Instance *ip)					// >>> #CE_3.0.2 2026-06-09
{
	ip->OpsNum = 0;
}

// ================
//__inline int32 OpsStack_GetLen(register _CE_Instance *ip)
__forceinline int32 OpsStack_GetLen(_CE_Instance *ip)				// >>> #CE_3.0.2 2026-06-09
{
	return	ip->OpsNum;
}

// ================
//__inline int32 OpsStack_DecLen(register _CE_Instance *ip)
__forceinline int32 OpsStack_DecLen(_CE_Instance *ip)				// >>> #CE_3.0.2 2026-06-09
{
	if (ip->OpsNum > 0)
		ip->OpsNum -= 1;
	return	ip->OpsNum;
}

// ================
//__inline _Operation * OpsStack_TopPnt(register _CE_Instance *ip)
__forceinline _Operation * OpsStack_TopPnt(_CE_Instance *ip)		// >>> #CE_3.0.2 2026-06-09
{
	if (ip->OpsNum <= 0)
		return NULL;
//	return	&((*(ip->OpsStk))->data[ip->OpsNum]);
	return	&(ip->OpsStkDat[ip->OpsNum]);			// >>> #CE_3.0.2 2026-06-20	
}

// ================
//__inline _Operation * OpsStack_GetPnt(register _CE_Instance *ip, int32 idx)
__forceinline _Operation * OpsStack_GetPnt(_CE_Instance *ip, int32 idx)		// >>> #CE_3.0.2 2026-06-09
{
	if ((idx <= 0) || (idx > ip->OpsNum))
		return NULL;
//	return	&((*(ip->OpsStk))->data[idx]);
	return	&(ip->OpsStkDat[idx]);					// >>> #CE_3.0.2 2026-06-20
}


// ================
__forceinline	MgErr	OpsStack_ResizeStack(_CE_Instance *ip, int32 newsize)
{
	MgErr	err;

	if (newsize == OpsStack_Size(ip))
		return noErr;

//	DbgPrintf("OpsStack_ResizeStack (%s): %d\n", newsize < OpsStack_Size(ip) ? "compress" : " expand ", newsize);

	// >>> #CE_3.0.4 2026-07-05
 	if ( err = ce_CreateOrResizeAlignedArray((UHandle*)&(ip->OpsStk), newsize, sizeof(_Operation), sizeof(int32), offsetof(_OpsArray,data)))
		goto out;	
//	(*(ip->OpsStk))->dimSize = newsize;				// no need, ce_1DArrayResize() sets this value
	ip->OpsStkDat = &((*(ip->OpsStk))->data[0]);	// >>> #CE_3.0.2 2026-06-20
	ip->OpsStkLen = newsize;						// >>> #CE_3.0.2 2026-06-20
out:
	return	err;
}

// ================
__forceinline	MgErr	OpsStack_Push(_CE_Instance *ip, _Operation *elp)
{
	MgErr	err;

	ip->OpsNum += 1;

	// Если таблица заполнена, то расширить
	if (ip->OpsNum  >= OpsStack_Size(ip)) {
		if (err = OpsStack_ResizeStack(ip, OpsStack_Size(ip) + OPSSTACK_BLKSIZE))
			return err;
	}
	// заполнить элемент таблицы
//	(*(ip->OpsStk))->data[ip->OpsNum] = *elp;
	ip->OpsStkDat[ip->OpsNum] = *elp;		// >>> #CE_3.0.2 2026-06-20	

	return	noErr;
}

