/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <VHistory.h>

// ========================================
// "Preset History" command executor
// ========================================
MgErr vmCmd_History_Preset(	
				int32		vch,
				LStrHandle	name,
				uInt32		type,
				_hLVA1Dbl	time,		// array of timestamps
				_hLVA1Poly	data,		// array of history values
				_hLVA1U32	wrid,		// array of writer IDs		[#CE_3.1.0 2026-08-20]
				LVBoolean	append,
				LVBoolean	*err_f,
				uInt16		*err_c
			)
{
	_VarRecord	*vp;
	LVBoolean	glob;
	MgErr		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	// ------------------------------
	// *** search for the variable
	// ------------------------------
	glob = (vch == -1);
	if ( !(vp = __GetVarPointer(vch, &glob, name, NULL, err_c))) {
		*err_f	= LVTRUE;
		goto out;			// err_f и err_c уже установлены
	}
	if (!(vp->hist)) {
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_NOHISTORY;
		goto out;
	}
	err = _VHistory_Preset(vp, type, time, data, wrid, append, err_f, err_c);
out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return	err;
}
