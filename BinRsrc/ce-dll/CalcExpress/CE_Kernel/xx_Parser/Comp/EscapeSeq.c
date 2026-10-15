/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

__inline Bool32	_is_octal(uChar	ch)
{
	return ((ch >= '0')&&(ch <= '7'));
}

int32 EscapeSeq(	_CE_Instance	*ip,
			uChar		*string,
			int32		*offset,
			LVBoolean	*err_f)
{
	register uChar	*pnt;
		 uChar	*eon, *beg;
		 int32	ch;
		 uChar	buf[8];

	pnt = string + *offset;
	switch(*pnt++) {

		case '\0' :	ch = -1; pnt--;	break;	// End Of Script
		case '\n' :	ch = -2;	break;	// line continuator
		// -------------
		case 'a' :	ch = '\a';	break;
		case 'b' :	ch = '\b';	break;
		case 'f' :	ch = '\f';	break;
		case 'n' :	ch = '\n';	break;
		case 'r' :	ch = '\r';	break;
		case 's' :	ch = ' ';	break;
		case 't' :	ch = '\t';	break;
		case 'v' :	ch = '\v';	break;
		case '?' :	ch = '?';	break;
		case '\\' :	ch = '\\';	break;
		case '\'' :	ch = '\'';	break;
		case '\"' :	ch = '\"';	break;
		// -------------
		case 'x' :
		case 'X' :
			// *** ASCII in Hexadecimal Representation
			if (!isxdigit(*pnt)) {
				pnt++;
				goto Invalid;
			}
			eon = buf;
			*eon++ = *pnt++; 
			if (isxdigit(*pnt))
				*eon++ = *pnt++;
			*eon = '\0';
			ch = strtol(buf, &eon, 16);
			break;

		default :
			if (!_is_octal(*(pnt-1)))
				goto Invalid;

			// *** ASCII in Octal Representation
			eon = buf;
			*eon++ = *(pnt-1);
			if (_is_octal(*pnt)) {
				*eon++ = *pnt++;
				if (_is_octal(*pnt))
					*eon++ = *pnt++;
			}
			*eon = 0;
			ch = strtol(buf, &eon, 8);
			break;

Invalid:		// *** Invalid Escape Sequence
			ch = -10;
			*err_f = LVTRUE;
			beg = string + *offset; if (*offset) beg-=1;
			CE_Error( ip, "Invalid escape-sequence \'",NULL,NULL);
			CE_SrcToError(ip, beg, (int32)(pnt-beg), 10);
			CE_Error( ip, "\'",NULL,NULL);
			break;
	}
	*offset = (int32)(pnt-string);
	return ch;
}
