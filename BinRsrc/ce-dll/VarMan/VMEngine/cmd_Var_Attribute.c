/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ========================================================
// Read Variable Attribute command
// ========================================================
MgErr	vmCmd_Var_GetAttribute(
				int32		vch,
				LVBoolean	*glob,
				LStrHandle	name,
				LStrHandle	attribute,
				float64		*nval,
				uInt32		*type,
				uInt32		*writerID,		// >>> #CE_3.0.7 2026-07-26
				LVBoolean	*err_f,
				uInt16		*err_c)
{
	_VarRecord	*vp;
	 MgErr 		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_SHARED();		// >>> #CE_3.0.2 2026-06-08
	//	EnterCriticalSection( &ctblLock );

	// *** найти переменную
	if ( !(vp = __GetVarPointer(vch, glob, name, NULL, err_c)) ) {
		*err_f	= LVTRUE;
		goto out;
	}
	if (type)		*type		= vp->inf.type;
	if (nval)		*nval		= vp->inf.num;
	if (writerID)	*writerID	= vp->writerID;		// >>> #CE_3.0.7 2026-07-26

	if (vp->attrib)	err = LStr_Copy(attribute, vp->attrib);
	else			err = LStr_Clear(attribute);		// if attribute is not created

out:
	VARMAN_UNLOCK_SHARED();		// >>> #CE_3.0.2 2026-06-08
	//	LeaveCriticalSection( &ctblLock );
	return err;
}

// ========================================================
// Write Variable Attribute command
// ========================================================
MgErr	vmCmd_Var_SetAttribute(
				int32		vch,
				LVBoolean	*glob,
				LStrHandle	name,
				LStrHandle	attribute,
				float64		*nval,
				uInt32		*type,
				LVBoolean	*err_f,
				uInt16		*err_c)
{
	_VarRecord	*vp;
	MgErr 		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	// *** найти переменную
	if ( !(vp = __GetVarPointer(vch, glob, name, NULL, err_c)) ) {
		*err_f	= LVTRUE;
		goto out;
	}
	if (type)	*type	= vp->inf.type;
	if (nval)	*nval	= vp->inf.num;

	err = LStr_CopyOrCreate(&(vp->attrib), attribute);

out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return err;
}
