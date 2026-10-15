/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// тело в конце файла
int32 _find_EOS( uChar *pnt, int32 off);

// 
int32 StrConst(
			_CE_Instance	*ip,
			uChar		*pnt,
			int32		off,
			int32		*expIdx,
			LVBoolean	*err_f,
			MgErr		*errp	)
{
	int32		boff, tmp;
	uChar		*dst, c;
	LStrHandle	buf;
	int32		len;

	*errp = noErr;

	len = LStrLen(*(ip->str));
	buf = ip->ROpd.str;

	// *** Выделить память под строку
	if (LStrLen(*buf) < len ) {

		if ( *errp = LStr_SetSize(buf, len) )
			goto out;
	}
	dst = LStrBuf(*buf);

BegLoop:
	boff = off-1;
	for (;;) {
		switch( c = *(pnt + off++)) {

			default :	// *** any character
				*dst++ = c;
				break;

			case '\"' :	// *** end of string
				goto EndLoop;

			case '\\' :	// *** escape-sequence
				tmp = EscapeSeq( ip, pnt, &off, err_f);
				if (*err_f) {		// invalid escape-sequence
					CE_Error( ip, " in string constant:\n", NULL,NULL);
					CE_SrcToError(ip, pnt+boff, _find_EOS(pnt,off)-boff, 40);
					goto TheEnd;
				}
				switch(tmp) {
					default :	*dst++ = (uChar)tmp;
					case -2 :	break;		// line-continuator
					case -1 :	goto	NoEnd;	// End Of Script
				}
				break;

			case '\n' :	// *** new-line
				CE_Error( ip, "newline in string constant:\n", NULL,NULL);
				CE_SrcToError(ip, pnt+boff, _find_EOS(pnt,off)-boff, 40);
				*err_f = LVTRUE;
				goto TheEnd;

			case '\0' :	// *** End Of Script
NoEnd:
				CE_Error( ip, "Unterminated string constant:\n", NULL,NULL);
				CE_SrcToError(ip, pnt+boff, off-boff, 40);
				CE_EOSToError(ip);
				off--;
				*err_f = LVTRUE;
				goto TheEnd;
		}
	}
EndLoop:
	// -------------------------------
	// Проверить на соседство констант
	// -------------------------------
	for(;;) {
		Bool32	comm;

		switch(c = *(pnt+off)) {
			case '\0' :			goto TheEnd;
			default :	if (c > ' ')	goto TheEnd;
					off++;
					break;
			case '\"' :	off++;		goto BegLoop;
			case '/' :
				comm = FALSE;
				tmp  = off;
				off = Comments(ip, pnt,off,&comm,err_f,errp);
				if (*err_f || *errp)
					goto TheEnd;
				if (!comm) {
					off = tmp;
					goto TheEnd;
				}
		}
	}
TheEnd:
	// Сохранить значение
	// ------------------
	ip->LOpd.type	= OPTYPE_STR;
	ip->LOpd.num	= 0;
	if (*errp = LStr_copy_n(LStrBuf(*buf), ip->LOpd.str, 0, (int32)(dst - LStrBuf(*buf))) )
		goto out;

	// Занести операнд в массив выражения
	// ----------------------------------
	*errp = ExAr_PutOperand(ip, &(ip->LOpd), expIdx);		// >>> #CE_3.0.4 2026-06-22
out:
	return (off);
}

// ====================================================

// -----------------------------------
// Looks for end of string constant
// Is only used to form error messages
// -----------------------------------
int32 _find_EOS( uChar *pnt, int32 off)
{
	for(;;) {
		switch (*(pnt+off)) {
			case '\0' :	return off;
			case '\"' :	return off+1;
			case '\\' :	off++;
					if (*(pnt+off) == '\0')
						return off;
		}
		off++;
	}
	return off;
}
