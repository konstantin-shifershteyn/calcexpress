/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =======================
CSE_PRIVATEFUNC
MgErr	TokenError(	_ScrParserInst	*pi,
			int32		cOffset,
			char		*cfmt,
			void		*par1,
			void		*par2,
			LVBoolean	*err_f)
{
	MgErr	err = noErr;

	// ***	Create source reference
	if (err = iPScrArray_CreateSourceReference(pi, pi->expOffset, cOffset))
		goto out;

	// ***	Error message
	err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, TRUE, cfmt, par1, par2);
out:
	*err_f = LVTRUE;
	return err;
}

// =======================
CSE_PRIVATEFUNC
MgErr	kwUnexpectedError(	_ScrParserInst	*pi,
				char		*kwName,
				LVBoolean	*err_f)
{
	*err_f	= LVTRUE;
	return	CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, FALSE, "Unexpected '%s' (missing ';' ?)", kwName, NULL);
}
