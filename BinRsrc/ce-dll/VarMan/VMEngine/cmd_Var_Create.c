/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr	__Create_RandomName(uChar **str);

// =====================================================

MgErr	__varCreate_CmdX (
				int32				vch,
				LVBoolean			*glob,
				LStrHandle			name,
				_VarInfo			*par,
				uInt32				writerID,		// >>> #CE_3.0.7 2026-07-23
				_VM_VI_WriteStats	*wrStats,		// >>> #CE_3.1.0-r6 2026-09-18
				_VM_EXT_FUNC		*extfunc,		//extension function
				void				*extdata,		//extension data
				_VarInfo			**vipp,			//a pointer to buffer to save a pointer to _VarInfo
				LVBoolean			*err_f,
				uInt16				*err_c )
{
	uChar			*RndName = NULL;	// !!!
	_NameAliasInfo	nai;
	_VarRecord		*vp;
	int32			idx;
	_HashTableRec	*hktbl;
	MgErr			err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	// ***	определить контекст
	if (*glob)	idx = CTBL_GBLIDX;
	else		idx = ctbl_find_index(vch);
	if (idx < 0) {
		*err_f = LVTRUE;
		*err_c = VM_ERR_CTX_INVHANDLE;
		goto out;
	}
	hktbl = &(((*ctbl)->data[idx]).hktbl[0]);	//	hktbl = ((*ctbl)->data[idx]).hktbl;


	// ***	разложить имя/алиас на составляющие
	if ( _ParseNameAlias(name, &nai, TRUE, err_c))	{	// >>> #CE_3.0.2 2026-06-14
		*err_f = LVTRUE;
		goto out;	// err_f и err_c уже установлены
	}
	// ***	Если имя не указано, то создать уникальное имя переменной
	if (!nai.flgName) {
		if (err = __Create_RandomName(&RndName))
			goto out;
		nai.pntName = RndName;
		nai.lenName = StrLen(RndName);
	}

	// ***	Создать переменную
	err = Var_Create( hktbl, nai.pntName, nai.lenName, par, writerID, extfunc, extdata, &vp, err_f, err_c);
	if (err || *err_f)
		goto out;

	// *** write pointer to _VarInfo
	if (vipp) {
		*vipp = &(vp->inf);
	}
	// ***	создать алиас (if any)
	if (nai.flgAlias) {
		err = Alias_Create( nai.pntAlias, nai.lenAlias, vp, idx, err_f, err_c);
		if (err || *err_f) {
			// Если ошибка создания алиаса, то уничтожить переменную
			Var_Destroy(hktbl, vp->name);
			goto out;
		}
	}

	if (wrStats)
		__ReadVarWritingStats(vp, wrStats);

out:
	if (RndName)
		DSDisposePtr(RndName);

	return err;
}

// ====================================================
MgErr	vmCmd_Var_CreateEx(
				int32				vch,
				LVBoolean			*glob,
				LStrHandle			name,
				_VarInfo			*par,
				uInt32				writerID,		// >>> #CE_3.0.7 2026-07-23
				_VM_VI_WriteStats	*wrStats,		// >>> #CE_3.1.0-r6 2026-09-18
				_VM_EXT_FUNC		*extfunc,		//extension function
				void				*extdata,		//extension data
				LVBoolean			*err_f,
				uInt16				*err_c)
{
	MgErr	err;

	VARMAN_LOCK_EXCLUSIVE();

	err = __varCreate_CmdX(vch, glob, name, par, writerID, wrStats, extfunc, extdata, NULL, err_f, err_c);

	VARMAN_UNLOCK_EXCLUSIVE();

	return err;
}

// ====================================================
MgErr	vmCmd_Var_Create(
				int32				vch,
				LVBoolean			*glob,
				LStrHandle			name,
				_VarInfo			*par,
				uInt32				writerID,		// >>> #CE_3.0.7 2026-07-23
				_VM_VI_WriteStats	*wrStats,		// >>> #CE_3.1.0-r6 2026-09-18
				LVBoolean			*err_f,
				uInt16				*err_c)
{
	MgErr	err;

	VARMAN_LOCK_EXCLUSIVE();

	err = __varCreate_CmdX(vch, glob, name, par, writerID, wrStats, NULL, NULL, NULL, err_f, err_c);

	VARMAN_UNLOCK_EXCLUSIVE();

	return err;
}
