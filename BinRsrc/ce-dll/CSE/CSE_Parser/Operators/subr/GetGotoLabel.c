/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__GetGotoLabel_C__
#define	__GetGotoLabel_C__

// ------------------------
CSE_PRIVATEFUNC
MgErr GetGotoLabel(	_ScrParserInst	*pi,
			int32		*offset,
			int32		*label,
			LVBoolean	*err_f)
{
	int32		bOff	= *offset;
	int32		curLine	= pi->curLine;
	MgErr		err	= noErr;


	// пропускаем пробелы и комментарии
	// --------------------------------
	*offset = bOff =	SkipBlanksAndComments(pi, *offset, &(pi->curLine), err_f);
	if (*err_f)	goto out;

	// определяем лексему
	// ------------------
	GetToken(pi, offset, &(pi->curLine), err_f);
	if (*err_f)	goto out;

	// Если не идентификатор, то ошибка
	// --------------------------------
	if (CharType(*(pi->scr + bOff)) != 1) {
//		err = spError(pi, curLine, "label name expected after 'goto'", NULL,NULL);

		iPScrArray_UpdateSourceReferenceLength(pi, *offset);
		err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, TRUE,
				"label name expected after 'goto'", NULL,NULL);
		*err_f = LVTRUE;
		goto out;
	}

	// Сохраняем информацию о метке
	// ----------------------------
	err = NLabel_GetOrCreateLabel(pi, label, pi->scr + bOff, *offset - bOff, err_f);
	if (err || *err_f)	goto out;

	// Проверяем наличие ';'
	// ---------------------
	err = IsThereSemicolon(pi, offset, 0, "goto", err_f);

out:
	// ***	Complete 'goto' statement
	iPScrArray_FinishJumpStatement(pi, *offset);

	return	err;
}

#endif
