/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__TestForSemicolon_C__
#define	__TestForSemicolon_C__

// ------------------------
CSE_PRIVATEFUNC
MgErr IsThereSemicolon(	_ScrParserInst	*pi,
			int32		*offset,
			int32		idxOffset,
			char		*kwName,
			LVBoolean	*err_f)
{
//	int32		off	= *offset;
//	int32		curLine	= pi->curLine;
	int32		cType;
	MgErr		err	= noErr;


	// Пропустить пробелы и комментарии
	// --------------------------------
	cType = GetNextSignificantToken(pi, offset, &(pi->curLine), err_f);
	if (*err_f)	goto out;

	// ожидается ';'
	// -------------
	if (cType != OP_SEMICOLON) {

//	!? NO	iPScrArray_UpdateSourceReferenceLength(pi, *offset);

		err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx - idxOffset, estSyntaxError, TRUE,
				"Missing ';' after '%s'", (LStrHandle)kwName, NULL);

//		err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, TRUE,
//				"Missing ';' after '%s'", (LStrHandle)kwName, NULL);

		*err_f = LVTRUE;
		goto out;
	}
out:
//	*offset = off;
	return	err;
}

#endif
