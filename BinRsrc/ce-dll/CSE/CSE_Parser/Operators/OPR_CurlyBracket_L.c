/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

CSE_PRIVATEFUNC
MgErr	Opr_cuBracket_L(
			_ScrParserInst	*pi,
			int32			*exOff,
			int32			*bOff,
			int32			*cOff,
			LVBoolean		*err_f)
{
	_OprInfo	opr;
	MgErr		err = noErr;


	// ***	Create '{' statement source reference
	// ------------------------------------------
	if (err = iPScrArray_CreateSourceReference(pi, *bOff, *cOff))
		goto out;

	// ***	Change stmType to STMTYPE_SRCREFSTUB (in order to skip it in debugging execution)
	(iPScrEPnt(*(pi->iPScr), pi->PScrIdx))->stmType	= STMTYPE_SRCREFSTUB;


	// ***	Test for the error: missing ';' before keyword
	// ---------------------------------------------------
	if (pi->expStat > SUBEXP_NONE) {
		err = kwUnexpectedError(pi, "{", err_f);
		goto out;
	}

	// ***	поместить в стек операторов
	// --------------------------------
	opr.code	= OP_CURLY_BRACKET_L;
	opr.nLine	= pi->curLine;
	opr.PScrIdx	= pi->PScrIdx;
	opr.label1	= -1;
	opr.label2	= -1;
	err = OprStack_Push(pi, &opr);

out:
	*exOff = *cOff;
	return	err;
}
