/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr	vmCmd_Var_Destroy(
				int32		vch,
				LVBoolean	*glob,
				LStrHandle	name,
				LVBoolean	*err_f,
				uInt16		*err_c)
{
	_NameAliasInfo	nai;
	_AliasRecord	*ap;
	int32			idx;
	_HashTableRec	*hktbl;
	MgErr			err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	if ( _ParseNameAlias(name, &nai, TRUE, err_c))	{	// >>> #CE_3.0.2 2026-06-14
		*err_f = LVTRUE;
		goto out;	// err_f и err_c уже установлены
	}

	if (!nai.flgName) {
		// ===============================
		// ***	Поиск переменной по алиасу
		// ===============================
		if ( !(ap = Alias_Find(NULL, nai.pntAlias, nai.lenAlias)) ) {
			*err_f	= LVTRUE;
			*err_c	= VM_ERR_ALIAS_NOTFOUND;
			goto out;
		}
		name = ap->var->name;
		idx  = ap->vcIndex;
	}
	else {
		// Определить контекст
		// ===================
		if (*glob)	idx = CTBL_GBLIDX;
		else		idx = ctbl_find_index(vch);
		if (idx < 0) {
			*err_f = LVTRUE; *err_c = VM_ERR_CTX_INVHANDLE;
			goto out;
		}
	}

	// Pointer to hash-table of the context
//	hktbl = ((*ctbl)->data[idx]).hktbl;
	hktbl = &(((*ctbl)->data[idx]).hktbl[0]);	// >>> #CE_3.1.0 2026-08-18

	// уничтожить эл-т списка
	// ======================
	err = Var_Destroy(hktbl, name);

out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return err;
}
