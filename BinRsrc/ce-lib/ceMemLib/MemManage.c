/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <extcode.h>
#include <celib\ceMemLib.h>
#include <CE_DataTypes.h>

// >>> #CE_3.1.0 2026-08-27
float64 ce_getLabviewMemoryUse(uInt32 units)
{
	MemStatRec	msr;

	DSMemStats(&msr);

	if (units > 2)
		units = 2;
	switch(units) {
		case 1 :	return ((float64)msr.totAllocSize)/(1024.0);			// in KBybet
		case 2 :	return ((float64)msr.totAllocSize)/(1024.0 * 1024.0);	// in MBytes
	}
	return (float64)msr.totAllocSize;	// in Bytes
}
// <<< #CE_3.1.0 2026-08-27

/*
void DebugCheckHandle(UHandle *h)
{
	static	Bool32	started = FALSE;
	if (!started) {
		started = TRUE;
		DbgPrintf("### Start tracking handles\n");
	}
	if (((uInt32)h) & 0x3) {	DbgPrintf("Not aligned &UHandle\n");	}
	if (((uInt32)(*h)) & 0x3) {	DbgPrintf("Not aligned UHandle\n");	}
	if (((uInt32)(**h)) & 0x3) {DbgPrintf("Not aligned UHandle*\n");	}
}
*/
// ====================

int32	ce_DisposeHandles(UHandle *arrHP, int32 size)
{
	int32	num = 0;

	for( ; size > 0; size--, arrHP++) {
		if (!DSCheckHandle(*arrHP)) {
			DSDisposeHandle(*arrHP);	num++;
		}
		*arrHP = NULL;	// mark it as unallocated
	}
	return num;
}

// ====================

int32	ce_DisposePointers(UPtr *arrPP, int32 size)
{
	int32	num = 0;

	for( ; size > 0; size--, arrPP++) {
		if (!DSCheckPtr(*arrPP)) {
			DSDisposePtr(*arrPP);	num++;
		}
		*arrPP = NULL;	// mark is as unallocated
	}
	return num;
}

// ====================

MgErr	ce_DSCopyHandle(UHandle *dstHP, UHandle srcH)
{
//	int32	size;
	size_t	size;	// >>> 2024-12-06 : CHECK 64 BIT

	size = DSGetHandleSize(srcH);	// >>> 2024-12-06 : CHECK 64 BIT

	if (*dstHP) {
		MgErr	err;
		if (err = DSSetHandleSize(*dstHP, size))
			return err;
	}
	else {
		if ( !(*dstHP = DSNewHandle(size)) )
			return mFullErr;
	}
	_MoveBlock(*srcH, **dstHP, size);
	return noErr;
}

// ====================

MgErr	ce_NumericArraySetSize(int32 typeCode, int32 numDims, UHandle *hdlPnt, int32 totalNewSize)
{
	int32	i, *dimSizeP;
	MgErr	err;

	if (*hdlPnt) {
		for(i = numDims, dimSizeP = (int32*)(**hdlPnt); i; i--)
			*dimSizeP++ = 0;
	}
// >>> #CE_3.0 2024-12-14
//#if defined(_M_X64)
//	if ((numDims & 1) != 0)	totalNewSize += sizeof(int32);	// add padding if numDims is odd
//#endif
// <<< #CE_3.0 2024-12-14
	err = NumericArrayResize(typeCode, numDims, hdlPnt, totalNewSize);
//DebugCheckHandle(hdlPnt);
	return err;
}

// ====================

MgErr	ce_1DArrayResize( UHandle *hdlPnt, int32 newSize, int32 itemSize )
{
	int32	oldSize = 0;
	int32	nPaddingBytes = 0;
	MgErr	err;

	if (*hdlPnt) {				// If allocated handle

		oldSize = *((int32*)(**hdlPnt));	// number of elements

		if (oldSize == newSize)			// return if new size equals old size
			return noErr;

		// Set dimSize field to actual number of bytes
		// In LabVIEW 7.0, NumericArrayResize function uses this field
		// to calculate the number of bytes to be copied
		//
		*((int32*)(**hdlPnt)) = oldSize * itemSize;
	}

// >>> #CE_3.0 2024-12-14
#if defined(_M_X64)
	// FIXME : padding in header can be unexpected if array element is a cluster where all members has size <= 4 bytes
	if (itemSize > 4)	nPaddingBytes = sizeof(int32);
#endif
// <<< #CE_3.0 2024-12-14
	if (err = NumericArrayResize(uB, 1, hdlPnt, nPaddingBytes + (newSize * itemSize))) {
		// If an error ocurred
		if (*hdlPnt && !DSCheckHandle(*hdlPnt)) {
			// if the handle still valid, restore the old number of elements
			 *((int32*)(**hdlPnt)) = oldSize;
		}
	}
	else {
		// Set dimSize field to the number of elements in the array
		*((int32*)(**hdlPnt)) = newSize;
//DebugCheckHandle(hdlPnt);
	}
	return err;
}

// ====================

