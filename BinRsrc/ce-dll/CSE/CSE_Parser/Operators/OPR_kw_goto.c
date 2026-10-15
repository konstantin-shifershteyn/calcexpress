/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include "subr\GetGotoLabel.c"

CSE_PRIVATEFUNC
MgErr	Opr_goto(	_ScrParserInst	*pi,
			int32		*exOff,
			int32		*bOff,
			int32		*cOff,
			LVBoolean	*err_f)
{
	iPScrElement	*pe;
	int32		label;
	MgErr		err = noErr;


	// ***	Create 'goto' statement
	// ----------------------------
	if (err = iPScrArray_CreateJumpStatement(pi, *bOff, *cOff))
		goto out;

	// ***	Test for the error: missing ';' before keyword
	// ---------------------------------------------------
	if (pi->expStat > SUBEXP_NONE) {
		err = kwUnexpectedError(pi, "goto", err_f);	goto out;
	}

	// ---------------------
	// определить метку
	// ---------------------
	err = GetGotoLabel(pi, cOff, &label, err_f);
	if (err || *err_f)  goto out;

	// ------------------------------------------------
	// создать переход по goto
	// ------------------------------------------------
	pe = iPScrArray_GetPnt(pi, -1);
	pe->jumpAction	= JA_JUMP;
	pe->jumpIndex	= label;

	// -------------------------------
	// «ј¬≈–Ў»“№ if,else,while,for !!!
	// -------------------------------
	err = kwCompleteTopLevelStatement(pi, *cOff);	// !!!CAN ADD ELEMENT(S) TO pi->iPScr

out:
	*exOff = *cOff;
	return	err;
}
