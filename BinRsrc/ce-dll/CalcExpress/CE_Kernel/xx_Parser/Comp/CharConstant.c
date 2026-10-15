/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// тело в конце файла
int32 _find_EOC( uChar *pnt, int32 off);

//
int32 CharConst(_CE_Instance	*ip,
		uChar		*pnt,
		int32		off,
		int32		*expIdx,
		LVBoolean	*err_f,
		MgErr		*errp)
{
	_Operand	*opd;
	int32		val  = 0;
	int32		clen = 0;
	uChar		c;
	int32		tmp, boff = off-1;

	*errp = noErr;
	opd   = &(ip->LOpd);

	// -------------------
	// Определить значение
	// -------------------
	while (TRUE) {
		switch(c = *(pnt + off++)) {
			default :	clen++;
					val <<= 8;
					val |= 0x000000FF & (int32)c;
					break;

			case '\'' :	goto ex;

			case '\\' :
				tmp = EscapeSeq( ip, pnt, &off, err_f);
				if (*err_f) {		// invalid escape-sequence
					CE_Error( ip, " in character constant: ", NULL,NULL);
					CE_SrcToError(ip, pnt+boff, _find_EOC(pnt,off)-boff, 20);
					goto out;
				}
				switch(tmp) {
					default :	clen++;
							val <<= 8;
							val |= 0xFF & tmp;
					case -2 :	break;		// line-continuator
					case -1 :	goto	NoEnd;	// End Of Script
				}
				break;

			case '\n' :	// *** new-line
				CE_Error( ip, "newline in character constant:\n", NULL,NULL);
				CE_SrcToError(ip, pnt+boff, _find_EOC(pnt,off)-boff, 20);
				*err_f = LVTRUE;
				goto out;

			case '\0' :
NoEnd:
				*errp  = CE_Error( ip, "Unterminated character constant:\n", NULL,NULL);
				CE_SrcToError(ip, pnt+boff, _find_EOC(pnt,off)-boff, 20);
				*errp  = CE_EOSToError(ip);
				*err_f = LVTRUE;
				off--;
				goto out;
		}
		if (*err_f)	goto ex;
	}
ex:
	// ---------------
	// Подготовить opd
	// ---------------
	opd->type = OPTYPE_DBL;
	opd->num  = val;
	if (*errp = LStr_Clear(opd->str))
		goto out;

	// ------------------------
	// Занести в стек операндов
	// ------------------------
	if (*errp = ExAr_PutOperand(ip, opd, expIdx))		// >>> #CE_3.0.4 2026-06-22
		goto out;

	if (!clen) {
		*errp  = CE_Error( ip, "Empty character constant", NULL,NULL);
		*err_f = LVTRUE;
	}
	if (clen > 4) {
		*errp = CE_Error( ip, "Character constant too long: ", NULL,NULL);
		CE_SrcToError(ip, pnt+boff, _find_EOC(pnt,off)-boff, 20);
		*err_f = LVTRUE;
	}
out:
	return (off);
}

// ====================================================

// -----------------------------------
// Looks for end of character constant
// Is only used to form error messages
// -----------------------------------
int32 _find_EOC( uChar *pnt, int32 off)
{
	for(;;) {
		switch (*(pnt+off)) {
			case '\0' :	return off;
			case '\'' :	return off+1;
			case '\\' :	off++;
					if (*(pnt+off) == '\0')
						return off;
		}
		off++;
	}
	return off;
}
