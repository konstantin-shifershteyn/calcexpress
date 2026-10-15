/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


_VarRecord * __fastcall __GetVarPointer(
						int32		vch,
						LVBoolean	*glob,
						LStrHandle	name,
						int32		*pVCIndex,
						uInt16		*err_c)
{
	_NameAliasInfo	nai;
	int32			vcIdx;
	_VarRecord		*vp;
	_AliasRecord	*ap;
	_HashTableRec	*hktbl;
	uInt32			hkey;

	if ( _ParseNameAlias(name, &nai, FALSE, err_c)) {		// >>> #CE_3.0.2 2026-06-14
		return NULL;
	}

	if (!nai.flgName) {
		// ===============================
		// ***	Поиск переменной по алиасу
		// ===============================
		if ( !(ap = Alias_Find(NULL, nai.pntAlias, nai.lenAlias)) ) {
			*err_c	= VM_ERR_ALIAS_NOTFOUND;
			return NULL;
		}
		vcIdx	= ap->vcIndex;				// index of context in the table
		vp		= ap->var;					// pointer to variable record
		*glob	= (vcIdx == CTBL_GBLIDX);	// !!! HERE !!!
		goto out;
//		return ap->var;
	}

	// ==============================
	// ***	Поиск переменной по имени
	// ==============================

	// ***	calc the context index in the context table
	if ( (vcIdx = Get_Context_Index(vch, (*glob != 0), err_c)) < 0 )
		return NULL;	// err_f и err_c уже установлены

	hkey	= HashKey(nai.pntName, nai.lenName);

val_vlist:
//	hktbl	= ((*ctbl)->data[vcIdx]).hktbl;
//	hktbl	= (ctblPnt->data[vcIdx]).hktbl;		// >>> #CE_3.1.0 2026-08-15
	hktbl	= &((ctblPnt->data[vcIdx]).hktbl[0]);	// >>> #CE_3.1.0 2026-08-18


	// ### search for var name
	if (!(vp = Var_FindName(&(hktbl[hkey]), nai.pntName, nai.lenName)) ) {
		// if not found
		if ((*glob) && (vcIdx != CTBL_GBLIDX)) {
			// *** initiate search in the global context
			vcIdx = CTBL_GBLIDX;
			goto val_vlist;
		}
		//*err_f = LVTRUE;
		*err_c = VM_ERR_VAR_NOTFOUND;
		return NULL;
	}
	// *** set sign of a global var
	if (vcIdx != CTBL_GBLIDX)
		*glob = LVFALSE;
out:
	if (pVCIndex)
		*pVCIndex = vcIdx;
	return	vp;
}

/*
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// >>> #CE_3.0.2 2026-06-06
// test function is to be called from a test VI
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
uInt32 __cdecl VMTest_FindVars(int32 repeat, int32 *VCH, _hLVA1Str varNames, uInt16 *errCodes, float64 *dblVals)
{
	int32		i;
//	_LVA1Str	*arrPnt = *varNames;
	int32		arrSize = (*varNames)->size;
	uInt32		ms = MillisecsTimer();
	_VarRecord	*vp;
	LVBoolean	global	= LVFALSE;
	LVBoolean	err_f	= LVFALSE;
	uInt16		err_c	= 0;

	if (repeat <= 0)
		repeat = 1;

	while (repeat-- > 0) {
		for (i=0; i < arrSize; i++) {
			global	= LVFALSE;
			err_f	= LVFALSE;
			err_c	= 0;
			vp	= __GetVarPointer(*VCH, &global, (*varNames)->val[i], NULL, &err_c);
//			vp	= __GetVarPointer(*VCH, &global, arrPnt->val[i], NULL, &err_c);
			errCodes[i] = err_c;
			dblVals[i]	= (vp != NULL) ? vp->inf.num : 0.0;
			//{
			//	### Test functions: LV vs CRT
			//	dblVals[i] = 0.0;
			//	if (vp) {
			//		MoveBlock(&(vp->inf.num), &dblVals[i], sizeof(dblVals[0]));
			//		memmove(&dblVals[i], &(vp->inf.num), sizeof(dblVals[0]));
			//	}
			//}
		}
	}
	return 	(MillisecsTimer() - ms);
}
// <<< #CE_3.0.2 2026-06-06
*/
