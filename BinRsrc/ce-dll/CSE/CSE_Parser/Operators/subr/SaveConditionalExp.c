/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__SaveConditionalExp_C__
#define	__SaveConditionalExp_C__


CSE_PRIVATEFUNC
MgErr SaveConditionalExp(
				_ScrParserInst	*pi,
				int32			*offset,
				char			*kwName,
				LVBoolean		*err_f)
{
	int32		brLevel, sqLevel;
	int32		begOff	= *offset;
	int32		endOff	= 0;
	int32		stmLine	= pi->curLine;
	int32		expLine	= pi->curLine;
	char		*errmsg	= NULL;
	MgErr		err	= noErr;


	// Пропустить пробелы и комментарии
	// --------------------------------
	*offset = begOff =	SkipBlanksAndComments(pi, *offset, &(pi->curLine), err_f);
	if (*err_f) goto out;

	// ожидается '(' как начало условного выражения
	// --------------------------------------------
	if (GetToken(pi, offset, &(pi->curLine), err_f) != OP_BRACKET_L) {
		if (!(*err_f)) {
			iPScrArray_UpdateSourceReferenceLength(pi, *offset);
			err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, TRUE,
					"Missing '(' after '%s'", kwName, NULL);

			*err_f = LVTRUE;
		}
		goto out;
	}

	*offset = begOff =	SkipBlanksAndComments(pi, *offset, &(pi->curLine), err_f);
	if (*err_f) goto out;

	expLine	= pi->curLine;	// remember line number of beginning of expression

	// ищем конец условного выражения
	// ------------------------------
	brLevel	= 1;
	sqLevel = 0;
//	curLine = pi->curLine;		
	while(*offset < pi->scrLen) {
		endOff	= *offset;
		switch(GetToken(pi, offset, &(pi->curLine), err_f))
		{
			case OP_BRACKET_L :
					brLevel++;
					break;
			case OP_BRACKET_R :
					if (--brLevel <= 0) {
					//	if (sqLevel > 0) {
					//		errmsg = "Missing ']' in conditional expression";
					//		goto err;
					//	}
					//	if (sqLevel < 0) {
					//		errmsg = "Unexpected ']' in conditional expression";
					//		goto err;
					//	}
						goto out;
					}
					break;

			case OP_COLON:		// пропускаем, рассматриваем как CE IndexRange
			case OP_COMMENT:	// пропускаем
			case OP_GENERAL:	// пропускаем
						break;

			case OP_SQUARE_BRACKET_L:	sqLevel++;	break;
			case OP_SQUARE_BRACKET_R:	sqLevel--;	break;

			case OP_SEMICOLON:
					goto err_unterminated;
					//	errmsg = "Invalid conditional expression";
					//	goto err;
			default :
					if (*err_f)	goto out;
					goto err_unterminated;
		}
	}
err_unterminated:
	iPScrArray_UpdateSourceReferenceLength(pi, *offset);
	err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, TRUE,
				"Unterminated '%s' conditional expression. Missing ')'", kwName, NULL);

	*err_f = LVTRUE;
	goto out;

//err:
//	err = spError(pi, stmLine, "%s after '%s'", (LStrHandle)errmsg, (LStrHandle)kwName);
//	*err_f = LVTRUE;

out:

	// ***	Complete 'if'/'while' source reference statement
	iPScrArray_FinishSourceReference(pi, *offset);

	if (!(*err_f || err)) {
		// ***	Create expression statement
		if (!endOff)	endOff	= *offset;
		err = iPScrArray_CreateExpression(pi, begOff, endOff);
		// ***	Set line number to line where the expression begins
		iPScrArray_UpdateLineNumbers(pi, expLine, expLine);
	}
	return	err;
}


#endif
