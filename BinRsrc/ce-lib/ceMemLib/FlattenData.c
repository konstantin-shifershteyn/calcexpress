/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <extcode.h>

#include <celib\FlattenData.h>

// ==========================================
void _flat_data(uInt32 type, LStrHandle data, LVBoolean *err_f)
{
	switch(type) {
		case OPTYPE_STR :
		case OPTYPE_A1STR :
		case OPTYPE_A2STR :	__make_flattened_StrArray(type, data, err_f);
							break;
		default :			__make_flattened_NumArray(type, data, err_f);
							break;
	}
}

// ==========================================
void _unflat_data(uInt32 type, LStrHandle data, LVBoolean *err_f)
{
	switch(type) {
		case OPTYPE_STR :
		case OPTYPE_A1STR :
		case OPTYPE_A2STR :	__make_unflattened_StrArray(type, data, err_f);
							break;
		default :			__make_unflattened_NumArray(type, data, err_f);
							break;
	}
}


// ===================================================================================
//	 онвертирует числовые данные из LV Flattened data формата в i386 формат
// ===================================================================================
void __make_unflattened_NumArray(	uInt32		type,
									LStrHandle	data,
									LVBoolean	*err_f)
{
	char	*str;
	int32	nDims, dbls, num;
	int64	nBytes;

	if (*err_f) return;

	str  = LStrBuf(*data);

	switch(type) {
		case OPTYPE_A1DBL :	nDims = 1; dbls = 1; break;
		case OPTYPE_A2DBL :	nDims = 2; dbls = 1; break;
		case OPTYPE_CDB :	nDims = 0; dbls = 2; break;
		case OPTYPE_A1CDB :	nDims = 1; dbls = 2; break;
		case OPTYPE_A2CDB :	nDims = 2; dbls = 2; break;
		default : return;
	}

/*
	switch(nDims) {
		case 2:	dbls *= _get_flattened_i32(str);  str = _reverse_i32(str);
		case 1:	dbls *= _get_flattened_i32(str);  str = _reverse_i32(str);
		case 0:	break;
	}
	if ( (uInt32)LStrLen(*data) < (uInt32)(nDims * sizeof(int32) + dbls * sizeof(float64)) )
		*err_f = LVTRUE;
	else	reverse_dbl( str, dbls);
	return;
*/
// >>> #CE_3.0 2025-04-21
	switch(nDims) {
		case 2:	num = _get_flattened_i32(str);  str = _reverse_i32(str);
				if (num < 0) goto Error;
				dbls *= num;
		case 1:	num = _get_flattened_i32(str);  str = _reverse_i32(str);
				if (num < 0) goto Error;
				dbls *= num;
		case 0:	break;
	}
	nBytes = (int64)nDims * sizeof(int32) + (int64)dbls * sizeof(float64);
	if ((nBytes < 0) || (nBytes > (int64)LStrLen(*data)))
		goto Error;
	reverse_dbl( str, dbls);
	return;
Error:
	*err_f = LVTRUE;
	return;
// <<< #CE_3.0 2025-04-21
}

// ===================================================================================
//	 онвертирует числовые данные из i386 формата в LV Flattened data формат
// ===================================================================================
void __make_flattened_NumArray(	uInt32		type,
								LStrHandle	data,
								LVBoolean	*err_f)
{
	char	*str;
	int32	nDims, dbls, num;
	int64	nBytes;

	if (*err_f) return;

	str  = LStrBuf(*data);

	switch(type) {
		case OPTYPE_A1DBL :	nDims = 1; dbls = 1; break;
		case OPTYPE_A2DBL :	nDims = 2; dbls = 1; break;
		case OPTYPE_CDB :	nDims = 0; dbls = 2; break;
		case OPTYPE_A1CDB :	nDims = 1; dbls = 2; break;
		case OPTYPE_A2CDB :	nDims = 2; dbls = 2; break;
		default : return;
	}
/*
	switch(nDims) {
		case 2:	dbls *= _get_unflattened_i32(str);  str = _reverse_i32(str);
		case 1:	dbls *= _get_unflattened_i32(str);  str = _reverse_i32(str);
		case 0:	break;
	}
	if ( (uInt32)LStrLen(*data) < (uInt32)(nDims * sizeof(int32) + dbls * sizeof(float64)) )
		*err_f = LVTRUE;
	else	reverse_dbl( str, dbls);
*/
	switch(nDims) {
		case 2:	num = _get_unflattened_i32(str);  str = _reverse_i32(str);
				if (num < 0) goto Error;
				dbls *= num;
		case 1:	num = _get_unflattened_i32(str);  str = _reverse_i32(str);
				if (num < 0) goto Error;
				dbls *= num;
		case 0:	break;
	}
	nBytes = (int64)nDims * sizeof(int32) + (int64)dbls * sizeof(float64);
	if ((nBytes < 0) || (nBytes > (int64)LStrLen(*data)))
		goto Error;
	reverse_dbl( str, dbls);
	return;
Error:
	*err_f = LVTRUE;
	return;
}


