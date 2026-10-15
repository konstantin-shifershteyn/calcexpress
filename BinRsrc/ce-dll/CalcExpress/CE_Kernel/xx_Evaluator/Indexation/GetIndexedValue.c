/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#ifndef __GetIndexedValue_C__
#define __GetIndexedValue_C__

#include "..\DataConversion\OperandInfo.c"
#include "..\DataConversion\NumConverts.c"
#include "..\DataConversion\StrConverts.c"

#include "Subr\PrepareResult1D.c"
#include "Subr\PrepareResult2D.c"

#include "Subr\GetIndexBaseRoutines.c"
#include "Subr\GetIndex1D.c"
#include "Subr\GetIndex2D.c"

// ---------------------------------------------

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// >>> #CE_3.0.5 2025-07-15
MgErr	GetIndexedValue_A1DBL_Idx(
					_CE_Instance	*ip,
					_OperandBase	*in,
					_OperandBase	*out,
					_indexes_info	*idxs)
{
	int32	*pSize;
	float64	*arrPnt;
	int32	arrSize;
	int32	idx;

	idx		= idxs->idx1.scalar;
	pSize	= (int32*)LStrBuf(*(in->str));
	arrSize	= *pSize++;
	arrPnt	= (float64*)pSize;
//if (((size_t)arrPnt % 8) != 0) DbgPrintf(__FUNCTION__ "(): array is not aligned\n");

	out->type = OPTYPE_DBL;
	if ((idx >= 0) && (idx < arrSize))
			out->num = arrPnt[idx];
	else	out->num = 0.0;
	return noErr;
}
// <<< #CE_3.0.5 2025-07-15
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// >>> #CE_3.0 2025-04-22
// if 'out->str' is NULL, string data are not copied
// <<< #CE_3.0 2025-04-22
MgErr	GetIndexedValue(
					_CE_Instance	*ip,
					_OperandBase	*in,
					_OperandBase	*out,
					_indexes_info	*idxs,
					LVBoolean		*err_f)
{
	_dbl_operand	nop;	// = {0};			// zero initialized
	_str_operand	sop;	// = {0};			// zero initialized
	Bool32			itComplex;
	int32			in_nDims;
	const char		*errmsg = NULL;
	MgErr			err = noErr;

//#define _TRACE_ME_
#ifdef	_TRACE_ME_
if (!in) {	*err_f=LVTRUE; err=CE_InternalError(ip, "in == NULL",NULL,NULL);	return err;	}
if (!out) {	*err_f=LVTRUE; err=CE_InternalError(ip, "out == NULL",NULL,NULL);	return err;	}
if (!idxs) {	*err_f=LVTRUE; err=CE_InternalError(ip, "idxs == NULL",NULL,NULL);	return err;	}
#undef	_TRACE_ME_
#endif

// >>> #CE_3.0.5 2025-07-15
	if ((in->type == OPTYPE_A1DBL) && (idxs->qty == 1) && (idxs->idx1.nDims == 0)) {
		return GetIndexedValue_A1DBL_Idx(ip, in, out, idxs);
	}
// <<< #CE_3.0.5 2025-07-15

	in_nDims = _operand_get_nDims(in->type);
	nop.alloc = sop.alloc = FALSE;
	nop.erMsg = sop.erMsg = NULL;

	if (in_nDims <= 0) {
		errmsg = "Indexing of non-array";
		goto out;
	}
	if (in_nDims < idxs->qty) {
		errmsg = "Number of indices exceeds the array dimensionality";
		goto out;
	}

	switch (_operand_get_BaseType(in->type)) {
		case OPTYPE_DBL :
		case OPTYPE_CDB :
			if (_prepare_operand(in, &nop, &itComplex)) {
				errmsg = "Internal Error: _prepare_operand() returned an error in "__FUNCTION__"()";
				goto out;
			}
			if (nop.nDims==1)	err = _index_1d_arr((_gen_operand*)&nop, idxs, out->str == NULL);	// >>> #CE_3.0 2025-04-22
			else				err = _index_2d_arr((_gen_operand*)&nop, idxs, out->str == NULL);	// <<< #CE_3.0 2025-04-22
			if ((errmsg = nop.erMsg) || err)
				goto out;
			out->num  = nop.scalar;
			if ((out->type = nop.ResType) != OPTYPE_DBL)
				if (out->str)			// >>> #CE_3.0 2025-04-22
					err = _save_array(&nop, &out->str);
			break;

		case OPTYPE_STR:
			err = _prepare_strings(in, &sop);
			if (err)	goto out;
			if (sop.nDims==1)	err = _index_1d_arr((_gen_operand*)&sop, idxs, out->str == NULL);	// >>> #CE_3.0 2025-04-22
			else				err = _index_2d_arr((_gen_operand*)&sop, idxs, out->str == NULL);	// <<< #CE_3.0 2025-04-22
			if ((errmsg = sop.erMsg) || err)
				goto out;
			out->type = sop.ResType;
			out->num  = 0;
			if (out->str)			// >>> #CE_3.0 2025-04-22
				err = _save_strings(&sop, &out->str);
			break;

		default:
			errmsg = "Unsupported datatype of indexed object";
			break;
	}
out:
	if (nop.alloc)		DSDisposePtr(nop.array);
	if (sop.alloc)		DSDisposePtr(sop.array);
	if (errmsg) {
		*err_f	= LVTRUE;
		err = CE_Error(ip, "%s", (LStrHandle)errmsg, NULL);
	}
	return	err;
}

#endif
