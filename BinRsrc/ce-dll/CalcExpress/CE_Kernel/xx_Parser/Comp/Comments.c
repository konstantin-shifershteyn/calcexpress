/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// --------------------------------------
// Функция пропускает комментарии C и C++
// --------------------------------------
int32 Comments(
			 _CE_Instance	*ip,
		register uChar		*pnt,
		register int32		off,
			 Bool32		*comment,
			 LVBoolean	*err_f,
			 MgErr		*errp)
{
	int32	boff = off;
	int32	level;

	// Проверить - начало комментария ?
	// --------------------------------
	switch (*(pnt+off++)) {
		case '/' :
			switch (*(pnt+off)) {
				case '*' : goto c_com;
				case '/' : goto l_com;
			}
			break;
		case '*' :
			if (*(pnt+off) != '/')
				break;
			*err_f = LVTRUE;
			*errp  = CE_Error(ip, "Unexpected end-of-comment token (*/)",NULL,NULL);
			break;
		default :
			break;
	}
out:
	return off;

	// --------------------
	// Строчный комментарий
	// --------------------
l_com:
	*comment = TRUE;		// признак комментария
	while(TRUE)			// поиск конца строки
		switch (*(pnt + ++off)) {
			case '\n' :
			case '\0' : goto out;
		}
	goto out;

	// ---------------------------
	// Обычный комментарий /*...*/
	// ---------------------------
c_com:
	*comment = TRUE;		// признак комментария
	level = 1;			// уровень вложения комментария
	off++;
	while (TRUE) {
		switch (*(pnt+off++)) {
			case '/' :
				if (*(pnt+off)=='*') {
					off++; level++;
				}
				break;
			case '*' :
				if (*(pnt+off)=='/') {
					off++;
					if (--level <= 0)
						goto out;
				}
				break;
			case '\0':
				*errp  = CE_Error(ip, "Unterminated comment:\n",NULL,NULL);
				*errp  = CE_SrcToError(ip, pnt+boff, off-boff, 40);
				*errp  = CE_EOSToError(ip);
				*err_f = LVTRUE;
				off--;
				goto out;
			default:
				break;
		}
	}
	goto out;
}
