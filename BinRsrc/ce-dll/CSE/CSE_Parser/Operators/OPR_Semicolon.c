/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

// function body is in this file below
CSE_PRIVATEFUNC	MgErr	GroupExpStatements(  _ScrParserInst *pi);

CSE_PRIVATEFUNC
MgErr	Opr_Semicolon(	_ScrParserInst	*pi,
						int32			*exOff,
						int32			*cOff,
						LVBoolean		*err_f )
{
	Bool32	fCanBeGrouped;
	MgErr	err = noErr;


	// ***	Create expression statement
	// --------------------------------
	if (err = iPScrArray_CreateExpression(pi, *exOff, *cOff))
		goto out;

	iPScrArray_SetParentAndNote(pi, -1, "expression statement");

	// ***	Set line number to line where the statement begins
	if (pi->expStat > SUBEXP_NONE) {
		iPScrArray_UpdateLineNumbers(pi, pi->expLine, pi->expLine);
	}

	// *** PARSE EXPRESSION STATEMENT
	// ------------------------------
	err = CSE_ParseExpression(pi, -1, FALSE, err_f);
	if (err || *err_f)	goto out;


	// -------------------------------
	// ÇÀÂÅÐØÈÒÜ if,else,while,for !!!
	// -------------------------------
//	err = kwCompleteTopLevelStatement(pi, *cOff);	// !!!CAN ADD ELEMENT(S) TO pi->iPScr

	fCanBeGrouped	= pi->fCanBeGrouped;		// remember status

	if (err = kwCompleteTopLevelStatement_EXP(pi, *cOff))	// !!!CAN ADD ELEMENT(S) TO pi->iPScr
		goto out;
//	if (pi->iCESP->fGroupExpStm && fCanBeGrouped)
	if (fCanBeGrouped)
		err = GroupExpStatements(pi);

out:
	*exOff = *cOff;
	return	err;
}


// ==========================================================
// 	The function joins two expression-statements
// ==========================================================
CSE_PRIVATEFUNC
MgErr	GroupExpStatements(_ScrParserInst *pi)
{
	iPScrElement	*pe1, *pe2;
	MgErr		err = noErr;

	if (pi->PScrIdx < 1) return noErr;

	pe1 = iPScrEPnt(*(pi->iPScr), pi->PScrIdx - 1);
	pe2 = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);

	if ((pe1->stmType != STMTYPE_EXPRESS) || (pe1->jumpAction != JA_JUMP))	return noErr;
	if ((pe2->stmType != STMTYPE_EXPRESS) || (pe2->jumpAction != JA_JUMP))	return noErr;

	// ***	Change source info
	pe1->stmLength	= pe2->stmOffset - pe1->stmOffset + pe2->stmLength;
	pe1->expLength	= pe2->expOffset - pe1->expOffset + pe2->expLength;

	// ***	Decrease index
	pi->PScrIdx -= 1;

// >>> #CE_3.0.5 2025-07-18
	err = CEKernel_MergeExpressions(pe1->ip, pe2->ip);	// add expression from pe2 instance to pe1 instance
	iPScr_FreeElement(pe2);								// release resources used by pe2 instance
	return err;
// <<< #CE_3.0.5 2025-07-18
}
