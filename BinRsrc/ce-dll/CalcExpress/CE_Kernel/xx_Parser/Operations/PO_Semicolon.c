/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =============================================
// 	входная операция ';'
// =============================================
// выталкивает все операции из стека в выражение
// сама в стек не помещается 
// =============================================
MgErr PO_Semicolon(	_CE_Instance	*ip,
			int32		*expIdx,
			_Operation	*op,
			Bool32		*subExp_begin,
			Bool32		*prev_operand,
			LVBoolean	*err_f)
{
	_Operation	*otp;		// temporary
	MgErr		err = noErr;

	if (*subExp_begin)
		goto out;	// пустой оператор
	*subExp_begin = TRUE;

	if (!(*prev_operand)) {
		err = CE_Error(ip, "Missing operand before ';'", NULL,NULL);
		*err_f = LVTRUE;
		goto out;
	}
	*prev_operand = FALSE;

	// ================================
	// Вытолкнуть из стека все операции
	// ================================
	for(;;) {
		otp = OpsStack_TopPnt(ip);	// операция в вершине стека
		if (!otp)  break;			// если стек пуст, то выход
		OpsStack_DecLen(ip);		// уменьшить размер стека

		switch(otp->category) {
			case CAT_OBRACKET:
				err = CE_Error(ip, "Missing ')' before ';'",	NULL,NULL);
				*err_f = LVTRUE;
				goto out;
			case CAT_CBRACKET:
				err = CE_Error(ip, "Unexpected ')' before ';'",	NULL,NULL);
				*err_f = LVTRUE;
				goto out;
			case CAT_OINDEX:
				err = CE_Error(ip, "Missing ']' before ';'",	NULL,NULL);
				*err_f = LVTRUE;
				goto out;
			case CAT_CINDEX:
				err = CE_Error(ip, "Unexpected ']' before ';'",	NULL,NULL);
				*err_f = LVTRUE;
				goto out;
			default:
				macro_PutToExpression(otp);
				break;
		}
	}
	macro_PutToExpression(op);		// заносим ';' в выражение
//	err = OpsStack_Push(ip, op);	// заносим ';' в стек
out:
	return	err;
}
