/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Parser
// -----------------------------------

#include <extcode.h>

#include <ceKernel.h>
#include <ceError.h>
#include <ceAccessContext.h>


// =================================================================

MgErr	__cdecl	viCEParse(
				LVRefNum			*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				LStrHandle			script,
				LVBoolean			*NeedFuncVIs,
				LStrHandle			ListFuncVIs,
				_CEVI_ParseParams	*params,
				LVBoolean			*err_f)
{
	_CE_Instance		*ip;
	_CEKernel_Par_Parse	ParParse;
	_CEKernel_Par_Diag	ParDiag;
	MgErr				err = noErr;


	// ***** Convert LVRefNum to Instance Reference (CE 3.0)
	if ( !(ip = (_CE_Instance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// -------------------------
	// *****  Захватить контекст
	// -------------------------
	if (!_CEContext_Lock(ip)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*err_f		= LVTRUE;
		return noErr;
	}

	if (*err_f) {
		CE_Error_Init(ip);
		err = CE_Error_AddPure(ip, "Input Error: error status passed to the CalcExpress Parser VI", NULL,NULL);
		goto out;
	}

	// --------------------------------------------------
	// *****  Вызвать CE Kernel для выполнения трансляции
	// --------------------------------------------------
	ParParse.in_Script			= script;
//	ParParse.in_enaNamedConst	= TRUE;							// >>> #CE_3.1.0-r6 2026-09-25
	ParParse.in_enaNamedConst	= params->fEnaNamedConst;		// >>> #CE_3.1.0-r6 2026-09-29

	err = cekrnl( CEK_CMD_Parse, &ip, &ParParse, NULL, err_f);
	if (err || *err_f)
		goto out;

	// -----------------------------------------------------
	// *****  Вызвать CE Kernel для получения списка FuncVIs
	// -----------------------------------------------------
	if (*NeedFuncVIs) {
		ParDiag.in_Mode	= CEK_MOD_Diag_FuncVI;
		ParDiag.io_List	= ListFuncVIs;
		err = cekrnl( CEK_CMD_Diag_Script, &ip, (void*)&ParDiag, NULL, err_f);
	}

out:
	if (err) {
		CE_Error_LVMgErr(ip, err, err_f);
	}
	// ----------------------------------
	// *****  Освободить контекст и выход
	// ----------------------------------
	_CEContext_UnlockReliable(ip);
	return err;
}
