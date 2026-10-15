/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__NumConverts_C__
#define	__NumConverts_C__

// ===========================================================
//	prepares numeric operands
// ===========================================================
Bool32 __fastcall	_prepare_operand(_OperandBase *arg, _dbl_operand *data, Bool32 *itComplex)
{
	int32	*pnt;

	data->ResType	= arg->type;		// datatype

	switch (arg->type) {

		case OPTYPE_DBL :
					data->nDims	= 0;
					data->size1	= data->size2 = 1;
					data->scalar= arg->num;
					data->array	= &(data->scalar);
					return FALSE;

		case OPTYPE_CDB :
					*itComplex	= TRUE;
					pnt	= (int32*)LStrBuf(*(arg->str));		// >>> #CE_3.0.5 2025-07-14
					data->nDims	= 0;
					data->size1	= data->size2 = 1;
					break;

		case OPTYPE_A1CDB :
					*itComplex	= TRUE;
		case OPTYPE_A1DBL :
					pnt	= (int32*)LStrBuf(*(arg->str));		// >>> #CE_3.0.5 2025-07-14
					data->nDims	= 1;
					data->size1	= *pnt++;
					data->size2	= 1;
// >>> #CE_3.0 2024-12-13
//#if defined(_M_X64)
//					pnt++;	// skip padding bytes
//#endif
// <<< #CE_3.0 2024-12-13
					break;

		case OPTYPE_A2CDB :
					*itComplex	= TRUE;
		case OPTYPE_A2DBL :
					pnt	= (int32*)LStrBuf(*(arg->str));		// >>> #CE_3.0.5 2025-07-14
					data->nDims	= 2;
					data->size1	= *pnt++;
					data->size2	= *pnt++;
					break;

		default :	return TRUE;
	}
	data->array = (float64*)pnt;
	return FALSE;
}

// ============================================
// Refreshes info on array dimensions from LStrHandle
// returns pointer to array data 
// ============================================
void * __fastcall	_save_dimensions_info(_dbl_operand *data, LStrHandle *sh )
{
	int32	*ipnt;

	ipnt = (int32*)LStrBuf(**sh);

	if (data->nDims > 0)	*ipnt++ = data->size1;
	if (data->nDims > 1)	*ipnt++ = data->size2;
// >>> #CE_3.0 2024-12-13
//#if defined(_M_X64)
//	if (data->nDims == 1)	ipnt++;	// skip padding bytes
//#endif
// <<< #CE_3.0 2024-12-13
	return	ipnt;
}

// ==================================================
//	saves numeric arrays
// ==================================================
MgErr __fastcall	_save_array(_dbl_operand *data, LStrHandle *sh)
{
	int32	size, dbls;
	int32	*ipnt;
	MgErr	err;
	int32	nDims;
	int32	nPaddingBytes = 0;

static	char *	ermsg = " !!! Debugging info (Please inform the developer):\n"
		"Mismatch of nDims and type of result has been detected in:\n"
		"file: "__FILE__" func: _save_array()";

	switch(data->ResType) {
		case OPTYPE_A1DBL :	nDims = 1; dbls = 1; break;
		case OPTYPE_A2DBL :	nDims = 2; dbls = 1; break;
		case OPTYPE_CDB :	nDims = 0; dbls = 2; break;
		case OPTYPE_A1CDB :	nDims = 1; dbls = 2; break;
		case OPTYPE_A2CDB :	nDims = 2; dbls = 2; break;
		default :		return noErr;
	}
	if (nDims != data->nDims) {
		data->nDims = nDims;
		data->erMsg = ermsg;
	}


//#if defined(_M_X64)
//	if (nDims == 1) nPaddingBytes = sizeof(int32);
//#endif

	size = (nPaddingBytes + (data->nDims * sizeof(int32))) + (dbls * (data->size1 * data->size2) * sizeof(float64));

	// Выделить память под строку
	if ( err = LStr_SetSizeOrCreate(sh, size) )
		return err;

	// сохранить размеры массива
	ipnt = _save_dimensions_info(data, sh);

	// Сохраняем данные
	_MoveBlock(data->array, ipnt, dbls * (data->size1 * data->size2) * sizeof(float64));

	return noErr;
}

#endif
