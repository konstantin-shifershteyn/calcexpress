/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#define	__OK__	FALSE
#define	__ER__	TRUE

__inline Bool32	__test_alias_syntax(uChar *pnt, int32 len, LVBoolean *err_f, uInt16 *err_c)
{
	if ( test_freestyle_name_syntax(pnt, len) ) {
		*err_f = LVTRUE; *err_c = VM_ERR_ALIAS_INVNAME;
		return __ER__;
	}
	return __OK__;
}
#undef	__OK__
#undef	__ER__

// =============================

MgErr	vmCmd_Alias_Operation(
				int32		*VCH,
				LVBoolean	*glob,
				LStrHandle	name,
				_VarInfo	*par,
				LVBoolean	*err_f,
				uInt16		*err_c)
{
	_VarRecord	*vp;
	_AliasRecord	*ap;
	int32		vcIndex;
	char		*pnt;
	int32		len;
	MgErr		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();		// >>> #CE_3.0.2 2026-06-08
	//	EnterCriticalSection( &ctblLock );

	// ***	Find variable
	// ------------------
	if ( !(vp = __GetVarPointer(*VCH, glob, name, &vcIndex, err_c)) ) {
		*err_f = LVTRUE;
		goto out;
	}
	pnt = LStrBuf(*(par->str));
	len = LStrLen(*(par->str));

	switch( (int32)(par->num) ) {

		case VM_SUBCMD_ALIAS_Dispose :
			if (!(vp->alias)) {
				*err_f = LVTRUE;
				*err_c = VM_ERR_ALIAS_NOALIAS;
				goto out;
			}
			err = Alias_Destroy(vp->alias);
			break;

		case VM_SUBCMD_ALIAS_NewOrRename :

			if (vp->alias) {

				// ***	It's important to do it here
				if (__test_alias_syntax(pnt, len, err_f, err_c))
					goto out;

				// ***	Test whether the submitted alias name is unique 
				if (ap = Alias_Find(NULL, pnt, len)) {
					if (ap != vp->alias) {
						*err_f = LVTRUE;
						*err_c = VM_ERR_ALIAS_NONUNIQUE;
					}
					goto out;
				}
				// ***	Due to Alias Hash-Table, renaming must be made as:
				// ***	Delete old alias and then create new one
				if (err = Alias_Destroy(vp->alias))
					goto out;
			}

		case VM_SUBCMD_ALIAS_New :

			// ***	Test syntax of the submitted alias name
			if (__test_alias_syntax(pnt, len, err_f, err_c))
				goto out;

			// ***	Create new alias
			err = Alias_Create( pnt, len, vp, vcIndex, err_f, err_c);
			break;

		default:
			*err_f = LVFALSE;
			*err_c = VM_ERR_NOERROR;
			break;
	}
out:
	VARMAN_UNLOCK_EXCLUSIVE();		// >>> #CE_3.0.2 2026-06-08
	//	LeaveCriticalSection( &ctblLock );
	return err;
}

