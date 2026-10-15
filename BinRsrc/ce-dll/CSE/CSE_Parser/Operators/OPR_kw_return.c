/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include "subr\SaveReturnExp.c"

CSE_PRIVATEFUNC
MgErr	Opr_return(	_ScrParserInst	*pi,
			int32		*exOff,
			int32		*bOff,		// offset of 'r' in "return"
			int32		*cOff,		// offset after "return"
			LVBoolean	*err_f)
{
	iPScrElement	*pe;
	MgErr		err = noErr;


	// ***	Create 'return' expression statement
	// -----------------------------------------
	if (err = iPScrArray_NewExpressionEx(pi, *bOff, *cOff))
		goto out;

	// ***	Test for the error: missing ';' before keyword
	// ---------------------------------------------------
	if (pi->expStat > SUBEXP_NONE) {
		err = kwUnexpectedError(pi, "return", err_f);	goto out;
	}

	// ***	Skip blanks and comments after 'return'
	*cOff = SkipBlanksAndComments(pi, *cOff, &(pi->curLine), err_f);
	if (*err_f)	goto out;

	// ***	Update offset of an expression after 'return'
	iPScrArray_UpdateExpressionOffset(pi, *cOff);


	// ------------------------------------------------
	// создать выход по return;
	// ------------------------------------------------
	pe = iPScrArray_GetPnt(pi, -1);
	pe->jumpAction	= JA_STOP;
	pe->jumpIndex	= 0x7FFFFFFF;	// pe->jumpIndex	= -1;

	// ----------------------------------
	// —охранить выражение после 'return'
	// ----------------------------------
	err = SaveReturnExp(pi, cOff, err_f);
	if (err || *err_f)  goto out;

	iPScrArray_SetParentAndNote(pi, -1, "'return' statement");

	// ----------------------------------
	// *** PARSE EXPRESSION STATEMENT
	// *** after SaveReturnExp() and before kwCompleting() !!!
	// ----------------------------------
	err = CSE_ParseExpression(pi, -1, TRUE, err_f);
	if (err || *err_f)	goto out;


	// -------------------------------
	// «ј¬≈–Ў»“№ if,else,while,for !!!
	// -------------------------------
	err = kwCompleteTopLevelStatement(pi, *cOff);	// !!!CAN ADD ELEMENT(S) TO pi->iPScr

out:
	*exOff = *cOff;
	return	err;
}
