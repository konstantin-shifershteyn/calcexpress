/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

CSE_PRIVATEFUNC
MgErr	Opr_else(	_ScrParserInst	*pi,
					int32			*exOff,
					int32			*bOff,
					int32			*cOff,
					LVBoolean		*err_f)
{
	_OprInfo		*opp;
	iPScrElement	*pe;
	char			*errmsg = NULL;
	MgErr			err = noErr;


	// ---------------
	// контроль ошибок
	// ---------------
	if ( !(opp = OprStack_TopPnt(pi)) ) {
		errmsg = "'else' without preceding 'if'";		goto out;
	}
	if (opp->code != OP_IF) {
		errmsg= "Illegal 'else' without matching 'if'";		goto out;
	}
	if (pi->expStat > SUBEXP_NONE) {
		errmsg = "Missing ';' before 'else'";  			goto out;
	}

	// -----------------------
	// ѕреобразовать if в else
	// -----------------------
	opp->code	= OP_ELSE;

	// ----------------------------------------------------------
	// ѕустое подвыражение, которое перепрыгивает через else блок
	// ----------------------------------------------------------
	if (err = iPScrArray_CreateJumpStub(pi, *bOff))
		return err;

	pe = iPScrArray_GetPnt(pi, -1);
	pe->jumpIndex	= opp->label2;		// index of label
	pe->jumpAction	= JA_JUMP;

	// ------------------------
	// сохранить текущую строку
	// ------------------------
	opp->nLine	= pi->curLine;

	// --------------------------------------
	// установить label1 на начало else блока
	// --------------------------------------
//	LblTbl_SetLabel(pi, opp->label1, pi->PScrIdx);
	LblTbl_SetLabel(pi, opp->label1, pi->PScrIdx + 1);	// +1 is jump to 'else' record

out:

	// ***	Create 'else' statement source reference
	if (err = iPScrArray_CreateSourceReference(pi, *bOff, *cOff))
		goto out;

	if (errmsg) {
		err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, FALSE, "%s", errmsg, NULL);
		*err_f = LVTRUE;
	}
	*exOff = *cOff;		// пропустить else
	return	err;
}
