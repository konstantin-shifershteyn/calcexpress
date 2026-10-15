/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__TokenPoutines_C__
#define	__TokenPoutines_C__

CSE_PRIVATEFUNC	__inline int32	CharType(uChar Sym);


// =================================================

// >>> #CE_3.1.0-r6 2026-09-28
// IMPORTANT: CE Parser also treats the Parsing Directives.
// BUT... CSE Parser handles the Directives in this function and does not pass them to CE Parser.
// so, CE Parser actually handles the Directives only when is called from a CE VI.
CSE_PRIVATEFUNC
int32	testDirective(	_ScrParserInst	*pi,
						int32			off,
						Bool32			*dir,
						LVBoolean		*err_f)
{
	uChar	*pnt = pi->scr;
	uChar	*eos;
	uChar	buf[32];
	int32	len, ct;

	*dir = FALSE;
	eos = (pnt + off);

	// ### Check for an Index Range Symbol
	// -----------------------------------
//	if (!StrNCmp(pnt+off, "$>", 2)) {	len = 2;	goto not_directive;	}
//	if (!StrNCmp(pnt+off, "$<", 2)) {	len = 2;	goto not_directive;	}
	if (__IsKeyword("$>", pnt+off, 2)) { len = 2;	goto not_directive;	}
	if (__IsKeyword("$<", pnt+off, 2)) { len = 2;	goto not_directive;	}

	// ### Check for a Parsing Directive
	// ---------------------------------
	eos = pnt + off + 1;
	for(;;) {	// scan for the end
		ct = CharType(*eos);
		if ((ct != 1) && (ct != 2))
			break;
		eos++;
	}
	len = (int32)(eos - (pnt + off));
	// ### compare
	if (__IsKeyword("$NCON" , pnt+off, len))	{ pi->fEnaNamedConst = TRUE;	goto directive;	}
	if (__IsKeyword("$NCOFF", pnt+off, len))	{ pi->fEnaNamedConst = FALSE;	goto directive;	}

	// ### ERROR: INVALID DIRECTIVE NAME
	memset(buf, 0, sizeof(buf));
	memmove(buf, pnt+off, min(len, sizeof(buf)-1));
	off += len;
	if (len>1)	TokenError(pi, off, "Invalid directive or special symbol \"%s\"",    (void*)buf, NULL, err_f);
	else		TokenError(pi, off, "Incomplete directive or special symbol \"%s\"", (void*)buf, NULL, err_f);
	return off;

not_directive:
	// ### ignore and go forward
	return off+1;

directive:
	// ### Just skeep the directive and mark it as a comment
	off += len;
	*dir = TRUE;
	return off;
}
// <<< #CE_3.1.0-r6 2026-09-28

// =================================================
CSE_PRIVATEFUNC
int32	CharConstant(	_ScrParserInst	*pi,
						int32			off,
						LVBoolean		*err_f)
{
	uChar	*pnt = pi->scr;

	if (*(pnt + off++) != '\'')	goto out;

	while (off < pi->scrLen) {
		switch( *(pnt + off) ) {

		case '\\' :	// escape sequence
				off += 2;	//skip
				break;
		case '\'' :	// final
				off += 1;
				goto out;

		case '\n' :	TokenError(pi, off, "newline in character constant", NULL,NULL, err_f);
				goto out;

		default :	off += 1;
				break;
		}
	}
	off = pi->scrLen;
	TokenError(pi, off, "Unexpected end of script in character constant", NULL,NULL, err_f);
out:
	return	off;
}

// =======================================================
CSE_PRIVATEFUNC
int32 StringConstant(	_ScrParserInst	*pi,
						int32			off,
						LVBoolean		*err_f)
{
	uChar	*pnt = pi->scr;

	if (*(pnt + off++) != '\"')	goto out;

	while (off < pi->scrLen) {
		switch( *(pnt + off) ) {

		case '\\' :	// escape sequence
				off += 2;	//skip
				break;
		case '\"' :	// final
				off += 1;
				goto out;

		case '\n' :
				TokenError(pi, off, "newline in string constant", NULL,NULL, err_f);
				goto out;

		default :	off += 1;
				break;
		}
	}
	off = pi->scrLen;
	TokenError(pi, off, "Unexpected end of script in string constant", NULL,NULL, err_f);
out:
	return	off;
}

