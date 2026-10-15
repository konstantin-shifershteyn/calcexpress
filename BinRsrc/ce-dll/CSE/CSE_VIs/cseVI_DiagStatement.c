/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Statement Diagnostic
// -----------------------------------

#include <extcode.h>

#include <CSE.h>
#include <CSE_AccessContext.h>

#include <celib\ceMemLib.h>

// ==========================================================

MgErr	__cdecl	cseVIDiagStm(	
				LVRefNum		*pRefNum,		// iPScrInstancePnt	*iCSE_Hdl,
				int32			StmIndex,
				int32			ItemID,
				LStrHandle		outString,
				LVBoolean		*fExpress,
				int32			*ExecStatus)
{
	iPScrInstance	*iCSE;
	iPScrElement	*pe;
	int32			arrSize;
	MgErr			err = noErr;

	LStr_Clear(outString);	// !!!
	*fExpress = LVFALSE;

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

	// --------------------------------------
	// *****  Execution
	// --------------------------------------

	arrSize = iPScrSize(*(iCSE->iPScr));
	if ((StmIndex < 0) || (StmIndex >= arrSize))
		goto out;

	pe = iPScrEPnt(*(iCSE->iPScr), StmIndex);

	if (pe->ip) {
		ItemID	= CSE_Diag_Convert_ItemID(ItemID);
		*fExpress = LVTRUE;
		err = CSE_List_Of_Item_Names( pe->ip, outString, ItemID );
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
