/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

CSE_PRIVATEFUNC
MgErr	Opr_cuBracket_R(
			_ScrParserInst	*pi,
			int32			*exOff,
			int32			*bOff,
			int32			*cOff,
			LVBoolean		*err_f)
{
	_OprInfo	*opp;
	MgErr		err = noErr;


	// ***	Create '}' statement source reference
	// ------------------------------------------
	if (err = iPScrArray_CreateSourceReference(pi, *bOff, *cOff))
		goto out;

	// ***	Test for the error: missing ';' before keyword
	// ---------------------------------------------------
	if (pi->expStat != SUBEXP_NONE) {		// !!!
		err = kwUnexpectedError(pi, "}", err_f);
		goto out;
	}


	if (opp = OprStack_TopPnt(pi)) {
		if (opp->code == OP_CURLY_BRACKET_L) {
			OprStack_DecLen(pi);			// óäàëèòü '{' èç ñòåêà
			err = kwCompleting(pi, *cOff);	// ÇÀÂÅÐØÈÒÜ if,else,while,for !!!
			goto out;						// !!!CAN ADD ELEMENT(S) TO pi->iPScr
		}
	}
	err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, FALSE,
			"Illegal '}' without matching '{'", NULL, NULL);
	*err_f = LVTRUE;

out:
	*exOff = *cOff;
	return	err;
}
