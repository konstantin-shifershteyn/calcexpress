/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef __SetIndexedValue_C__
#define __SetIndexedValue_C__

#include "..\DataConversion\OperandInfo.c"
#include "..\DataConversion\NumConverts.c"
#include "..\DataConversion\StrConverts.c"

#include "Subr\PrepareResult1D.c"
#include "Subr\PrepareResult2D.c"

#include "Subr\GetIndexBaseRoutines.c"
#include "Subr\SetIndexBaseRoutines.c"
#include "Subr\SetIndex1D.c"
#include "Subr\SetIndex2D.c"


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// >>> #CE_3.0.5 2025-07-15
MgErr	SetIndexedValue_A1DBL_Idx(
					_CE_Instance	*ip,
					_OperandBase	*arr,
					_OperandBase	*val,
					_indexes_info	*idxs)
{
	int32	*pSize;
	float64	*arrPnt;
	int32	arrSize;
	int32	idx;

	idx		= idxs->idx1.scalar;
	pSize	= (int32*)LStrBuf(*(arr->str));
	arrSize	= *pSize++;
	arrPnt	= (float64*)pSize;
//if (((size_t)arrPnt % 8) != 0) DbgPrintf(__FUNCTION__ "(): array is not aligned\n");

	if ((idx >= 0) && (idx < arrSize))
		arrPnt[idx] = val->num;
	return noErr;
}
// <<< #CE_3.0.5 2025-07-15
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// this function does not set writerID for a variable, it must be done in a caller function
MgErr	SetIndexedValue(
					_CE_Instance	*ip,
					_OperandBase	*arr,
					_OperandBase	*val,
					_indexes_info	*idxs,
					LVBoolean		*err_f )
{
	_dbl_operand	aNop, vNop;
	_str_operand	aSop, vSop;
	Bool32			itComplex;
	int32			in_nDims;
	uInt32			in_elType;
	const char		*errmsg = NULL;
	MgErr			err = noErr;

//#define _TRACE_ME_
#ifdef	_TRACE_ME_
DbgPrintf(__FUNCTION__"(): enter\n");
if (!arr) {	*err_f=LVTRUE; err=CE_InternalError(ip, "arr == NULL",NULL,NULL);	return err;	}
if (!val) {	*err_f=LVTRUE; err=CE_InternalError(ip, "val == NULL",NULL,NULL);	return err;	}
if (!idxs) {	*err_f=LVTRUE; err=CE_InternalError(ip, "idxs == NULL",NULL,NULL);	return err;	}
#undef	_TRACE_ME_
#endif

// >>> #CE_3.0.5 2025-07-15
	if ((arr->type == OPTYPE_A1DBL) && (val->type == OPTYPE_DBL) && (idxs->qty == 1) && (idxs->idx1.nDims == 0)) {
		return SetIndexedValue_A1DBL_Idx(ip, arr, val, idxs);
	}
// <<< #CE_3.0.5 2025-07-15

	in_nDims	= _operand_get_nDims(arr->type);
	in_elType	= _operand_get_BaseType(arr->type);
	aNop.alloc	= vNop.alloc = aSop.alloc = vSop.alloc = FALSE;
	aNop.erMsg	= vNop.erMsg = aSop.erMsg = vSop.erMsg = NULL;

	if (in_nDims <= 0) {
		errmsg = "Indexing of non array";
		goto out;
	}
	if (in_nDims < idxs->qty) {
		errmsg = "Number of indices exceeds the array dimensionality";
		goto out;
	}
	if (in_elType != _operand_get_BaseType(val->type)) {
		errmsg = "Incompatible operand data types in assignment";
		goto out;
	}
	// совместимость размерностей проверяется в функциях _set_idxEl_xd_arr()

	switch (in_elType) {
		case OPTYPE_DBL :
		case OPTYPE_CDB :
			if (err = _prepare_operand(arr, &aNop, &itComplex))	goto out;
			if (err = _prepare_operand(val, &vNop, &itComplex))	goto out;

			if (aNop.nDims == 1)
					_set_idxEl_1d_arr((_gen_operand*)&aNop, idxs, (_gen_operand*)&vNop);
			else	_set_idxEl_2d_arr((_gen_operand*)&aNop, idxs, (_gen_operand*)&vNop);
			errmsg = aNop.erMsg;
			break;

		case OPTYPE_STR:
			if (err = _prepare_strings(arr, &aSop))	goto out;
			if (err = _prepare_strings(val, &vSop))	goto out;

			if (aSop.nDims == 1)
					_set_idxEl_1d_arr((_gen_operand*)&aSop, idxs, (_gen_operand*)&vSop);
			else	_set_idxEl_2d_arr((_gen_operand*)&aSop, idxs, (_gen_operand*)&vSop);
			errmsg = aSop.erMsg;

			err = _save_strings(&aSop, &arr->str);
			break;
		default:
			errmsg = "Unsupported datatype of indexed object";
			break;
	}
out:
//DbgPrintf(__FUNCTION__"(): out\n");
	if (aNop.alloc)		DSDisposePtr(aNop.array);
	if (vNop.alloc)		DSDisposePtr(vNop.array);
	if (aSop.alloc)		DSDisposePtr(aSop.array);
	if (vSop.alloc)		DSDisposePtr(vSop.array);

	if (err)	return	err;

	if (errmsg) {
		*err_f	= LVTRUE;
		err = CE_Error(ip, "%s", (LStrHandle)errmsg, NULL);
	}
//DbgPrintf(__FUNCTION__"(): exit\n");
	return	err;
}

#endif
