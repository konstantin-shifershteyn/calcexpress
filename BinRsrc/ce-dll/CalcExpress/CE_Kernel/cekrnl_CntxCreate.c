/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr	CEKernel_Context_Create(_CE_InstancePnt *InstanceRef)
{
	_CE_Instance	*ip;
	MgErr		err = noErr;

	// ----------

	ip = *InstanceRef = (_CE_Instance*)ce_DSNewPClr( sizeof(_CE_Instance) );
	if (!ip) {
		err = mFullErr; goto out;
	}

	ip->SyncTag		= 0;
	ip->CancelTag	= 0;
	ip->VCHandle	= 0;
	ip->oldVCH		= 0;						// >>> #CE_3.0.2 2026-06-19
	ip->vmChangeCnt	= VM_HardChangeCounter();	// >>> #CE_3.0.2 2026-06-20
	ip->writerID	= 0;						// >>> #CE_3.0.7 2026-07-23

	ip->procPars.ParsingInProgress	= BitFALSE;
	ip->procPars.ErrorOfParsing		= BitFALSE;
	ip->procPars.testSyntaxOnly		= BitFALSE;
	ip->procPars.enaNamedConst		= BitTRUE;

	// !!!!!!!!!!!!!!!!

	// Создать массив элементов выражения
	// ==================================
	if ( err = ExAr_Create(ip) )
		goto out;

	// Создать handle входной строки
	// =============================
	if (err = LStr_Create(&(ip->str), 0))
		goto out;

	// Создать handle строки сообщения об ошибке
	// =========================================
	if (err = LStr_Create(&(ip->ErrMsg), 0))
		goto out;

	// Инициализация стека операндов
	// =============================
	ip->OpdStk = NULL;
	ip->OpdNum = 0;
	if (err= OpdStack_CreateStack(ip) )
		goto out;

	// Инициализация стека функций
	// ===========================
	ip->FIOStk = NULL;
	ip->FIONum = 0;
	if (err= FIOStack_CreateStack(ip) )
		goto out;

	// Инициализация стека операций
	// ============================
	ip->OpsStk = NULL;
	ip->OpsNum = 0;
	if (err= OpsStack_CreateStack(ip) )
		goto out;

	// Инициализация временных операндов
	// =================================
	if ( err = __operand_prepare(&(ip->LOpd)) )
		goto out;
	if ( err = __operand_prepare(&(ip->ROpd)) )
		goto out;

	// ==============================
	// *** Установить signature field
	// ==============================
	ip->signature = CE_CONTEXT_SIGNATURE;
out:
	// ======================
	// create LabVIEW RefNum
	// ======================
	if (ip)	{
		LVRefNum	refnum;
		MgErr		err = noErr;
		refnum = ceref_Create((LVRefNumData)(ip),  cerefKind_CEInstance,  &err);
	}
	return err;
}
