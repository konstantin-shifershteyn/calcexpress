/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ========================================================
// –асширенна€ функци€ установки значени€ переменной
// ========================================================
MgErr	vmCmd_Var_SetValueEx(
				int32				vch,
				LVBoolean			*glob,
				LStrHandle			name,
				_VarInfo			*par,
				uInt32				writerID,		// >>> #CE_3.0.7 2026-07-23
				_VM_VI_WriteStats	*wrStats,		// >>> #CE_3.1.0-r6 2026-09-18
				_VM_EXT_FUNC		*extfunc,	//extension function
				void				*extdata,	//extension data
				LVBoolean			*err_f,
				uInt16				*err_c
				)
{
	 _VarInfo	*vip;
	 _VarRecord	*vp;
	 MgErr 		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	// *** найти переменную
	if ( !(vp = __GetVarPointer(vch, glob, name, NULL, err_c))) {
		*err_f	= LVTRUE;
		goto out;		// err_f и err_c уже установлены
	}

	vip = &(vp->inf);

	if (vip->type != par->type) {	// проверить соответствие типов
		*err_f = LVTRUE;
		*err_c = VM_ERR_VAR_INVTYPE;
		goto out;
	}

	if (extfunc)	err = extfunc(name, vip, par, extdata, err_f, err_c);
	else			err = Var_Copy(vip, par);
	if (err)	goto out;

	vp->writerID = writerID;		// >>> #CE_3.0.7 2026-07-23
	__UpdateVarWritingTimeNow(vp);	// >>> #CE_3.1.0-r6 2026-09-17
	if (vp->hist)
		err = _VHistory_AddCurValue(vp);

	if (wrStats)
		__ReadVarWritingStats(vp, wrStats);
out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return err;
}

// ========================================================
// ѕроста€ функци€ установки значени€ переменной
// ========================================================
MgErr	vmCmd_Var_SetValue(
				int32				vch,
				LVBoolean			*glob,
				LStrHandle			name,
				_VarInfo			*value,
				uInt32				writerID,		// >>> #CE_3.0.7 2026-07-23
				_VM_VI_WriteStats	*wrStats,		// >>> #CE_3.1.0-r6 2026-09-18
				LVBoolean			*err_f,
				uInt16				*err_c
				)
{
	 _VarInfo		*vip;
	 _VarRecord		*vp;
	 MgErr 			err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	// *** найти переменную
	if ( !(vp = __GetVarPointer(vch, glob, name, NULL, err_c))) {
		*err_f	= LVTRUE;
		goto out;		// err_f и err_c уже установлены
	}
	vip = &(vp->inf);

	if (vip->type != value->type) {	// проверить соответствие типов
		*err_f = LVTRUE;
		*err_c = VM_ERR_VAR_INVTYPE;
		goto out;
	}

	if ( err = Var_Copy(vip, value))
		goto out;

	vp->writerID = writerID;			// >>> #CE_3.0.7 2026-07-23
	__UpdateVarWritingTimeNow(vp);		// >>> #CE_3.1.0-r6 2026-09-17
	if (vp->hist)
		err = _VHistory_AddCurValue(vp);

	if (wrStats)
		__ReadVarWritingStats(vp, wrStats);

out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return err;
}


// #################################################################################################
// >>> #CE_3.0.2 2026-06-13

// ========================================================
// Set variable value using the VarInfo pointer
// ========================================================
// this function does not set writerID in a variable, it must be done in a caller function
MgErr	__vmCmd_Var_SetValue_Pnt(
				_VarInfo	*vip,		// pointer to target variable
				_VarInfo	*value,		// pointer to new value is to be set
				uInt32		writerID,
				LVBoolean	*err_f,
				uInt16		*err_c )
{
//	 _VarRecord		*vp = (_VarRecord*)((char*)vip - offsetof(_VarRecord, inf));
	_VarRecord	*vp = __VarInfoPnt_To_VarRecordPnt(vip);	// >>> #CE_3.0.7 2026-07-23
	MgErr 		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	if (vip != value) {					// >>> #CE_3.0.7 2026-07-25 : e.g., it can happen in a such statement: x = x;
		if (vip->type != value->type) {	// проверить соответствие типов
			*err_f = LVTRUE;
			*err_c = VM_ERR_VAR_INVTYPE;
			goto out;
		}
		if ( err = Var_Copy(vip, value))
			goto out;
	}
	vp->writerID = writerID;			// >>> #CE_3.1.0 2026-08-20
	__UpdateVarWritingTimeNow(vp);		// >>> #CE_3.1.0-r6 2026-09-17
	if (vp->hist)
		err = _VHistory_AddCurValue(vp);
out:
	return err;
}

// <<< #CE_3.0.2 2026-06-13
