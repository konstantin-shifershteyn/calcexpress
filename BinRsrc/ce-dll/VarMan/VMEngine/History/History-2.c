/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <VHistory.h>

// ================================================================================
MgErr	_VHistory_Preset(
				_VarRecord	*vep,
				uInt32		type,		// 'data' type
				_hLVA1Dbl	time,		// array of timestamps
				_hLVA1Poly	data,		// array of history values
				_hLVA1U32	wrid,		// array of writer IDs		[#CE_3.1.0 2026-08-20]
				LVBoolean	append,
				LVBoolean	*err_f,
				uInt16		*err_c
			)
{
	_pVHistory	hist;
	_pVHistData	hBuf;
	_VarInfo	vinf;
	int32		i, size;
// >>> #CE_3.1.0 2026-08-20
	Bool32		_wrid_arr;			// TRUE it we use array of writer IDs; otherwise, a scalar value
	uInt32		_wrid_val;			// scalar value of writer ID
// <<< #CE_3.1.0 2026-08-20

	MgErr	err = noErr;

	// ===============================================
	// Test if Variable has history
	// ===============================================
	if (!(hist = vep->hist)) {		//no history
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_NOHISTORY;
		goto out;
	}
	if (!(hBuf = hist->buf))
		goto out;	//stub

	// ===============================================
	// Test Variable Type
	// ===============================================
	if (hist->vType == type) {
		// *** if a variable has an expected type ***
		switch (hist->vType) {
			case VM_TYPE_DBL :	break;
			case VM_TYPE_CDB :	break;
			case VM_TYPE_STR :	break;
			default:		goto InvalidType;
		}
	}
	else {	// *** a variable type differs from an expected type ***
InvalidType:	*err_f = LVTRUE;
		*err_c = VM_ERR_VAR_INVTYPE;
		goto out;
	}

	// ===============================================
	// Reset History Content
	// ===============================================
	if (!append) {
		if (err = _VHistory_Reset(vep))
			goto out;
	}

	// ===============================================
	// Copy data to history
	// ===============================================
	vinf.type = type;
	vinf.num  = 0.0;
	vinf.str  = NULL;

	size = (*data)->size;

// >>> #CE_3.1.0 2026-08-20
	_wrid_arr	= ((*wrid)->size >= size);	// TRUE it we use array of writer IDs; otherwise, a scalar value
	_wrid_val	= ((*wrid)->size > 0) ? (*wrid)->val[0] : 0;	// scalar value of writer ID
// <<< #CE_3.1.0 2026-08-20

	for (i=0; i < size; i++) {
		switch(type) {
			case VM_TYPE_DBL :
				vinf.num = (*(_hLVA1Dbl)data)->val[i];
				break;
			case VM_TYPE_CDB :
			case VM_TYPE_STR :
//				if ( err = LStr_CopyOrCreate(&(vinf.str), (*(_hLVA1Str)data)->val[i]) )
				if ( err = ce_CopyOrCreate_OperandString(&(vinf.str), (*(_hLVA1Str)data)->val[i], type) )	// >>> #CE_3.0.4 2026-07-06
					goto out;
				break;
		}
		if (err = __hist_save(hist, &vinf, FALSE, MillisecsTimer(), (*(time))->val[i], _wrid_arr ? (*wrid)->val[i] : _wrid_val ) )	// >>> #CE_3.1.0 2026-08-20
			goto out;
	}
out:
	return err;
}