// =======================================================
CSE_PRIVATEFUNC
int32 testComment(	 _ScrParserInst		*pi,
					int32				off,
					Bool32				*comment,
					int32				*lineCnt,
					LVBoolean			*err_f)
{
	uChar	ch;
	uChar	*pnt = pi->scr;
	int32	level, stLine;
	int32	bOffset;

	*comment = FALSE;

	if ((off+1) < pi->scrLen) {
		if (!StrNCmp(pnt+off, "//", 2))	goto l_com;
		if (!StrNCmp(pnt+off, "/*", 2))	goto c_com;

		if (!StrNCmp(pnt+off, "*/", 2)) {
			off += 2;
			TokenError(pi, off, "Unexpected '*/'", NULL,NULL, err_f);
			return	off;
		}
	}
	return	off+1;

l_com:	// *** Line Comment
	// ----------------
	*comment = TRUE;
	off += 2;
	while(off < pi->scrLen)	 {		// поиск конца строки
		if (*(pnt + off++) == '\n') {
			*lineCnt += 1;
			break;
		}
	}
	goto out;

c_com:	// *** C-style Comment
	// -------------------
	*comment	= TRUE;
	stLine		= *lineCnt;
	bOffset		= off;

	off += 2; level = 1;

	while (off < pi->scrLen) {

		ch = *(pnt+off);
		if (++off >= pi->scrLen)
			break;

		switch (ch) {
			case '/' :
				if (*(pnt+off)=='*') {
					off++; level++;
				}
				break;
			case '*' :
				if (*(pnt+off)=='/') {
					off++;
					if (--level <= 0)	goto out;
				}
				break;
			case '\n' :
				*lineCnt += 1;
			default:
				break;
		}
	}
	if (level > 0) {
	//	pi->expOffset = bOffset;	// ???
		TokenError(pi, off, "Unexpected end of script in comment started in line %d",
						 (void*)(size_t)stLine, NULL, err_f);
	}
out:
	return off;
}

// ======================================
CSE_PRIVATEFUNC
int32 SkipBlanksAndComments(_ScrParserInst	*pi,
							int32			off,
							int32			*lineCnt,
							LVBoolean		*err_f)
{
	char	*pnt 	= pi->scr;
	Bool32	fComment;

	while(off < pi->scrLen) {
		while(off < pi->scrLen) {
			if (*(pnt + off) > ' ')		break;
			if (*(pnt + off) == '\n')	*lineCnt += 1;
			off++;
		}

		off = testComment(pi, off, &fComment, lineCnt, err_f);
		if (*err_f)	break;
		if (!fComment)	{ off -= 1; break; }
	}
	return off;
}



// ##############################################################
// Return:
// 1:	'A'-'Z' , 'a'-'z' , '_'
// 2:	'0'-'9'
// 3:	'.'
// 4:	'\"'
// 5:	'\''

#pragma warning (disable: 4035)
CSE_PRIVATEFUNC	__inline int32	CharType(uChar Sym)
{
	static  uChar	CTTbl[256] = 
	{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 4, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 3, 0 ,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0 ,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1 ,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 ,
	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
	};
	return	(int32)CTTbl[Sym];
}
#pragma warning (default: 4035)

// =========================================

CSE_PRIVATEFUNC
int32 _LookForEndOfToken(_ScrParserInst *pi, int32 off)
{
	uChar	*pnt = pi->scr;

	while(off < pi->scrLen) {
		switch( CharType(*(pnt + off)) ) {
			case 1 :
			case 2 :
			case 3 : off++;
				 break;
			default: goto out;
		}
	}
out:
	return off;
}

// ==========================================

CSE_PRIVATEFUNC
int32 testKeyword(_ScrParserInst *pi, int32 begOff, int32 endOff)
{
	typedef struct {
		char	*name;
		int32	len;
		int32	code;
	} _kwrd_info;

	static	_kwrd_info	kw[] = {
		{ "if"			, 2	, OP_IF			},
		{ "else"		, 4	, OP_ELSE		},
		{ "for"			, 3	, OP_FOR		},
		{ "while"		, 5	, OP_WHILE		},
		{ "dowhile"		, 7	, OP_DOWHILE	},
		{ "goto"		, 4	, OP_GOTO		},
		{ "break"		, 5	, OP_BREAK		},
		{ "continue"	, 8	, OP_CONTINUE	},
		{ "return"		, 6	, OP_RETURN		}
	};
	static	kwSize = sizeof(kw)/sizeof(_kwrd_info);


	uChar	*pnt;
	int32	len, i;

	pnt = pi->scr + begOff;
	len = endOff - begOff;

	for (i=0; i<kwSize; i++) {
		if (kw[i].len == len)
			if (!StrNCmp(pnt, kw[i].name, len))
				return kw[i].code;
	}
	return OP_GENERAL;
}


// ---------------------------
CSE_PRIVATEFUNC
Bool32 __testAlphaDig(char *p, int32 off, int32 len)
{
	if (!len)	return FALSE;
	for (p = p+off; len > 0; p++, len--)
		if (isalnum(*p) || (*p=='_'))
			return FALSE;
	return TRUE; 
}


#endif
