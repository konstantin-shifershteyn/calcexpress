/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

// =================================
// "Create History" command executor
// =================================
MgErr vmCmd_History_Create(	int32			vch,
							LStrHandle		name,
							_VHistParams	*histPars,
							Bool32			enaWriterIDs,
							LVBoolean		*err_f,
							uInt16			*err_c)
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
	glob = (vch == -1) ? LVTRUE : LVFALSE;
	if ( !(vp = __GetVarPointer(vch, &glob, name, NULL, err_c))) {
		*err_f	= LVTRUE;
		goto out;			// err_f и err_c уже установлены
	}
	err = _VHistory_Create(vp, histPars, enaWriterIDs, err_f, err_c);
out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return	err;
}

// ==================================
// "Destroy History" command executor
// ==================================
MgErr vmCmd_History_Destroy(	int32		vch,
								LStrHandle	name,
								LVBoolean	*err_f,
								uInt16		*err_c)
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
	glob = (vch == -1) ? LVTRUE : LVFALSE;
	if ( !(vp = __GetVarPointer(vch, &glob, name, NULL, err_c))) {
		*err_f	= LVTRUE;
		goto out;			// err_f и err_c уже установлены
	}
	if (!(vp->hist)) {
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_NOHISTORY;
		goto out;
	}
	err = _VHistory_Destroy(vp);
out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return	err;
}

/*
// ========================================
// "Save Value to History" command executor
// ========================================
MgErr vmCmd_History_Save(	int32		vch,
							LStrHandle	name,
							LVBoolean	*err_f,
							uInt16		*err_c )
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
	glob = (vch == -1) ? LVTRUE : LVFALSE;
	if ( !(vp = __GetVarPointer(vch, &glob, name, NULL, err_c))) {
		*err_f	= LVTRUE;
		goto out;			// err_f и err_c уже установлены
	}
	if (!(vp->hist)) {
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_NOHISTORY;
		goto out;
	}
	err = _VHistory_AddCurValue(vp);
out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return	err;
}
*/

// ========================================
// "Access History" command executor
// ========================================
MgErr vmCmd_History_Access(	int32			vch,
							LStrHandle		name,
							LVBoolean		*hasWrID_out,	// >>> #CE_3.1.0 2026-08-23
							VHistAcc_func	extFunc,
							void			*extData,
							LVBoolean		*err_f,
							uInt16			*err_c	)
{
	_VarRecord	*vp;
	LVBoolean	glob;
	MgErr		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_SHARED();
	//VARMAN_LOCK_EXCLUSIVE();

	// ------------------------------
	// *** search for the variable
	// ------------------------------
	glob = (vch == -1) ? LVTRUE : LVFALSE;
	if ( !(vp = __GetVarPointer(vch, &glob, name, NULL, err_c))) {
		*err_f	= LVTRUE;
		goto out;			// err_f и err_c уже установлены
	}
	if (!(vp->hist)) {
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_NOHISTORY;
		goto out;
	}
	if (hasWrID_out) {
		*hasWrID_out = (vp->hist->writerIDs != NULL ? LVTRUE : LVFALSE);		// >>> #CE_3.1.0 2026-08-23
	}
	err = extFunc(vp->hist, extData, err_f, err_c);
out:
	VARMAN_UNLOCK_SHARED();
	//VARMAN_UNLOCK_EXCLUSIVE();

	return	err;
}


// ================================
// "Reset History" command executor
// ================================
MgErr vmCmd_History_Reset(	int32		vch,
							LStrHandle	name,
							LVBoolean	*err_f,
							uInt16		*err_c)
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
	glob = (vch == -1) ? LVTRUE : LVFALSE;
	if ( !(vp = __GetVarPointer(vch, &glob, name, NULL, err_c))) {
		*err_f	= LVTRUE;
		goto out;			// err_f и err_c уже установлены
	}
	if (!(vp->hist)) {
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_NOHISTORY;
		goto out;
	}
	err = _VHistory_Reset(vp);
out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return	err;
}

// ====================================
// "Configure History" command executor
// ====================================
MgErr vmCmd_History_Config(	int32			vch,
							LStrHandle		name,
							_VHistParams	*hpars,
							LVBoolean		*err_f,
							uInt16			*err_c)
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
	glob = (vch == -1) ? LVTRUE : LVFALSE;
	if ( !(vp = __GetVarPointer(vch, &glob, name, NULL, err_c))) {
		*err_f	= LVTRUE;
		goto out;			// err_f и err_c уже установлены
	}
	if (!(vp->hist)) {
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_NOHISTORY;
		goto out;
	}
	err = _VHistory_Config(vp, hpars);
out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return	err;
}
