/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// Общий макрос, используется в подключаемых файлах
// >>> #CE_3.0.4 2026-06-22
#define	macro_PutToExpression(ops)		if (err = ExAr_PutOperation(ip, ops, expIdx)) goto out;
// <<< #CE_3.0.4 2026-06-22


// ###################################################

#include "PO_OBracket.c"
#include "PO_CBracket.c"
#include "PO_OIndex.c"
#include "PO_CIndex.c"
#include "PO_Comma.c"
#include "PO_Semicolon.c"
#include "PO_AtDelimiter.c"

// ###################################################

MgErr ProcOperation(_CE_Instance	*ip,
					int32			*expIdx,
					_Operation		*op )
{
	Bool32		exop;
	_Operation	*otp;
	MgErr		err = noErr;

	// *************************************
	//        Просмотр стека операций
	// *************************************
	for(;;) {
		otp = OpsStack_TopPnt(ip);	// операция в вершине стека
		if (!otp)					// если стек пуст, то поместить входную операцию в стек
			goto SaveOp;

		if (op->direction == DIR_LR)	exop = (op->prior <= otp->prior);
		else							exop = (op->prior <  otp->prior);
		if (exop) {
			OpsStack_DecLen(ip);		// уменьшить размер стека
			macro_PutToExpression(otp);	// перенести из стека в выражение
			continue;
		}
SaveOp:
		err = OpsStack_Push(ip, op);	// заносим операцию в стек
		break;
	}
/*
	if (op->code == OP2_IdxRange) {
		 int32		i;
		// проверить находится ли '..' внутри [ ]
		for (i=OpsStack_GetLen(ip); i>0; i--) {
			if (!(otp = OpsStack_GetPnt(ip, i)))
				break;
			if (otp->category == CAT_OBRACKET)	// нашли '('
				break;			// error
			if (otp->category == CAT_OINDEX)	// нашли '['
				goto out;			// все OK

		}
		err = CE_Error(ip, "Unexpected '..'. "
				"Index Range operator may only be used to form an array index"
				, NULL,NULL);
		*err_f = LVTRUE;
	}
*/
out:
	return err;
}

#undef	macro_PutToExpression
