/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Linking CSE Context with Caller context
// -----------------------------------

#include <extcode.h>

#include <celib\ceMemLib.h>

#include <CSE.h>
#include <CSE Parser.h>
#include <CSE_AccessContext.h>

enum {
	cmd_SavePath		= 0,
	cmd_Link			= 1,
	cmd_Delink			= 2,
	cmd_CallChainInfo	= 3
};

/*
typedef	struct {
	int32			CallLevel;
#if defined(_M_X64)
	int32		pad_1;		// >>> #CE_3.0 2024-12-11
#endif
	iPScrInstance	*Caller;
	iPScrInstance	*Callee;
} _CallChainInfo;
*/
typedef	struct {
	int32		CallLevel;
	LVRefNum	Caller_Refnum;
	LVRefNum	Callee_Refnum;
} _CallChainInfo;

MgErr	__cdecl	cseVILinkScripts(
					LVRefNum			*pRefNum,		// iPScrInstancePnt	*iCSE_Hdl,
					LVRefNum			*pCallerRefNum,	// iPScrInstancePnt	*iCaller_Hdl,
					int32				action,
					LStrHandle			ScriptName,
					LStrHandle			CallbackVIPath,
					int32				*CallerRunMode,
					_CallChainInfo		*ccInfo,
					int32				*ExecStatus )
{
	iPScrInstance	*iCSE;
	iPScrInstance	*iCaller		= NULL;
	Bool32			fCallerLocked	= FALSE;
	MgErr			err		= noErr;

	if (*ExecStatus != estSuccess)
		if (action != cmd_Delink)
			return noErr;

//	iCSE	= *iCSE_Hdl;
//	iCaller	= *iCaller_Hdl;

	// ***** Convert LVRefNum to CSE Instance Reference (CE 3.0)
	if ( !(iCSE = (iPScrInstance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// ***** Convert Caller LVRefNum to CSE Instance Reference (CE 3.0)
	if (*pCallerRefNum != __NotRefNum)
		iCaller = (iPScrInstance*)ceref_GetRefData(*pCallerRefNum, NULL);


	// --------------------------------------
	// *****  Захватить CSE Instance Context
	// --------------------------------------
	if (!CSE_Context_Lock(iCSE)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*ExecStatus = estInvalidRefNum;
		return noErr;
	}

	// --------------------------------------
	// *****  Execution
	// --------------------------------------

	// ***	Common parameters
	// ----------------------
	ccInfo->CallLevel	= iCSE->rtlCallLevel;
	if (iCSE->rtlCaller)
			ccInfo->Caller_Refnum	= ceref_GetRefnum((LVRefNumData)iCSE->rtlCaller, NULL);
	else	ccInfo->Caller_Refnum	= __NotRefNum;
	if (iCSE->rtlCallee)
			ccInfo->Callee_Refnum	= ceref_GetRefnum((LVRefNumData)iCSE->rtlCallee, NULL);
	else	ccInfo->Callee_Refnum	= __NotRefNum;

	if (err = LStr_Copy(ScriptName, iCSE->ScriptName))
		goto out;

	// ***	action specific parameters
	// -------------------------------
	switch(action) {

		case cmd_SavePath :
			// ***	save path in the actual context (not a caller's)
			err = LStr_Copy(iCSE->rtlCallbackVIPath, CallbackVIPath);
			break;

		case cmd_Link :
			// ***** check for NULL pointer (this code is added just to make silent the MS Code Analyzer (warning C6011)
			if (iCaller == NULL) {
				*ExecStatus = estInvalidRefNum;
				break;
			}
			// *****  Захватить Caller CSE Instance Context
			if ( fCallerLocked = (CSE_Context_Lock(iCaller) != NULL) ) {		// >>> #CE_4.0.0 2026-10-01

				// ***	Update link information
				iCSE->rtlCallLevel	= iCaller->rtlCallLevel + 1;
				iCSE->rtlCaller		= iCaller;
				iCSE->rtlCallee		= iCaller->rtlCallee;
				iCaller->rtlCallee	= iCSE;

				// ***	Returned params
				*CallerRunMode	= iCaller->rtlRunMode;
				err = LStr_Copy(CallbackVIPath, iCaller->rtlCallbackVIPath);
			}
			else {
				*ExecStatus = estInvalidRefNum;
			}
			break;


		case cmd_Delink :
			// ***	Delink even if an input error status !

			// ***** check for NULL pointer (this code is added just to make silent the MS Code Analyzer (warning C6011)
			if (iCaller == NULL) {
				*ExecStatus = estInvalidRefNum;
				break;
			}
			// ***	Захватить Caller CSE Instance Context
			if ( fCallerLocked = (CSE_Context_Lock(iCaller) != NULL) ) {
				if (iCaller->rtlCallee == iCSE) {
					iCSE->rtlCallLevel	= 0;
					iCSE->rtlCaller		= NULL;
					// !!! iCSE->rtCallee is reset while execution
					iCaller->rtlCallee	= iCSE->rtlCallee;	// NULL
				}
				else {
					*ExecStatus = estInvalidRefNum;
				}
				*CallerRunMode	= iCaller->rtlRunMode;
			}
			else {
				if (*ExecStatus == estSuccess)
					*ExecStatus = estInvalidRefNum;
			}
			break;


		case cmd_CallChainInfo :
			if (iCaller) {
				if ( fCallerLocked = (CSE_Context_Lock(iCaller) != NULL) ) {
					*CallerRunMode	= iCaller->rtlRunMode;
				}
			}
			err = LStr_Copy(CallbackVIPath, iCSE->rtlCallbackVIPath);
			break;
	}
out:
	// *****  Return LabVIEW Manager Errors as CSE Execution Status
	if (err) {
		*ExecStatus = err;
	}

	// *****  Освободить Caller CSE Instance Context
	if (fCallerLocked)
		CSE_Context_UnlockReliable(iCaller);

	// *****  Освободить CSE Instance Context
	CSE_Context_UnlockReliable(iCSE);

	return err;
}