MgErr	ce_1DArraySetSize( UHandle *hdlPnt, int32 newSize, int32 itemSize )
{
	int32	oldSize = 0;
	int32	nPaddingBytes = 0;
	MgErr	err;

	if (*hdlPnt) {				// If allocated handle

		oldSize = *((int32*)(**hdlPnt));	// number of elements

		if (oldSize == newSize)			// return if new size equals old size
			return noErr;

		// Set dimSize field to 0 to suppress copying data in LabVIEW 7
		// In LabVIEW 7.0, NumericArrayResize function uses this field
		// to calculate the number of bytes to be copied
		//
		*((int32*)(**hdlPnt)) = 0;
	}

// >>> #CE_3.0 2024-12-14
#if defined(_M_X64)
	// FIXME : padding in header can be unexpected if array element is a cluster where all members have size <= 4 bytes
	if (itemSize > 4)	nPaddingBytes = sizeof(int32);
#endif
// <<< #CE_3.0 2024-12-14
	if (err = NumericArrayResize(uB, 1, hdlPnt, nPaddingBytes + (newSize * itemSize))) {
		// If an error ocurred
		if (*hdlPnt && !DSCheckHandle(*hdlPnt)) {
			// if the handle still valid, restore the old number of elements
			 *((int32*)(**hdlPnt)) = oldSize;
		}
	}
	else {
		// Set dimSize field to the number of elements in the array
		*((int32*)(**hdlPnt)) = newSize;
//DebugCheckHandle(hdlPnt);
	}
	return err;
}

// ====================
// >>> #CE_3.0.4 2026-07-05
MgErr	ce_CreateOrResizeAlignedArray(UHandle *hdlPnt, int32 arraySize, int32 itemSize, size_t allign, size_t dataOffset)
{
	size_t	totalSize = dataOffset + arraySize * itemSize;
	MgErr	err;

	if (hdlPnt == NULL)
		return mZoneErr;
	
	if (*hdlPnt == NULL) {
		// ### create new handle
		*hdlPnt = DSNewAlignedHClr(totalSize, allign, dataOffset);
		if (*hdlPnt != NULL) {
			*((int32*)(**hdlPnt)) = arraySize;
			return noErr;
		}
		return mFullErr;
	}

	// ### resize handle
	err = DSSetAlignedHSzClr(*hdlPnt, totalSize, allign, dataOffset);
	if (err == noErr) {
		// Set dimSize field to the number of elements in the array
		*((int32*)(**hdlPnt)) = arraySize;
	}
	return err;
}
// <<< #CE_3.0.4 2026-07-05

// =========================
// >>> #CE_3.0.4 2026-07-06
// Copy LVString with enclosed numeric array
MgErr	ce_CopyOrCreate_LStrWithNumArray( LStrHandle *phDstStr, LStrHandle hSrcStr, int32 nDims )
{
	size_t	dataOffset	= sizeof(int32) + nDims * sizeof(int32);	// string length AND nDims array dimensions
	MgErr	err = noErr;

	if (phDstStr == NULL)
		return mZoneErr;

	if (*phDstStr == NULL) {
		// ### create new string
//DbgPrintf(__FUNCTION__"(): Create\n");
		*phDstStr = (LStrHandle)DSNewAlignedHandle(sizeof(int32) + LStrLen(*hSrcStr), sizeof(float64), dataOffset);
		if (*phDstStr == NULL) {
			err = mFullErr;
		}
	}
	else {
		Bool32	aligned = ( (((size_t)(&LStrLen(**phDstStr)) + dataOffset) % sizeof(float64)) == 0 );
		Bool32  samelen = ( LStrLen(**phDstStr) == LStrLen(*hSrcStr) );
		if ( !aligned || !samelen ) {
//DbgPrintf(__FUNCTION__"(): Realloc: %s %s\n", aligned ? "" : "not_aligned", samelen ? "" : "new_len");
			err = DSSetAlignedHandleSize(*phDstStr, sizeof(int32) + LStrLen(*hSrcStr), sizeof(float64), dataOffset);
		}
//else	DbgPrintf(__FUNCTION__"(): Reuse buffer\n");
	}
	if (err == noErr) {
		LStrLen(**phDstStr) = LStrLen(*hSrcStr);			// set size
		_MoveBlock( LStrBuf(*hSrcStr), LStrBuf(**phDstStr), LStrLen(*hSrcStr));	// copy data
	}
	return err;
}
// =============================
MgErr	ce_CopyOrCreate_OperandString( LStrHandle *phDstStr, LStrHandle hSrcStr, uInt32 dataType)
{
	int32	nDims = 0;
	switch (dataType) 	{
		default :				return LStr_CopyOrCreate(phDstStr, hSrcStr);
		case OPTYPE_CDB :		nDims = 0;	break;
		case OPTYPE_A1DBL :
		case OPTYPE_A1CDB :		nDims = 1;	break;
		case OPTYPE_A2DBL :
		case OPTYPE_A2CDB :		nDims = 2;	break;

	}
	return ce_CopyOrCreate_LStrWithNumArray(phDstStr, hSrcStr, nDims);
}
// <<< #CE_3.0.4 2026-07-06
