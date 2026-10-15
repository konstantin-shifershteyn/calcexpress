/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include "subr\GetForParams.c"

CSE_PRIVATEFUNC
MgErr	Opr_for(	_ScrParserInst	*pi,
			int32		*exOff,
			int32		*bOff,
			int32		*cOff,
			LVBoolean	*err_f)
{
	_OprInfo	opr;
	iPScrElement	*pe;
	_ForStatParams	fss;
	MgErr		err = noErr;

	// ***	Create 'for' statement source reference
	// --------------------------------------------
	if (err = iPScrArray_CreateSourceReference(pi, *bOff, *cOff))
		goto out;

	// ***	Test for the error: missing ';' before keyword
	// ---------------------------------------------------
	if (pi->expStat > SUBEXP_NONE) {
		err = kwUnexpectedError(pi, "for", err_f);	goto out;
	}

	// ***	занести в стек операторов
	// ------------------------------
	opr.code	= OP_FOR;
	opr.nLine	= pi->curLine;
	opr.PScrIdx	= pi->PScrIdx;
	if (err = LblTbl_NewLabel(pi, &opr.label1) )	goto out;
	if (err = LblTbl_NewLabel(pi, &opr.label2) )	goto out;
	if (err = OprStack_Push(pi, &opr))		goto out;


	// ------------------------
	// определить параметры for
	// ------------------------
	err = GetForParams(pi, cOff, &fss, err_f);
	if (err || *err_f)  goto out;


	pi->NestingLevel += 1;


	// ------------------------------
	// ***	(1) init-expression   ***
	// ------------------------------

	// ***	Create initializing expression statement
	if (err = iPScrArray_NewExpression(pi, fss.iExpOff))	goto out;
	iPScrArray_FinishExpressionEx(	pi, fss.iExpOffEnd, fss.iExpOff + fss.iExpLen);
	iPScrArray_UpdateLineNumbers(	pi, fss.iExpLine, fss.iExpLine);

	iPScrArray_SetParentAndNote(pi, pi->PScrIdx-1, "'for' init-expression");

//	// ***	Set next statement index (jump over loop-exp)
//	pe = iPScrArray_GetPnt(pi, -1);
//	pe->jumpAction	= JA_JUMP;
//	pe->jumpIndex	= -1;

	// ***	Parse initial expression (init-exp)
	err = CSE_ParseExpression(pi, -1, FALSE, err_f);
	if (err || *err_f)  goto out;



	// -------------------------------------
	// ***	(2) conditional expression   ***
	// -------------------------------------

	// ***	Create conditional expression statement
	if (err = iPScrArray_NewExpression(pi, fss.cExpOff))	goto out;
	iPScrArray_FinishExpressionEx(	pi, fss.cExpOffEnd, fss.cExpOff + fss.cExpLen);
	iPScrArray_UpdateLineNumbers(	pi, fss.cExpLine, fss.cExpLine);

	iPScrArray_SetParentAndNote(pi, pi->PScrIdx-2, "'for' conditional expression");

	// *** label2 должна указывать на подвыражение после тела цикла
	// значение label2 будет определено при извлечении for из стека
	pe = iPScrArray_GetPnt(pi, -1);
	pe->jumpAction	= JA_JUMP_IFF;
	pe->jumpIndex	= opr.label2;

	// ***	Parse conditional expression (cond-exp)
	err = CSE_ParseExpression(pi, -1, FALSE, err_f);
	if (err || *err_f)  goto out;

	// ***	if cond-expression is empty, jumpAction must be an unconditional jump
	if (pe->stmType != STMTYPE_EXPRESS) {
		pe->jumpAction	= JA_JUMP;
		pe->jumpIndex	= -1;
	}

	// ---------------------------------------
	// ***	(3) Jump-Stub over loop-expression
	// ---------------------------------------
	if (err = iPScrArray_CreateJumpStub(pi, fss.pExpOff))	goto out;
	pe = iPScrArray_GetPnt(pi, -1);
	pe->jumpIndex = -2;		// 


	// ------------------------------
	// ***	(4) loop-expression   ***
	// ------------------------------

	// ***	Create loop-expression statement
	if (err = iPScrArray_NewExpression(pi, fss.pExpOff))	goto out;
	iPScrArray_FinishExpressionEx(	pi, fss.pExpOffEnd, fss.pExpOff + fss.pExpLen);
	iPScrArray_UpdateLineNumbers(	pi, fss.pExpLine, fss.pExpLine);

	iPScrArray_SetParentAndNote(pi, pi->PScrIdx-4, "'for' loop-expression");

	// *** label1 points to loop-exp (executes after for-loop boby)
	// будет использовано при извлечении for из стека
	LblTbl_SetLabel(pi, opr.label1, pi->PScrIdx);

	// ***	Parse loop expression (loop-exp)
	err = CSE_ParseExpression(pi, -1, FALSE, err_f);
	if (err || *err_f)  goto out;

	// ***	Set jump address to conditional expression
	pe = iPScrArray_GetPnt(pi, -1);
	pe->jumpIndex = -2 + LABEL_DIRECTION_THRESHOLD;


out:
	*exOff = *cOff;
	return	err;
}
