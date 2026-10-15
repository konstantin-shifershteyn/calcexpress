/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__SaveForParams_C__
#define	__SaveForParams_C__

typedef	struct {
	int32	iExpOff;
	int32	iExpOffEnd;	// offset after. it can be greater than xExpOff + xExpLen
	int32	iExpLen;
	int32	iExpLine;	// line number where init-exp expression begins
	int32	cExpOff;
	int32	cExpOffEnd;	// offset after. it can be greater than xExpOff + xExpLen
	int32	cExpLen;	
	int32	cExpLine;	// line number where cond-exp expression begins
	int32	pExpOff;
	int32	pExpOffEnd;	// offset after. it can be greater than xExpOff + xExpLen
	int32	pExpLen;	
	int32	pExpLine;	// line number where loop-exp expression begins
} _ForStatParams;

// CSE_PRIVATEFUNC	char * __for_testAlphaDig(char *p, int32 off, int32 len);


// ------------------------
CSE_PRIVATEFUNC
MgErr GetForParams(	_ScrParserInst	*pi,
			int32		*offset,
			_ForStatParams	*fsp,
			LVBoolean	*err_f)
{
//	int32		lineCnt;
	int32		brLevel, sqLevel, nSemicolons;
	int32		begOff	= *offset;
	int32		endOff	= 0;
	int32		stmLine	= pi->curLine;
	char		*errmsg	= NULL;
	MgErr		err	= noErr;


	// Пропустить пробелы и комментарии
	// --------------------------------
	*offset = begOff =	SkipBlanksAndComments(pi, *offset, &(pi->curLine), err_f);
	if (*err_f) goto out;

	// ожидается '(' как начало for параметров
	// ---------------------------------------
	if (GetToken(pi, offset, &(pi->curLine), err_f) != OP_BRACKET_L) {
		if (!(*err_f)) {
//			err = spError(pi, stmLine, "Missing '(' after 'for'", NULL, NULL);

			iPScrArray_UpdateSourceReferenceLength(pi, *offset);
			err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, TRUE,
					"Missing '(' after 'for'", NULL, NULL);

			*err_f = LVTRUE;
		}
		goto out;
	}

	*offset = begOff =	SkipBlanksAndComments(pi, *offset, &(pi->curLine), err_f);
	if (*err_f) goto out;

	brLevel		= 1;
	sqLevel		= 0;
	nSemicolons	= 0;

	fsp->iExpLine	= pi->curLine;	// line number where init-exp expression begins

	fsp->iExpOff	= fsp->cExpOff		= fsp->pExpOff		= *offset;
	fsp->iExpOffEnd	= fsp->cExpOffEnd	= fsp->pExpOffEnd	= *offset;
	fsp->iExpLen	= fsp->cExpLen		= fsp->pExpLen		= 0;
	fsp->iExpLine	= fsp->cExpLine		= fsp->pExpLine		= pi->curLine;

	// ищем конец for параметров
	// -------------------------
	while(*offset < pi->scrLen) {

		begOff = *offset;

		switch( GetToken(pi, offset, &(pi->curLine), err_f) ) {
			case OP_BRACKET_L :	brLevel++; break;
			case OP_BRACKET_R :	if (--brLevel <= 0) goto save;
						break;

			case OP_COLON:		// пропускаем, рассматриваем как CE IndexRange
			case OP_COMMENT:	// пропускаем
			case OP_GENERAL:	// пропускаем
						break;

			case OP_SQUARE_BRACKET_L:	sqLevel++;	break;
			case OP_SQUARE_BRACKET_R:	sqLevel--;	break;

			case OP_SEMICOLON:
				switch (nSemicolons++) {

					case 0:	fsp->iExpOffEnd	= *offset;
						fsp->iExpLen	= begOff - fsp->iExpOff;

						*offset = SkipBlanksAndComments(pi, *offset, &(pi->curLine), err_f);
						if (*err_f) goto out;

						fsp->cExpOff	= *offset;
						fsp->cExpLine	= pi->curLine;
						break;

					case 1:	fsp->cExpOffEnd	= *offset;
						fsp->cExpLen	= begOff - fsp->cExpOff;

						*offset = SkipBlanksAndComments(pi, *offset, &(pi->curLine), err_f);
						if (*err_f) goto out;

						fsp->pExpOff	= *offset;
						fsp->pExpLine	= pi->curLine;
						break;

					default:
						goto err0;
						// errmsg = "Invalid 'for' parameters expression";
						//goto err;
				}
			//	if (sqLevel)
			//		goto	save;	// ошибка будет отловлена
			//	if (brLevel > 1)	{
			//		errmsg = "Missing ')' in 'for' parameters expression";
			//		goto	err;
			//	}
				break;

			default :	if (*err_f)	goto out;
					goto err0;
		}
	}
err0:	errmsg = "Unterminated 'for' parameters expression. Missing ')'";
err:
//	err = spError(pi, stmLine, "%s", (LStrHandle)errmsg, NULL);

	iPScrArray_UpdateSourceReferenceLength(pi, *offset);
	err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, TRUE,
				"%s", errmsg, NULL);

	*err_f = LVTRUE;
	goto out;

save:
	// ***	complete loop-exp expression
//	fsp->pExpOffEnd	= *offset;
	fsp->pExpLen	= begOff - fsp->pExpOff;		// длина последней части
	if (fsp->pExpLen)	fsp->pExpOffEnd	= begOff;	// without final ')'
	else			fsp->pExpOffEnd	= *offset;	// including final ')'

	// -------------------------------------------
	// проверить условное выражение (поверхностно)
	// -------------------------------------------
	if (nSemicolons != 2) {
		errmsg = "Invalid 'for' parameters. Expected: for(init-exp; cond-exp; loop-exp)";
		goto err;
	}

//	if (sqLevel > 0) {
//		errmsg = "Missing ']' in 'for' parameters expression";		goto err;
//	}
//	if (sqLevel < 0) {
//		errmsg = "Unexpected ']' in 'for' parameters expression";	goto err;
//	}


	// --------------------------------------------
	// еще одна проверка синтаксиса (поверхностная)
	// --------------------------------------------
//	if (errmsg = __for_testAlphaDig(pi->scr, fsp->iExpOff, fsp->iExpLen))	goto err;
//	if (errmsg = __for_testAlphaDig(pi->scr, fsp->cExpOff, fsp->cExpLen))	goto err;
//	if (errmsg = __for_testAlphaDig(pi->scr, fsp->pExpOff, fsp->pExpLen))	goto err;

out:
	// ***	Complete 'for' source reference statement
	iPScrArray_FinishSourceReference(pi, *offset);

	return	err;
}

/*
// ----------------
CESP30_PRIVATEFUNC
char * __for_testAlphaDig(char *p, int32 off, int32 len)
{
	if (__testAlphaDig(p,off,len))
		return	"Invalid 'for' parameters expression";
	return NULL;
}
*/

#endif
