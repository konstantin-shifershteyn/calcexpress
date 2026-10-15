/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <celib\ceMemLib.h>


// =========================

Bool32 LStr_iCmp( LStrHandle s1H, LStrHandle s2H )
{
	if (LStrLen(*s1H) != LStrLen(*s2H))
		return FALSE;
	return !_memicmp(LStrBuf(*s1H), LStrBuf(*s2H), LStrLen(*s2H));
}

// =========================

MgErr	LStr_Copy( LStrHandle dstH, LStrHandle srcH )
{
	MgErr	err;

	if ( !(err = LStr_SetSize(dstH, LStrLen(*srcH))) )
		_MoveBlock( LStrBuf(*srcH), LStrBuf(*dstH), LStrLen(*srcH));
	return err;
}

// =========================

MgErr	LStr_CopyOrCreate( LStrHandle *dstHP, LStrHandle srcH )
{
	MgErr	err;

	if ( !(err = LStr_SetSizeOrCreate(dstHP, LStrLen(*srcH))) )
		_MoveBlock( LStrBuf(*srcH), LStrBuf(**dstHP), LStrLen(*srcH));
	return err;
}

// =========================

MgErr LStr_Cat(LStrHandle dstH, LStrHandle srcH)
{
	int32	dstLen, srcLen;
	MgErr	err;

	dstLen = LStrLen(*dstH);
	srcLen = LStrLen(*srcH);

	if ( !(err = LStr_Resize( dstH, dstLen + srcLen)) )
		_MoveBlock( LStrBuf(*srcH), LStrBuf(*dstH)+dstLen, srcLen);
	return err;
}

// ============================

MgErr	LStr_Create( LStrHandle *strHP, int32 len )
{
//DbgPrintf(__FUNCTION__"(): create string\n");
	if ( !(*strHP = (LStrHandle)DSNewHandle( sizeof(int32) + len) ))
		return mFullErr;
	LStrLen( **strHP ) = len;
	return noErr;
}

// ============================

MgErr	LStr_CreateFromMem( LStrHandle *strHP, char *srcPnt, int32 srcLen )
{
	if (!srcPnt)
		return noErr;	//?

	if (srcLen < 0)
		srcLen = (int32)strlen(srcPnt);		// >>> 2024-12-06 : CHECK 64 BIT

//DbgPrintf(__FUNCTION__"(): create string\n");
	if ( !(*strHP = (LStrHandle)DSNewHandle( sizeof(int32) + srcLen) ))
		return mFullErr;
	LStrLen( **strHP ) = srcLen;

	if (srcPnt && srcLen)
		_MoveBlock(srcPnt, LStrBuf(**strHP), srcLen);
	return noErr;
}

// =========================
// this function preserves existing data
MgErr	LStr_Resize (LStrHandle strH, int32 newLen)
{
	MgErr	err = noErr;
	if (LStrLen(*strH) != newLen) {
//DbgPrintf(__FUNCTION__"(): resize string\n");
		if ( !(err = NumericArrayResize(uB, 1L, (UHandle*)&strH, newLen)) )
			LStrLen(*strH) = newLen;
	}
	return err;
}
// =========================
// this function preserves existing data
MgErr	LStr_ResizeOrCreate(LStrHandle *strHP, int32 newLen)
{
	MgErr	err;
	if (*strHP)
		if (LStrLen(**strHP) == newLen)
			return noErr;

//DbgPrintf(__FUNCTION__"(): create/resize string\n");
	if ( !(err = NumericArrayResize(uB, 1L, (UHandle*)strHP, newLen)) )
		LStrLen(**strHP) = newLen;
	return err;
}
// =========================
// this function does not preserve existing data
MgErr	LStr_SetSize(LStrHandle strH, int32 newLen)
{
	MgErr	err = noErr;
	if (LStrLen(*strH) != newLen) {
		LStrLen(*strH) = 0;	// for LV7
//DbgPrintf(__FUNCTION__"(): resize string\n");
		if ( !(err = NumericArrayResize(uB, 1L, (UHandle*)&strH, newLen)) )
			LStrLen(*strH) = newLen;
	}
	return err;
}

// =========================
// this function does not preserve existing data
MgErr LStr_SetSizeOrCreate( LStrHandle *strHP, int32 newLen)
{
	MgErr	err;
	if (*strHP) {
		if (LStrLen(**strHP) == newLen)
			return noErr;
		LStrLen(**strHP) = 0;	// for LV7
	}
//DbgPrintf(__FUNCTION__"(): create/resize string\n");
	if ( !(err = NumericArrayResize(uB, 1L, (UHandle*)strHP, newLen)) )
		LStrLen(**strHP) = newLen;
	return err;
}
