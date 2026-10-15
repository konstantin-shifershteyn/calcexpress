/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr vmCmd_MiscFieldAccess(
				int32				*vch,
				VM_MiscAccessPars	*par,
				LVBoolean			*err_f,
				uInt16				*err_c )
{
	int32		idx;
	int32		*pMisc;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	if (par->cmd == VM_CMD_MISCACC_GET)
			VARMAN_LOCK_SHARED();
	else	VARMAN_LOCK_EXCLUSIVE();

	// Поиск контекста
	// ===============
	if ( (idx = ctbl_find_index(*vch)) < 0) {
		*err_f	= LVTRUE;
		*err_c	= VM_ERR_CTX_INVHANDLE;
		goto out;
	}
	// Проверка индекса
	// ================
	if ((par->idx < 0) || (par->idx >= CNTX_MISC_FIELDS)) {
		*err_f	= LVTRUE;
		*err_c	= VM_ERR_INVALID_PARAMS;
		goto out;
	}
	pMisc  = &(((*ctbl)->data[idx]).misc[par->idx]);	// pointer to misc field

	// Выполнить
	// =========
	switch(par->cmd) {
		case VM_CMD_MISCACC_GET :
			par->old	= *pMisc;
			par->val	= *pMisc;
			break;
		case VM_CMD_MISCACC_SET :
			par->old	= *pMisc;
			*pMisc		= par->val;
			break;
	}
out:
	if (par->cmd == VM_CMD_MISCACC_GET)
			VARMAN_UNLOCK_SHARED();
	else	VARMAN_UNLOCK_EXCLUSIVE();

	return noErr;
}
