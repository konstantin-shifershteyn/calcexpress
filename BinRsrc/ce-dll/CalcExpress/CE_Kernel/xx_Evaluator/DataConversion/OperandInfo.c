/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__OperandInfo_C__
#define	__OperandInfo_C__

//////////////////////////////
int32	__fastcall	_operand_get_nDims(uInt32 type)
{
	switch(type) {
		case	OPTYPE_DBL:
		case	OPTYPE_CDB:
		case	OPTYPE_STR:		return	0;

		case	OPTYPE_A1DBL:
		case	OPTYPE_A1CDB:
		case	OPTYPE_A1STR:	return	1;

		case	OPTYPE_A2DBL:
		case	OPTYPE_A2CDB:
		case	OPTYPE_A2STR:	return	2;
	}
	return -1;
}

//------------
uInt32	__fastcall	_operand_get_BaseType(uInt32 type)
{
	switch(type) {
		case	OPTYPE_DBL:
		case	OPTYPE_A1DBL:
		case	OPTYPE_A2DBL:	return	OPTYPE_DBL;

		case	OPTYPE_CDB:
		case	OPTYPE_A1CDB:
		case	OPTYPE_A2CDB:	return	OPTYPE_CDB;

		case	OPTYPE_STR:
		case	OPTYPE_A1STR:
		case	OPTYPE_A2STR:	return	OPTYPE_STR;
	}
	return	OPTYPE_UNKNOWN;
}

//------------
uInt32	__fastcall	_operand_get_TargetType(uInt32 baseType, int32 nDims)
{
	switch(baseType) {
		case	OPTYPE_DBL:
		case	OPTYPE_A1DBL:
		case	OPTYPE_A2DBL:
				switch(nDims) {
					case 0 : return	OPTYPE_DBL;
					case 1 : return	OPTYPE_A1DBL;
					case 2 : return	OPTYPE_A2DBL;
					break;
				}
				break;
		case	OPTYPE_CDB:
		case	OPTYPE_A1CDB:
		case	OPTYPE_A2CDB:
				switch(nDims) {
					case 0 : return	OPTYPE_CDB;
					case 1 : return	OPTYPE_A1CDB;
					case 2 : return	OPTYPE_A2CDB;
					break;
				}
				break;
		case	OPTYPE_STR:
		case	OPTYPE_A1STR:
		case	OPTYPE_A2STR:
				switch(nDims) {
					case 0 : return	OPTYPE_STR;
					case 1 : return	OPTYPE_A1STR;
					case 2 : return	OPTYPE_A2STR;
					break;
				}
				break;
	}
	return	OPTYPE_UNKNOWN;
}

// =============================================================

void	__fastcall	_general_operand_info( _OperandBase *arg, _gen_operand *data )
{
	int32	*pnt;

	data->ResType	= arg->type;		// Тип операнда

	switch (arg->type) {

		case OPTYPE_DBL :
					pnt			= (int32*)&(data->scalar);
					data->nDims	= 0;
					data->size1	= data->size2 = 1;
					data->scalar= arg->num;
					break;

		case OPTYPE_CDB :
					pnt			= (int32*)LStrBuf(*(arg->str));		// >>> #CE_3.0.5 2025-07-14
					data->nDims	= 0;
					data->size1	= data->size2 = 1;
					break;

		case OPTYPE_STR :
					pnt			= (int32*)&(LStrLen(*(arg->str)));	// !!! pointer to LStr
					data->nDims	= 0;
					data->size1	= data->size2 = 1;
					break;

		case OPTYPE_A1STR :
		case OPTYPE_A1CDB :
		case OPTYPE_A1DBL :	
					pnt			= (int32*)LStrBuf(*(arg->str));		// >>> #CE_3.0.5 2025-07-14
					data->nDims	= 1;
					data->size1	= *pnt++;
					data->size2	= 1;
//#if defined(_M_X64)
//					pnt++;
//#endif
					break;

		case OPTYPE_A2STR :
		case OPTYPE_A2CDB :
		case OPTYPE_A2DBL :	
					pnt			= (int32*)LStrBuf(*(arg->str));		// >>> #CE_3.0.5 2025-07-14
					data->nDims	= 2;
					data->size1	= *pnt++;
					data->size2	= *pnt++;
					break;

		default :	data->nDims	= 0;
					data->size1	= data->size2 = 1;
					pnt			= (int32*)&(LStrLen(*(arg->str)));	// !!! pointer to LStr
					break;
	}
	data->array = pnt;
}

#endif
