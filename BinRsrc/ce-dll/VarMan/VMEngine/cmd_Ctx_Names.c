/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr vmCmd_vcNames_Reset(
				LVBoolean	*err_f,
				uInt16		*err_c)
{
	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	vcName_reset_names();

	VARMAN_UNLOCK_EXCLUSIVE();

	return noErr;
}

// ------------------------------------

MgErr vmCmd_vcNames_Assign(
				int32		*handle,
				LStrHandle	vcName,
				LVBoolean	*err_f,
				uInt16		*err_c)
{
	int32	idx, inm;
	MgErr	err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	// ### search for the Context
	if ( (idx = ctbl_find_index(*handle)) < 0) {
		*err_f = LVTRUE;
		*err_c = VM_ERR_CTX_INVHANDLE;
		goto out;
	}
	if (LStrLen(*vcName)) {
		// ### check name syntax
		if (vcName_test_syntax(vcName)) {
			*err_f = LVTRUE;
			*err_c = VM_ERR_VCNAME_INVNAME;
			goto out;
		}
		// ### check the name for unique
		if ((inm = vcName_find_name(vcName)) >= 0) {
			if (inm != idx) {	// если имя принадлежит другому контексту
				*err_f = LVTRUE;
				*err_c = VM_ERR_VCNAME_NONUNIQUE;
			}
			goto out;
		}
	}
	// ### assign the context name
	err = LStr_Copy((*ctbl)->data[idx].vcName, vcName);

out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return err;
}

// ------------------------------------

MgErr vmCmd_vcNames_GetName(
				int32		*handle,
				LStrHandle	vcName,
				LVBoolean	*err_f,
				uInt16		*err_c)
{
	int32	idx;
	MgErr	err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;	return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_SHARED();

	// Поиск контекста
	// ===============
	if (*handle == -1) {
		idx = CTBL_GBLIDX;				// Global Context
	}
	else {
		if ( (idx = ctbl_find_index(*handle)) < 0) {
			*err_f = LVTRUE;
			*err_c = VM_ERR_CTX_INVHANDLE;
			goto out;
		}
	}
	// Скопировать имя
	err = LStr_Copy(vcName , (*ctbl)->data[idx].vcName);
out:
	VARMAN_UNLOCK_SHARED();

	return err;
}

// ------------------------------------

MgErr vmCmd_vcNames_FindName(
				int32		*handle,
				LStrHandle	vcName,
				LVBoolean	*err_f,
				uInt16		*err_c)
{
	int32	idx;
	MgErr	err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;	return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_SHARED();

	// Проверить синтаксис имени
	// =========================
	if (vcName_test_syntax(vcName))
		goto NotFound;

	// Поиск контекста
	// ===============
	if ((idx = vcName_find_name(vcName)) < 0) {
NotFound:
		*err_f	= LVTRUE;
		*err_c	= VM_ERR_VCNAME_NOTFOUND;
		*handle	= 0;
	}
	else {
		if (idx == CTBL_GBLIDX)	*handle = -1;
		else			*handle = (*ctbl)->data[idx].key;
	}

	VARMAN_UNLOCK_SHARED();

	return err;
}
