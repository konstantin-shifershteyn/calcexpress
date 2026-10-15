/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


int32 NumConst(
			 _CE_Instance	*ip,
			uChar			*pnt,
			int32			off,
			int32			*expIdx,
			LVBoolean		*err_f,
			MgErr			*errp)
{
	_Operand	*opd;
	int32		len;
	Bool32		OctOrHex;
	uChar		*eon, *p;

	*errp = noErr;

	opd = &(ip->LOpd);

	// -------------------
	// Определить значение
	// -------------------
	if ((*(pnt+off)=='0')&&(*(pnt+off+1)!='.'))	{
//		OctOrHex = TRUE;	opd->num = strtoul(pnt+off, &eon, 0);
		OctOrHex = TRUE;	opd->num = (float64)(int32)strtoul(pnt+off, &eon, 0);	// >>> #CE_3.0  2025-07-01 : integer constants are "signed 32-bit integers"
	}
	else {
		OctOrHex = FALSE;	opd->num = strtod (pnt+off, &eon);
	}
	// ------------------
	// Проверка на ошибку
	// ------------------
	switch( CharType(*eon) ) {
		case 0:		// operator or white space
		case 4:		// \"
		case 5:		// \'
			break;
//		case 3:		// dot (point)
//			if (*(eon-1) == '.') { eon-=1;	break;	}
//			if (*(eon+1) == '.') {		break;	}
		default:	// --
			if (OctOrHex) {
				switch (*(pnt+off+1)) {
				case 'x' :
				case 'X' :	p = "Hexadecimal";	break;
				default  :	p = "Octal";		break;
				}
			}
			else	p = "Decimal";
			// determine text of constant
			while(CharType(*eon)) {
				if ((*eon == '\"') || (*eon == '\''))	break;
//				if (!StrNCmp(eon, "..", 2))		break;
				eon++;
			}
			len = (int32)(eon - (pnt + off));
			*err_f = LVTRUE;
			CE_Error( ip, "Invalid %s constant \'", (LStrHandle)p, NULL);
			CE_SrcToError(ip, pnt+off, len, len);	// add text to error log
			CE_Error( ip, "\'", NULL, NULL);
	}
	// ---------------
	// Подготовить opd
	// ---------------
	opd->type = OPTYPE_DBL;
	if (*errp = LStr_Clear(opd->str))
		goto out;

	// ------------------------
	// Занести в стек операндов
	// ------------------------
	*errp = ExAr_PutOperand(ip, opd, expIdx);		// >>> #CE_3.0.4 2026-06-22
out:
	return (int32)(eon-pnt);
}
