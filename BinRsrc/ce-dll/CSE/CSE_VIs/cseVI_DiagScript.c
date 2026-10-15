/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Script Diagnostic
// -----------------------------------

#include <extcode.h>

#include <CSE.h>
#include <CSE_AccessContext.h>

// ==========================================================

MgErr	__cdecl	cseVIDiagScript(
				LVRefNum		*pRefNum,		// iPScrInstancePnt	*iCSE_Hdl,
				int32			ItemID,
				LStr1DArrayHdl	outArray,
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

	ItemID	= CSE_Diag_Convert_ItemID(ItemID);
	err = CSE_Array_Of_Lists_Of_Item_Names(iCSE, outArray, ItemID);
	// the array is clear if an error occurred

//out:
	// *****  Return LabVIEW Manager Errors as CSE Execution Status
	if (err) {
		*ExecStatus = err;
	}

	// *****  Освободить CSE Instance Context
	CSE_Context_UnlockReliable(iCSE);

	return err;
}
