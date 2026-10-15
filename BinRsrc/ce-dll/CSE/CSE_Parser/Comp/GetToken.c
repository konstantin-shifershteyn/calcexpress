/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include "TokenRoutines.c"

CSE_PRIVATEFUNC
int32 GetToken ( _ScrParserInst *pi, int32 *offset, int32 *lineCnt, LVBoolean *err_f);

// =======================
CSE_PRIVATEFUNC
int32 GetNextSignificantToken (	_ScrParserInst	*pi,
				int32		*offset,
				int32		*lineCnt,
				LVBoolean	*err_f)
{
	int32	cType;

	// ***	Skip blanks and comments
	do {
		cType = GetToken(pi, offset, lineCnt, err_f);
		if (*err_f)	break;
	} while (cType == OP_COMMENT);
	return cType;
}

// =======================
CSE_PRIVATEFUNC
int32 GetToken ( _ScrParserInst *pi, int32 *offset, int32 *lineCnt, LVBoolean *err_f)
{
	int32		datlen;
	Bool32		comment;
	uChar		*pnt;
	int32		off;
	int32		boff;
	int32		RetCode = OP_GENERAL;

	pnt	= pi->scr;
	datlen	= pi->scrLen;
	off	= *offset;

	if ((off < 0) || (off >= datlen)) {
not_found:
		RetCode	= OP_EndOfScript;
		off = datlen;
		goto out;
	}

	// Пропустить пробельные символы
	// -----------------------------
	while (*(pnt+off) <= ' ') {
		if (*(pnt+off) == '\n')
			*lineCnt += 1;
		if (++off >= datlen)
			goto not_found;
	}
	// ----------------------
	// Определить тип лексемы
	// ----------------------
	switch( CharType(*(pnt+off)) ) {

		case 1:	// Identifier
			// ----------
			boff = off;
			off = _LookForEndOfToken(pi, off);

			pi->identOff	= boff;
			pi->identLen	= off - boff;
			pi->MayBeLabel	= TRUE;

			// сравнить с шаблонами
			RetCode = testKeyword(pi, boff, off);
			goto TheEnd;
		//	break;

		case 2:
		case 3:	// Digit or decimal point
			// ----------------------
			off = _LookForEndOfToken(pi, off);
			RetCode	= OP_GENERAL;
			break;

		default:
			switch (*(pnt+off)) {
				case ':' :	RetCode = OP_COLON;  off+= 1;	goto TheEnd;
				case ';' :	RetCode	= OP_SEMICOLON;			break;
				case '(' :	RetCode	= OP_BRACKET_L;			break;
				case ')' :	RetCode	= OP_BRACKET_R;			break;
				case '[' :	RetCode	= OP_SQUARE_BRACKET_L;	break;
				case ']' :	RetCode	= OP_SQUARE_BRACKET_R;	break;
				case '{' :	RetCode	= OP_CURLY_BRACKET_L;	break;
				case '}' :	RetCode	= OP_CURLY_BRACKET_R;	break;

				case '\"':	off = StringConstant( pi, off, err_f);
							RetCode	= OP_GENERAL;
							goto out;
				case '\'':	off = CharConstant( pi, off, err_f);
							RetCode	= OP_GENERAL;
							goto out;

// >>> #CE_3.1.0-r6 2026-09-28
				case '$' :	off = testDirective(pi, off, &comment, err_f);
							if (comment)	RetCode	= OP_COMMENT;
							else			RetCode	= OP_GENERAL;
							goto out;
// <<< #CE_3.1.0-r6 2026-09-28
					

				default  :	off = testComment(pi, off, &comment, lineCnt, err_f);
							if (comment)	RetCode	= OP_COMMENT;
							else			RetCode	= OP_GENERAL;
							goto out;
			}
			off += 1;
			break;
	}
out:
	pi->MayBeLabel	= FALSE;
TheEnd:
	*offset = off;

	if (*err_f)
		RetCode = OP_UNKNOWN;
	return	RetCode;
}

