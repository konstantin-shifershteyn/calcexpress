/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef __ceMemLib_H__
#define __ceMemLib_H__

#include <extcode.h>
#include <memory.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// >>> #CE_3.0.2 2026-06-06
// ### LV function replacents
#define	_MoveBlock(src, dst, bytes)		memmove((dst),(src),(bytes))
#define _ClearMem(pnt, elSize)			memset((pnt),0,(elSize))
// <<< #CE_3.0.2 2026-06-06

// =============================================
//	LV String Routines
// =============================================

//extern	Bool32	LStr_Cmp	(LStrHandle s1H,  LStrHandle s2H);

__inline Bool32 LStr_Cmp( LStrHandle s1H, LStrHandle s2H) {
	if (LStrLen(*s1H) != LStrLen(*s2H))
		return FALSE;
	// return !BlockCmp(LStrBuf(*s1H), LStrBuf(*s2H), LStrLen(*s2H));
	return !memcmp(LStrBuf(*s1H), LStrBuf(*s2H), LStrLen(*s2H));		// >>> #CE_3.0.2 2026-06-06
}
// >>> #CE_3.0.2 2026-06-19
__inline Bool32 LStr_CmpWithMem( LStrHandle s1H, char *pnt, int32 len) {
	if (LStrLen(*s1H) != len)
		return FALSE;
	return !memcmp(LStrBuf(*s1H), pnt, len);
}
// <<< #CE_3.0.2 2026-06-19

extern	Bool32	LStr_iCmp			(LStrHandle s1H,	LStrHandle s2H );

extern	MgErr	LStr_Cat			(LStrHandle dstH,	LStrHandle srcH);

extern	MgErr	LStr_Copy			(LStrHandle  dstH,	LStrHandle srcH);
extern	MgErr	LStr_CopyOrCreate	(LStrHandle *dstH,	LStrHandle srcH);

extern	MgErr	LStr_SetSize		(LStrHandle  strH,	int32 newLen);
extern	MgErr	LStr_SetSizeOrCreate(LStrHandle *strHP,	int32 newLen);

extern	MgErr	LStr_Resize			(LStrHandle  strH,	int32 newLen);
extern	MgErr	LStr_ResizeOrCreate	(LStrHandle *strHP,	int32 newLen);

extern	MgErr	LStr_Create			(LStrHandle *strHP,	int32 len );
extern	MgErr	LStr_CreateFromMem	(LStrHandle *strHP,	char *srcPnt, int32 srcLen );

#define		LStr_Clear(s)		LStr_SetSize((s), 0)
#define		LStr_Destroy(s)		DSDisposeHandle((UHandle)(s))



// =============================================
//	Array Routines
// =============================================

// >>> #CE_3.0 2024-12-16 - declined
//#define	ce_NumericArrayResize(t, nd, hp, s)	NumericArrayResize((t),(nd),(hp),(s))

extern	MgErr	ce_NumericArraySetSize(int32 typeCode, int32 numDims, UHandle *hdlPnt, int32 totalNewSize);

extern	MgErr	ce_1DArrayResize ( UHandle *hdlPnt, int32 newSize, int32 itemSize );
extern	MgErr	ce_1DArraySetSize( UHandle *hdlPnt, int32 newSize, int32 itemSize );

// >>> #CE_3.0.4 2026-07-05
extern	MgErr	ce_CreateOrResizeAlignedArray(UHandle *hdlPnt, int32 arraySize, int32 itemSize, size_t allign, size_t dataOffset);
// <<< #CE_3.0.4 2026-07-05
// >>> #CE_3.0.4 2026-07-06
extern	MgErr	ce_CopyOrCreate_LStrWithNumArray( LStrHandle *phDstStr, LStrHandle hSrcStr, int32 nDims );
extern	MgErr	ce_CopyOrCreate_OperandString( LStrHandle *phDstStr, LStrHandle hSrcStr, uInt32 dataType);
// <<< #CE_3.0.4 2026-07-06

// =============================================
//	Memory management routines
// =============================================

extern	float64 ce_getLabviewMemoryUse(uInt32 units);

extern	MgErr	ce_DSCopyHandle( UHandle *dstHP, UHandle srcH );

extern	int32	ce_DisposeHandles( UHandle *arrHP, int32 size);
extern	int32	ce_DisposePointers(UPtr    *arrPP, int32 size);

__inline void *	ce_DSNewPtr (int32 size) { if (size < 0) return NULL; return DSNewPtr(size); }
__inline void *	ce_DSNewPClr(int32 size) { if (size < 0) return NULL; return DSNewPClr(size); }


// =============================================
//	Memory Blocks manipulation routines
// =============================================

__inline void *	_CopyBlock(void *src, void *dst, int32 numBytes) {
	_MoveBlock(src, dst, numBytes);
	return ((char*)(dst) + numBytes);
}

extern	void	ReverseBlock(void *pnt, int32 elNums, int32 elSize);
extern	MgErr	Transpose2D (void *pnt, int32 size1,  int32 size2,  int32 elSize);
extern	void	ShiftBlock  (void *pnt, int32 elNums, int32 elSize, int32 n);
extern	MgErr	RotateBlock (void *pnt, int32 elNums, int32 elSize, int32 n);


// >>> #CE_3.1.0-r6 2026-09-28
#define __IsKeyword(kwd,pnt,len)		(Bool32)(sizeof(kwd)-1 == (len) ? !strncmp(kwd, (const char*)(pnt), (len)) : FALSE)
//  ---
__forceinline Bool32 _IsKeyword(const char *kwd, const char *pnt, int32 len)
{
	if (len == strlen(kwd))
		return (memcmp(kwd, pnt, len));
	return FALSE;
}
// <<< #CE_3.1.0-r6 2026-09-28


#ifdef __cplusplus
}
#endif

#endif
