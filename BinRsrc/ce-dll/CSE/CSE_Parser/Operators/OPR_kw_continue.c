/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include "subr\TestForSemicolon.c"

CSE_PRIVATEFUNC
MgErr	Opr_continue(	_ScrParserInst	*pi,
			int32		*exOff,
			int32		*bOff,
			int32		*cOff,
			LVBoolean	*err_f)
{
	_OprInfo	*opp;
	iPScrElement	*pe;
	int32		idx;
	MgErr		err = noErr;


	// ***	Create 'continue' statement
	// --------------------------------
	if (err = iPScrArray_CreateJumpStatement(pi, *bOff, *cOff))
		goto out;

	// ***	Test for the error: missing ';' before keyword
	// ---------------------------------------------------
	if (pi->expStat > SUBEXP_NONE) {
		err = kwUnexpectedError(pi, "continue", err_f);	goto out;
	}

	// ***	Test for semicolon ( ; )
	// -----------------------------
	err = IsThereSemicolon(pi, cOff, 0, "continue", err_f);
	if (err || *err_f)  goto out;	// !!!

	// ***	Complete 'continue' statement
	// ----------------------------------
	iPScrArray_FinishJumpStatement(pi, *cOff);

	// -------------------------------------
	// найти ближайший while или for в стеке
	// -------------------------------------
	if (opp = OprStack_TopPnt(pi)) {
		idx = OprStack_TopIndex(pi);
		while (opp) {
			switch(opp->code) {
				case OP_FOR :
				case OP_WHILE :
				case OP_DOWHILE :	goto save;
				default :		break;
			}
			opp = OprStack_GetPnt(pi, --idx);
		}
	}
	err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, FALSE,
			 "Illegal 'continue'", NULL, NULL);
	*err_f = LVTRUE;
	goto out;

save:
	// ---------------------------------------------------
	// создать переход по continue (jump to begin of loop)
	// ---------------------------------------------------
	pe = iPScrArray_GetPnt(pi, -1);
	pe->jumpAction	= JA_JUMP;
	pe->jumpIndex	= opp->label1;


	// -------------------------------
	// «ј¬≈–Ў»“№ if,else,while,for !!!
	// -------------------------------
	err = kwCompleteTopLevelStatement(pi, *cOff);	// !!!CAN ADD ELEMENT(S) TO pi->iPScr

out:
	*exOff = *cOff;
	return	err;
}
