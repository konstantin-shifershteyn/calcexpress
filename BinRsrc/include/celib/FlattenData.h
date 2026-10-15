/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__FLATTEN_DATA_H__
#define	__FLATTEN_DATA_H__

#include <extcode.h>
#include <stdlib.h>

#include <CE_DataTypes.h>

#ifdef	__cplusplus
extern "C" {
#endif

extern	Bool32	Test_Data_Flattened_To_LVString( uInt32 type, LStrHandle data, LVBoolean *err_f);

extern	void	_flat_data(	uInt32 type, LStrHandle data, LVBoolean *err_f);
extern	void	_unflat_data(	uInt32 type, LStrHandle data, LVBoolean *err_f);

extern	void	__make_flattened_NumArray(	uInt32 type, LStrHandle data, LVBoolean *err_f);
extern	void	__make_flattened_StrArray(	uInt32 type, LStrHandle data, LVBoolean *err_f);
extern	void	__make_unflattened_NumArray(uInt32 type, LStrHandle data, LVBoolean *err_f);
extern	void	__make_unflattened_StrArray(uInt32 type, LStrHandle data, LVBoolean *err_f);

//extern	int32	_get_flattened_i32( char *src );
//extern	int32	_get_unflattened_i32( char *src );
//extern	char *	_reverse_i32( char *src );
//extern	float64* reverse_dbl( register char *src, int32 size );


// >>> #CE_3.0.2 2026-06-11
// -----------
// returns a 32-bit value stored in the Flattened format
__forceinline	int32 _get_flattened_i32( char *src )
{
	uint32_t rawVal = *(uint32_t*)src;
	return (int32)_byteswap_ulong(rawVal);
}
// -----------
// returns a 32-bit value stored in the Unflattened format (native i386)
__forceinline	int32 _get_unflattened_i32(char *src)
{
	return *((int32*)src);
}
// -----------
// converts a 32-bit value between Little-Endian and Big-Endian formats (Flatten/Unflatten)
__inline	char * _reverse_i32(char *src)
{
	uint32_t *pnt32 = (uint32_t*)src;
	*pnt32 = _byteswap_ulong(*pnt32);
	return src + 4;
}
// -----------
// converts an array of 64-bit values between Little-Endian and Big-Endian formats (Flatten/Unflatten)
__inline	float64 * reverse_dbl(char *src, int32 nVals)
{
	uint64_t *pnt64 = (uint64_t*)src;
	while (nVals-- > 0) {
		*pnt64 = _byteswap_uint64(*pnt64);
		pnt64++;
	}
	return (float64*)pnt64;
}
// <<< #CE_3.0.2 2026-06-11

#ifdef	__cplusplus
extern "C" {
#endif

#endif
