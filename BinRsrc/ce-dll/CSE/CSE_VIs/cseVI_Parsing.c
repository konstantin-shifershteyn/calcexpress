/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Script Parser
// -----------------------------------

#include <extcode.h>

#include <ceKernel.h>
#include <CSE.h>
#include <CSE Parser.h>
#include <CSE_AccessContext.h>

// ==========================================================

MgErr	__cdecl	cseVIParse(	
				LVRefNum			*pRefNum,	// iPScrInstancePnt	*iCSE_Hdl,
				LStrHandle			Script,
				LStrHandle			ScriptName,
				LVBoolean			*fNeedFuncVIs,
				_VIParseParams		*params,
				LStr1DArrayHdl		ArrOfListFuncVIs,
				LVBoolean			*fDidParsing,
				int32				*ExecStatus)
{
	iPScrInstance	*iCSE;
	MgErr			err = noErr;


	if (*ExecStatus != estSuccess)
		return noErr;

	// ***** Convert LVRefNum to CSE Instance Reference (CE 3.0)
	if ( !(iCSE = (iPScrInstance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// *****  lock CSE Instance Context
	// --------------------------------------
	if (!CSE_Context_Lock(iCSE)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*ExecStatus = estInvalidRefNum;
		return noErr;
	}

	// =============================================
	// Step 1. Parse a CSE Script
	// =============================================
	err = CSE_ParseScript(iCSE, Script, ScriptName, params, fDidParsing, ExecStatus);
	if (err || (*ExecStatus != estSuccess))
		goto out;		// ArrayOfStatements is clear if an error occured

	// ============================================
	// Step 2. Build List of FuncVIs
	// ============================================
	if (*fNeedFuncVIs) {
		if (err = CSE_Array_Of_Lists_Of_Item_Names(iCSE, ArrOfListFuncVIs, CEK_MOD_Diag_FuncVI))
			goto out;	// the array is clear if an error occurred
	}

out:
	// *****  Return LabVIEW Manager Errors as CSE Execution Status
	if (err) {
		*ExecStatus = err;
		iPScr_ResizeArray(&(iCSE->iPScr), 0);	// clear parsed script instance
	}

	// *****  unlock CSE Instance Context
	CSE_Context_UnlockReliable(iCSE);

	return err;
}
