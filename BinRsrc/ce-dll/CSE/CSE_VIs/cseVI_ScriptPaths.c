/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Script Paths manager
// -----------------------------------

#include <extcode.h>

#include <celib\ceMemLib.h>

#include <CSE.h>
#include <CSE_AccessContext.h>


enum {
	cmdGet	= 0,
	cmdSave = 1
};
// ==========================================================

MgErr	__cdecl	cseVIScriptPaths(	
					LVRefNum		*pRefNum,	// iPScrInstancePnt	*iCSE_Hdl,
					int32			action,
					LStrHandle		ScriptPath,
					LStrHandle		SearchPaths,
					int32			*ExecStatus)
{
	iPScrInstance	*iCSE;
	MgErr			err = noErr;


	if (*ExecStatus != estSuccess)
		return noErr;

	// ***** Convert LVRefNum to CSE Instance Reference (CE 3.0)
	if ( !(iCSE = (iPScrInstance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// *****  Захватить CSE Instance Context
	// -------------------------------------
	if (!CSE_Context_Lock(iCSE)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*ExecStatus = estInvalidRefNum;
		return noErr;
	}

	switch(action) {

		case cmdGet :
			if (err = LStr_Copy(ScriptPath,  iCSE->rtlScriptPath))	goto out;
			if (err = LStr_Copy(SearchPaths, iCSE->rtlSearchPaths))	goto out;
			break;

		case cmdSave :
			if (err = LStr_Copy(iCSE->rtlScriptPath,  ScriptPath))	goto out;
			if (err = LStr_Copy(iCSE->rtlSearchPaths, SearchPaths))	goto out;
			break;
	}

out:
	// *****  Return LabVIEW Manager Errors as CSE Execution Status
	if (err) {
		*ExecStatus = err;
	}

	// *****  Освободить CSE Instance Context
	CSE_Context_UnlockReliable(iCSE);

	return err;
}
