/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ========================================================
// Расширенная функция определения значения переменной
// ========================================================
// >>> #CE_3.0 2025-04-22
// if 'par->str' is NULL, string data are not copied
// <<< #CE_3.0 2025-04-22
MgErr	vmCmd_Var_GetValueEx(
				int32				vch,
				LVBoolean			*glob,
				LStrHandle			name,
				_VarInfo			*par,
				//uInt32			*writerID,
				_VM_VI_WriteStats	*wrStats,		// >>> #CE_3.1.0-r6 2026-09-18
				_VM_EXT_FUNC		*extfunc,	//extension function
				void				*extdata,	//extension data
				LVBoolean			*err_f,
				uInt16				*err_c )
{
	_VarRecord *vp;
	 MgErr 		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_SHARED();		// >>> #CE_3.0.2 2026-06-08
	//	EnterCriticalSection( &ctblLock );

	// *** найти переменную
// >>> #CE_3.0.2 2026-06-08
	if ( vp = __GetVarPointer(vch, glob, name, NULL, err_c) ) {
		//*err_f	= LVFALSE;
		if (extfunc)	err = extfunc(name, &(vp->inf), par, extdata, err_f, err_c);
		else			err = Var_Copy( par, &(vp->inf));
		//if (writerID)
		//	*writerID = vp->writerID;		// >>> #CE_3.1.0 2026-08-22
		if (wrStats)
			__ReadVarWritingStats(vp, wrStats);
	}
	else *err_f	= LVTRUE;
// <<< #CE_3.0.2 2026-06-08

	VARMAN_UNLOCK_SHARED();		// >>> #CE_3.0.2 2026-06-08
	//	LeaveCriticalSection( &ctblLock );
	return err;
}

// ========================================================
// Простая функция определения значения переменной
// ========================================================
// >>> #CE_3.0 2025-04-22
// if 'par->str' is NULL, string data are not copied
// <<< #CE_3.0 2025-04-22
MgErr	vmCmd_Var_GetValue(
				int32				vch,
				LVBoolean			*glob,
				LStrHandle			name,
				_VarInfo			*par,
				//uInt32			*writerID,		// >>> #CE_3.1.0 2026-08-22
				_VM_VI_WriteStats	*wrStats,		// >>> #CE_3.1.0-r6 2026-09-18
				LVBoolean			*err_f,
				uInt16				*err_c )
{
	_VarRecord *vp;
	MgErr 		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_SHARED();		// >>> #CE_3.0.2 2026-06-08
	//	EnterCriticalSection( &ctblLock );

	// *** найти переменную и скопировать значение
// >>> #CE_3.0.2 2026-06-08
	if (vp = __GetVarPointer(vch, glob, name, NULL, err_c)) {
		//*err_f	= LVFALSE;
		err = Var_Copy( par, &(vp->inf));
		//if (writerID)
		//	*writerID = vp->writerID;		// >>> #CE_3.1.0 2026-08-22
		if (wrStats)
			__ReadVarWritingStats(vp, wrStats);
	}
	else *err_f	= LVTRUE;
// <<< #CE_3.0.2 2026-06-08
	VARMAN_UNLOCK_SHARED();		// >>> #CE_3.0.2 2026-06-08
	//	LeaveCriticalSection( &ctblLock );
	return err;
}
