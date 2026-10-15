/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

void	__fill_AliasInfoReport(_AliasRecord *ap, _VarInfoReport *vir);

// ================================
MgErr vmCmd_Alias_InfoList( _VarInfo *dout, LVBoolean *err_f, uInt16 *err_c )
{
	_VarInfoReport	vir;
	_AliasRecord	*ap;
	int32		i;
	uChar		*pnt;
	int32		len, lenTotal, numTotal;
	MgErr		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_SHARED();
	
	dout->type = VM_TYPE_STR;
	dout->num  = 0;			// it will contain number of records


	// ***	Produce output string
	// ==========================
	for(lenTotal = numTotal = 0, pnt=NULL; ; ) {

		// ***	PASS 1:	Calc output string length
		// ***	PASS 2:	Fill out the output string

		for (i=0; i < ALIAS_HASH_TABLE_SIZE; i++) {
			for( ap = aht[i]; ap; ap = ap->lnkHashList ) {
				__fill_AliasInfoReport(ap, &vir);		// fill record
				len = VInfo_To_CString(pnt, &vir);	//
				if (pnt)
					pnt += len;
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
	VARMAN_UNLOCK_SHARED();

	return err;
}

// ================================
void	__fill_AliasInfoReport(_AliasRecord *ap, _VarInfoReport *vir)
{
	vir->pntAlias	= LStrBuf(*(ap->name));
	vir->lenAlias	= LStrLen(*(ap->name));

	vir->pntName	= LStrBuf(*(ap->var->name));
	vir->lenName	= LStrLen(*(ap->var->name));

	if (ap->vcIndex == CTBL_GBLIDX)
		vir->vch = -1;
	else	vir->vch = ((*ctbl)->data[ap->vcIndex]).key;	// vc handle

	vir->type	= ap->var->inf.type;
	vir->fHist	= (ap->var->hist != NULL);
}
