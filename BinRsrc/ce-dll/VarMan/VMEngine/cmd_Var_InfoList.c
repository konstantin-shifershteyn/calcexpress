/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


void	__fill_VarInfoReport(_VarRecord *vp, _VarInfoReport *vir);

// ====================================
MgErr vmCmd_Var_InfoList(	int32		vch,
							LVBoolean	*glob,
							LStrHandle	name,
							_VarInfo	*dout,
							LVBoolean	*err_f,
							uInt16		*err_c )
{
	_HashTableRec	*hktbl;
	int32			vcIndex;
	_VarInfoReport	vir = { NULL,0, NULL,0, 0,0,LVFALSE };
	int32			i_ht = 0;		// index in the hash table
	int32			i_va = 0;		// index in the var array (within a context)
	_VarRecord		*vp = NULL;		// pointer in the var list (within a context)
	uChar			*pnt = NULL;
	int32			len, lenTotal, numTotal;
	MgErr			err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_SHARED();		// >>> #CE_3.0.2 2026-06-08
	//	EnterCriticalSection( &ctblLock );

	dout->type = VM_TYPE_STR;
	dout->num  = 0;			// it will contain number of records

	if (name) {
		// =========================================================================
		// ***	If name is specified, make report for single variable (may be alias)
		// =========================================================================
		if( vp = __GetVarPointer(vch, glob, name, &vcIndex, err_c)) {
			//*err_f	= LVFALSE;
			// если нет ошибки, если же есть, то vir уже содержит заглушку
			// выход по ошибке не делаем !!!
			if (vcIndex == CTBL_GBLIDX)
					vir.vch = -1;
			else	vir.vch = ((*ctbl)->data[vcIndex]).key;	// vc handle
			__fill_VarInfoReport(vp, &vir);		// fill record
		}
		else *err_f	= LVTRUE;

		lenTotal = VInfo_To_CString( NULL, &vir);	// calc string length

		// ***	Allocate memory for output string (dout->str may contain NULL)
		if (err = LStr_SetSizeOrCreate(&(dout->str), lenTotal))
			goto out;

		// ***	fill output string
		lenTotal = VInfo_To_CString(LStrBuf(*(dout->str)), &vir);
		// ---
		dout->num = numTotal = 1;
		goto out;
	}

	// ===========================================================
	// ***	Make report for all variables of the specified context
	// ===========================================================

	// ***	find context
	// =================
	if ( (vcIndex = Get_Context_Index_SG(vch, *glob, err_c)) < 0 ) {
		*err_f = LVTRUE;
		goto out;	// err_f и err_c уже установлены
	}

//	hktbl	= ((*ctbl)->data[vcIndex]).hktbl;
	hktbl	= &(((*ctbl)->data[vcIndex]).hktbl[0]);		// >>> #CE_3.1.0 2026-08-18

	vir.vch	= *glob ? -1 : vch;		// -1 if global context

	// ***	Produce output string
	// ==========================
	lenTotal = 0;
	numTotal = 0;
	pnt = NULL;
	// ### 2 passes loop
	for(;;) {
		// ***	PASS 1:	Calc output string length
		// ***	PASS 2:	Fill out the output string
		for (i_ht=0; i_ht < VAR_HASH_TABLE_SIZE; i_ht++) {
#if (VAR_ARRAY_SIZE > 0)
// >>> #CE_3.1.0 2026-08-18
			// ### run through the var array
			vp = &(hktbl[i_ht].varray[0]);
			for (i_va=0; i_va < VAR_ARRAY_SIZE; i_va++, vp++) {
				//vp = &(hktbl[i_ht].varray[i_va]);
				if (vp->name == NULL)		// if not used
					continue;
				__fill_VarInfoReport(vp, &vir);		// fill record
				len = VInfo_To_CString(pnt, &vir);	//
				if (pnt)
					pnt  += len;
				lenTotal += len;
				numTotal += 1;
			}
// <<< #CE_3.1.0 2026-08-18
#endif
			// ### run through the var list
			for( vp = hktbl[i_ht].vlist; vp; vp = vp->link ) {
				__fill_VarInfoReport(vp, &vir);		// fill record
				len = VInfo_To_CString(pnt, &vir);	//
				if (pnt)
					pnt  += len;
				lenTotal += len;
				numTotal += 1;
			}
		}
		if (pnt)
			break;

		// ***	PASS 1 ONLY
		// ***	Allocate memory for output string (dout->str may contain NULL)
		if (err = LStr_SetSizeOrCreate(&(dout->str), lenTotal))
			goto out;

		pnt = LStrBuf(*(dout->str));
		lenTotal = 0;
		numTotal = 0;
	}
	dout->num  = numTotal;	// number of records
out:
	VARMAN_UNLOCK_SHARED();		// >>> #CE_3.0.2 2026-06-08
	//	LeaveCriticalSection( &ctblLock );
	return err;
}

// ====================================
void	__fill_VarInfoReport(_VarRecord *vp, _VarInfoReport *vir)
{
	vir->pntName	= LStrBuf(*(vp->name));
	vir->lenName	= LStrLen(*(vp->name));
//	vir->vch	= vch;				// has to be done outside
	vir->type	= vp->inf.type;
	vir->fHist	= (vp->hist != NULL);
	if (vp->alias) {
		vir->pntAlias	= LStrBuf(*(vp->alias->name));
		vir->lenAlias	= LStrLen(*(vp->alias->name));
	}
	else {
		vir->pntAlias	= NULL;
		vir->lenAlias	= 0;
	}
}
