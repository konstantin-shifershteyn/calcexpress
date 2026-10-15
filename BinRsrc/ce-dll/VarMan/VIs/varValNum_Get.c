/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

MgErr vmExtFunc_getval(	
			LStrHandle	name,		// name of variable
			_VarInfo	*vp_int,	//_VarInfo inside VarMan
			_VarInfo	*vp_ext,	//_VarInfo outside VarMan
			void		*extdata,	// external data
			LVBoolean	*err_f,
			uInt16		*err_c)
{
	vmcin_extdata	*vpar;
	int32			nDims, nDbls, lvtc;
	int32			size, size1, size2;
	int32			*vmpnt, *vipnt;
	char			*sbuf;
	uInt32			slen;
	MgErr			err;

	vpar = (vmcin_extdata*)extdata;

	if (*err_f)	return noErr;

	if (vp_int->type != vpar->type)	//поверка соответствия типов
		goto invalid_type;

	switch (vpar->type) {
		case OPTYPE_A1DBL :	nDims = 1; nDbls = 1; lvtc = fD; break;
		case OPTYPE_A2DBL :	nDims = 2; nDbls = 1; lvtc = fD; break;
		case OPTYPE_CDB   :	nDims = 0; nDbls = 2; lvtc = cD; break;
		case OPTYPE_A1CDB :	nDims = 1; nDbls = 2; lvtc = cD; break;
		case OPTYPE_A2CDB :	nDims = 2; nDbls = 2; lvtc = cD; break;
		default : goto invalid_type;
	}

	sbuf = LStrBuf(*(vp_int->str));
	slen = LStrLen(*(vp_int->str));

	if (slen < (nDims * sizeof(int32)))
		goto bad_format;

	vmpnt = (int32*)sbuf;
	switch (nDims) {
		case 2 :	size1 = *vmpnt++; size2 = *vmpnt++;
					break;
		case 1 :	size1 = *vmpnt++; size2 = 1;
// >>> #CE_3.0 2024-12-13
//#if defined(_M_X64)
//					vmpnt++;	// skip padding bytes
//#endif
// <<< #CE_3.0 2024-12-13
					break;
		case 0 :	size1 = size2 = 1;
					break;
	}
	size = size1 * size2;


	if (slen < (nDims * sizeof(int32) + size * sizeof(float64) * nDbls) )
		goto bad_format;

	if (nDims) {
		if (err= ce_NumericArraySetSize(lvtc, nDims, &(vpar->val), size) )
			return err;
		vipnt = (int32*)(*vpar->val);
		*vipnt++ = size1;
		if (nDims > 1) *vipnt++ = size2;
// >>> #CE_3.0 2024-12-13
#if defined(_M_X64)
		if (nDims == 1) vipnt++;	// skip padding bytes
#endif
// <<< #CE_3.0 2024-12-13
	}
	else {
		vipnt = (int32*)vpar->val;
	}
	_MoveBlock(vmpnt, vipnt, size * nDbls * sizeof(float64));

	return noErr;

bad_format:
	*err_f = LVTRUE;
	*err_c = VM_ERR_BADDATA;
	return noErr;

invalid_type:
	*err_f = LVTRUE;
	*err_c = VM_ERR_VAR_INVTYPE;
	return noErr;
}
