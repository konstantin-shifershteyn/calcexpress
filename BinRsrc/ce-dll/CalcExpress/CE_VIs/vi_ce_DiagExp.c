/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Diagnostic
// -----------------------------------

#include <extcode.h>

#include <ceKernel.h>
#include <ceAccessContext.h>


// =================================================================
MgErr	__cdecl	viCEDiagExp(
				LVRefNum	*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				int32		*mode,
				LStrHandle	List,
				LVBoolean	*err_f)
{
	_CE_Instance		*ip;
	_CEKernel_Par_Diag	ParDiag;
	MgErr				err = noErr;

	if (*err_f)
		return noErr;

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
		err = LStrPrintf(List, "Invalid CE Instance Reference");
		return err;
	}

	ParDiag.in_Mode	= *mode;
	ParDiag.io_List	= List;


	err = cekrnl( CEK_CMD_Diag_Script, &ip, (void*)&ParDiag, NULL, err_f);

	// --------------------------
	// *****  Освободить контекст
	// --------------------------
	_CEContext_UnlockReliable(ip);
	return err;
}
