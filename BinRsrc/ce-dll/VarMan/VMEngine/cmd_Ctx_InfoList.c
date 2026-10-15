/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr vmCmd_Context_InfoList(
				LStrHandle	Creator,
				_VarInfo	*dout,
				LVBoolean	*err_f,
				uInt16		*err_c )
{
	int32		i;
	int32		vcNum, len;
	int32		*pnt;
	Bool32		fCreator = FALSE;
	MgErr		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}
	*err_c = VM_ERR_NOERROR;

	VARMAN_LOCK_EXCLUSIVE();

	if (Creator)
		fCreator = LStrLen(*Creator);

	// ---
	dout->type = VM_TYPE_STR;
	dout->num  = 0;			// it will contain number of VC handles


	// ***	Determine number of opened contexts
	vcNum = 0;
	for (i=0; i < (*ctbl)->dimSize; i++) {
		if ( ((*ctbl)->data[i].key != -1)&&(i != CTBL_GBLIDX) ) {
			// ----------
			if (fCreator)
				if (!LStr_iCmp(Creator, (*ctbl)->data[i].Creator))
					continue;
			// ----------
			vcNum += 1;
		}
	}
	len = vcNum * sizeof(int32);

	// ***	Allocate memory for output string (dout->str may contain NULL)
	if (err = LStr_SetSizeOrCreate(&(dout->str), len))
		goto out;

	// ***	Fill in the output string with context handles (they are int32)
	pnt = (int32*)LStrBuf(*(dout->str));
	for (i=0; (i < (*ctbl)->dimSize) && vcNum; i++) {
		if ( ((*ctbl)->data[i].key != -1) && (i != CTBL_GBLIDX) ) {
			// ----------
			if (fCreator)
				if (!LStr_iCmp(Creator, (*ctbl)->data[i].Creator))
					continue;
			// ----------
			vcNum -= 1;
			dout->num += 1;
			*pnt++ = (*ctbl)->data[i].key;
		}
	}
out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return err;
}
