/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <extcode.h>

#include <CE_DataTypes.h>
#include <celib\FlattenData.h>

#define	__OK__	FALSE
#define	__ER__	TRUE

Bool32	Test_Data_Flattened_To_LVString(uInt32		type,
					LStrHandle	data,
					LVBoolean	*err_f)
{
	char	*datPnt;
	uInt32	datLen, byteCnt, len;
	uInt32	HeaderSize, dimSize1, dimSize2, Counter;
	uInt32	nDims, dbls;

	if (*err_f) return __ER__;

	datPnt  = LStrBuf(*data);
	datLen	= LStrLen(*data);

	switch(type) {
		case OPTYPE_IDXRANGE :	nDims = 0; dbls = 1; break;
		case OPTYPE_DBL   :	nDims = 0; dbls = 1; break;
		case OPTYPE_A1DBL :	nDims = 1; dbls = 1; break;
		case OPTYPE_A2DBL :	nDims = 2; dbls = 1; break;
		case OPTYPE_CDB   :	nDims = 0; dbls = 2; break;
		case OPTYPE_A1CDB :	nDims = 1; dbls = 2; break;
		case OPTYPE_A2CDB :	nDims = 2; dbls = 2; break;

		case OPTYPE_A1STR :	nDims = 1; dbls = 0; break;
		case OPTYPE_A2STR :	nDims = 2; dbls = 0; break;

		case OPTYPE_STR   :
		default 	  : return __OK__;
	}

	HeaderSize = nDims * sizeof(int32);

	// ***	Test for dimensions
	// ------------------------
	if (datLen < HeaderSize)
		goto Error;

	// ***	Determine dimension size(s)
	// --------------------------------
	switch(nDims) {
		case 2:	dimSize1 = _get_flattened_i32(datPnt);
			dimSize2 = _get_flattened_i32(datPnt + sizeof(int32));
			break;
		case 1:	dimSize1 = _get_flattened_i32(datPnt);
			dimSize2 = 1;
			break;
		case 0:	dimSize1 = dimSize2 = 1;
			break;
	}

	// ***	Test for numeric data
	// --------------------------
	if (dbls) {
		if ( datLen < (HeaderSize + dimSize1 * dimSize2 * dbls * sizeof(float64)) )
			goto Error;
		goto out;
	}

	// ***	Test for string arrays
	// ---------------------------
	Counter	= dimSize1 * dimSize2;		// total number of strings in an array
	byteCnt	= HeaderSize;
	datPnt += HeaderSize;
	for ( ; Counter; Counter--) {

		if ((byteCnt += sizeof(int32)) > datLen)	// 4 bytes for string length
			goto Error;

		len = _get_flattened_i32(datPnt);		// get string length

		if ((byteCnt += len) > datLen)			// add string length
			goto Error;

		datPnt += sizeof(int32) + len;			// skip current string
	}
out:
	return	__OK__;
Error:
	*err_f	= LVTRUE;
	return	__ER__;
}
