/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =============================

MgErr	vmCmd_VarMan_Init( LVBoolean *err_f, uInt16 *err_c)
{
	MgErr	err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;	return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	if (err = ContextTable_Init())
		goto out;
	err = AliasHashTable_DestroyAll();	// на всякий случай

out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return err;
}

// =============================

MgErr	vmCmd_Context_Open(	
				int32		*handle,
				LStrHandle	Creator,
				int32		*lifetime,
				LVBoolean	*err_f,
				uInt16		*err_c)
{
	int32	idx;
	MgErr	err = noErr;

	*handle	= 0;	// !!!

	if (*err_f) {
		*err_c	= VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	// Fins a free element in the Context Table
	// ==========================================
	if ( (idx = ctbl_find_free()) < 0 )  {
		if ((idx = ctbl_create()) < 0) {	// extend the table if no free elements
			*err_f = LVTRUE;
			*err_c = VM_ERR_GENERIC_ERROR;
			if (idx != -1)
				err = mFullErr;
			goto out;
		}
	}
	*handle = ctbl_new_handle(idx);				// calc the VC handle
	err = ctbl_fill_el(idx, *handle, Creator, *lifetime);	// fill in the context
out:
	VARMAN_UNLOCK_EXCLUSIVE();
	
	return err;
}

// =============================

MgErr	vmCmd_Context_Close( int32 *handle, LVBoolean *err_f, uInt16 *err_c)
{
	int32	idx;
	MgErr	err = noErr;

	// Закрываем вне зависимости от состояния входной ошибки,
	// но состояние *err_f не меняем !!!

	if (*err_f)	*err_c = VM_ERR_INPUTERROR;
	else		*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	if ( (idx = ctbl_find_index(*handle)) < 0) {	// найти контекст
		*err_f = LVTRUE;			// не найден
		*err_c = VM_ERR_CTX_INVHANDLE;
	}
	else {
		err = ctbl_free_el(idx);		// освободить
	}

	VARMAN_UNLOCK_EXCLUSIVE();
	
	return err;
}

// =============================

MgErr	vmCmd_Context_Test( int32 *handle, LVBoolean *err_f, uInt16 *err_c)
{
	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;	return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_SHARED();

	if ( ctbl_find_index(*handle) < 0) {	// найти контекст
		*err_f = LVTRUE;					// не найден
		*err_c = VM_ERR_CTX_INVHANDLE;
	}

	VARMAN_UNLOCK_SHARED();

	return noErr;
}

// =============================

MgErr	vmCmd_Context_GetInfo(	
				int32		*handle,
				LStrHandle	Creator,
				LStrHandle	vcName,
				int32		*lifetime,
				LVBoolean	*err_f,
				uInt16		*err_c)
{
	int32	idx;
	MgErr	err = noErr;

	*lifetime = 0;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_SHARED();

	if (*handle == -1) {
		idx = CTBL_GBLIDX;				// Global Context
	}
	else {
		if ( (idx = ctbl_find_index(*handle)) < 0) {	// find
			*err_f = LVTRUE;				// not found
			*err_c = VM_ERR_CTX_INVHANDLE;
			goto out;
		}
	}
	// ***	Context's life time value
	*lifetime = (*ctbl)->data[idx].LifeTime;

	// ***	Context's creator name
	if (err = LStr_Copy(Creator, (*ctbl)->data[idx].Creator))
		goto out;

	// ***	Context's name
	if (err = LStr_Copy(vcName , (*ctbl)->data[idx].vcName))
		goto out;
out:
	if (err) {
		LStr_Clear(Creator);	// size = 0
		LStr_Clear(vcName);		// size = 0
	}
	VARMAN_UNLOCK_SHARED();

	return err;
}
