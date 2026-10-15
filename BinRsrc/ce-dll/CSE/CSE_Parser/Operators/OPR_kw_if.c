/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

CSE_PRIVATEFUNC
MgErr	Opr_if(		_ScrParserInst	*pi,
			int32		*exOff,
			int32		*bOff,
			int32		*cOff,
			LVBoolean	*err_f)
{
	_OprInfo	opr;
	iPScrElement	*pe;
	MgErr		err = noErr;


	// ***	Create 'if' statement source reference
	// -------------------------------------------
	if (err = iPScrArray_CreateSourceReference(pi, *bOff, *cOff))
		goto out;

	// ***	Test for the error: missing ';' before keyword
	// ---------------------------------------------------
	if (pi->expStat > SUBEXP_NONE) {
		err = kwUnexpectedError(pi, "if", err_f);	goto out;
	}

	// ***	занести в стек операторов
	// ------------------------------
	opr.code	= OP_IF;
	opr.nLine	= pi->curLine;
	opr.PScrIdx	= pi->PScrIdx;
	if (err = LblTbl_NewLabel(pi, &opr.label1) )	goto out;
	if (err = LblTbl_NewLabel(pi, &opr.label2) )	goto out;
	if (err = OprStack_Push(pi, &opr))				goto out;


	pi->NestingLevel += 1;

	// -----------------------------------------------------------------
	// выделить условное выражение и поместить его в массив подвыражений
	// -----------------------------------------------------------------
	err = SaveConditionalExp(pi, cOff, "if", err_f);
	if (err || *err_f)
		goto out;

	iPScrArray_SetParentAndNote(pi, pi->PScrIdx-1, "'if' conditional expression");

	// -----------------------------
	// установить параметры перехода
	// -----------------------------
	pe = iPScrArray_GetPnt(pi, -1);
	pe->jumpAction	= JA_JUMP_IFF;
	pe->jumpIndex	= opr.label1;


	// *** PARSE CONDITIONAL EXPRESSION
	// --------------------------------
	err = CSE_ParseExpression(pi, -1, FALSE, err_f);

out:
	*exOff = *cOff;
	return	err;
}
