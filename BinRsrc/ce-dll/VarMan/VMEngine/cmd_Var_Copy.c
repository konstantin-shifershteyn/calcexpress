/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#define	VM_DST_ERR_OFFSET	1000	// influences CE variable functions + online help

MgErr	vmCmd_Var_Copy(
				_VM_2Var_Info	*vars,
				uInt32			writerID,		// >>> #CE_3.0.7 2026-07-23
				LVBoolean		*err_f,
				uInt16			*err_c )
{
	_VarRecord	*dvp;
	_VarInfo	*src, *dst;
	LVBoolean	glob;
	MgErr		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	// ------------------------------
	// *** search for source variable
	// ------------------------------
	glob = (vars->src_vch == -1);
	if ( !(dvp = __GetVarPointer(vars->src_vch, &glob, vars->src_name, NULL, err_c))) {
		*err_f	= LVTRUE;
		goto out;			// err_f и err_c уже установлены
	}
	src = &(dvp->inf);

	// -----------------------------------
	// *** search for destination variable
	// -----------------------------------
	glob	= (vars->dst_vch == -1);
	if ( dvp = __GetVarPointer(vars->dst_vch, &glob, vars->dst_name, NULL, err_c)) {
		//*err_f	= LVFALSE;
		dst = &(dvp->inf);

		// *** сравнить типы переменных
		if (src->type == dst->type) {
			err = Var_Copy(dst, src);			// скопировать если типы совпадают
			dvp->writerID = writerID;			// >>> #CE_3.1.0 2026-08-20
			__UpdateVarWritingTimeNow(dvp);		// >>> #CE_3.1.0-r6 2026-09-17
			if (dvp->hist)
				_VHistory_AddCurValue(dvp);
		}
		else {									// ошибка если разные типы
			*err_c = VM_ERR_VAR_INVTYPE;
			*err_f = LVTRUE;
		}
		goto out;
	}
	*err_f	= (*err_c != 0);

	// ----------------------------------------
	// *** If destination variable is not found
	// ----------------------------------------
	if ((*err_c == VM_ERR_VAR_NOTFOUND) && vars->enaCreate) {

		// *** если не нашли и можно создать, то создать
		*err_f	= LVFALSE;
//		*err_c	= 0;
		glob	= (vars->dst_vch == -1);
		err = __varCreate_CmdX(	vars->dst_vch,
					&glob,
					vars->dst_name,
					src,
					writerID, NULL,
					NULL, NULL,
					NULL,
					err_f, err_c);
	}
	if (*err_f)	{	// err_f и err_c уже установлены
		*err_c += VM_DST_ERR_OFFSET;
	}
out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return err;
}