// ===================================================================================
//	 онвертирует строковые массивы из LV Flattened data формата в i386 формат
// ===================================================================================
void __make_unflattened_StrArray(	uInt32		type,
									LStrHandle	data,
									LVBoolean	*err_f)
{
	int32	size, size1, size2;
	int32	i, len;
	int32	dataLen, byteCnt;
	char	*str;

	if (*err_f) return;

	str	= LStrBuf(*data);
	dataLen	= LStrLen(*data);

	switch(type) {
		case OPTYPE_A1STR :
					byteCnt = sizeof(int32);
					if (dataLen < byteCnt)
						goto Error;
					size1 = _get_flattened_i32(str); str = _reverse_i32(str);
					size2 = 1;
					if (size1 < 0)	goto Error;		// >>> #CE_3.0 2025-04-21
					break;
		case OPTYPE_A2STR :
					byteCnt = 2 * sizeof(int32);
					if (dataLen < byteCnt )
						goto Error;
					size1 = _get_flattened_i32(str); str = _reverse_i32(str);
					size2 = _get_flattened_i32(str); str = _reverse_i32(str);
					if (size1 < 0)	goto Error;		// >>> #CE_3.0 2025-04-21
					if (size2 < 0)	goto Error;		// >>> #CE_3.0 2025-04-21
					break;
		default : return;
	}

	size = size1 * size2;			// общее кол-во строк в массиве

	for (i=0; i < size; i++) {
		if ((byteCnt += sizeof(int32)) > dataLen)
			goto Error;
		len = _get_flattened_i32(str);		// длина строки
//		if ((byteCnt += len) > dataLen)
//			goto Error;
// >>> #CE_3.0 2025-04-21
		byteCnt += len;
		if ((len < 0) || (len > dataLen))
			goto Error;
		if ((byteCnt < 0) || (byteCnt > dataLen))
			goto Error;
// <<< #CE_3.0 2025-04-21
		str = _reverse_i32(str);			// конвертировать длину
		str += len;							// пропустить данные строки
	}
	return;
Error:
	*err_f = LVTRUE;
	return;
}


// =====================================================================================
//	 онвертирует строковые массивы из внутреннего формата в LV Flattened data формат
// =====================================================================================
void __make_flattened_StrArray(	uInt32		type,
								LStrHandle	data,
								LVBoolean	*err_f)
{
	int32	size, size1, size2;
	int32	i, len;
	int32	dataLen, byteCnt;
	char	*str;

	if (*err_f) return;

	str	= LStrBuf(*data);
	dataLen	= LStrLen(*data);

	switch(type) {
		case OPTYPE_A1STR :
					byteCnt = sizeof(int32);
					if (dataLen < byteCnt)
						goto Error;
					size1 = _get_unflattened_i32(str); str = _reverse_i32(str);
					size2 = 1;
					if (size1 < 0)	goto Error;		// >>> #CE_3.0 2025-04-21
					break;
		case OPTYPE_A2STR :
					byteCnt = 2 * sizeof(int32);
					if (dataLen < byteCnt )
						goto Error;
					size1 = _get_unflattened_i32(str); str = _reverse_i32(str);
					size2 = _get_unflattened_i32(str); str = _reverse_i32(str);
					if (size1 < 0)	goto Error;		// >>> #CE_3.0 2025-04-21
					if (size2 < 0)	goto Error;		// >>> #CE_3.0 2025-04-21
					break;
		default : return;
	}

	size = size1 * size2;			// общее кол-во строк в массиве
	for (i=0; i < size; i++) {
		if ((byteCnt += sizeof(int32)) > dataLen)
			goto Error;
		len = _get_unflattened_i32(str);	// длина строки
//		if ((byteCnt += len) > dataLen)
//			goto Error;
// >>> #CE_3.0 2025-04-21
		byteCnt += len;
		if ((len < 0) || (len > dataLen))
			goto Error;
		if ((byteCnt < 0) || (byteCnt > dataLen))
			goto Error;
// <<< #CE_3.0 2025-04-21
		str = _reverse_i32(str);	// конвертировать длину
		str += len;					// пропустить данные строки
	}
	return;
Error:
	*err_f = LVTRUE;
	return;
}
