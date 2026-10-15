/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr	CEKernel_Context_FreeMem(_CE_Instance *ip, Bool32 DestroyParsedScript)
{
//	MgErr		err = noErr;

	if (__CECheckReference(ip))
		goto out;

	// временные операнды
	// ============================
	__operand_decrease(&(ip->LOpd));
	__operand_decrease(&(ip->ROpd));

	// Стек операндов
	// ==============
	ip->OpdNum = 0;
	OpdStack_ActOperands(ip, opdsCmd_FreeMem, 0, OpdStack_Size(ip));	// free operands' data
	OpdStack_ResizeStack(ip, OPDSTACK_BLKSIZE);			// decrease stack size

	// стек операций
	// =======================
	ip->OpsNum = 0;
	OpsStack_ResizeStack(ip, OPSSTACK_BLKSIZE);			// decrease stack size

	// стек функций
	// ======================
	ip->FIONum = 0;
	FIOStack_ResizeStack(ip, FIOSTACK_BLKSIZE);			// decrease stack size

	if (DestroyParsedScript) {
		// массив элементов выражения
		// ===================================
		ExAr_Resize(ip, 0);			// >>> #CE_3.0.4 2026-06-22		// parsed script

		// очистить строки
		// ================
		LStr_Clear(ip->str);		// input script
		LStr_Clear(ip->ErrMsg);		// error message
	}
out:
	return noErr;
}
