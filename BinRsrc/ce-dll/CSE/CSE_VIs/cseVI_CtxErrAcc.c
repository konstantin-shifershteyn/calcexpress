/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Access to Context Error Info
// -----------------------------------

#include <extcode.h>

#include <CSE.h>
#include <CSE_AccessContext.h>

#include <celib\ceMemLib.h>

#define	ACTION_READ	0
#define	ACTION_WRITE	1
#define	ACTION_RESET	2

// =================================================================

MgErr	__cdecl	cseVICtxErrAcc(	
				LVRefNum	*pRefNum,		// iPScrInstancePnt	*iCSE_Hdl,
				int32		*action,
				int32		*StmIndex,
				int32		*ErrCode,
				LStrHandle	ErrMessage,
				int32		*ExecStatus )
{
	iPScrInstance	*iCSE;
//	iPScrElement	*pe = NULL;
//	LStrHandle	StmSource = NULL;
	MgErr		err = noErr;


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

	switch (*action) {

		case ACTION_READ :
			*ErrCode	= iCSE->ErrCode;
			*StmIndex	= iCSE->ErrIndex;
			err = LStr_Copy(ErrMessage, iCSE->ErrMsg);
			break;

		case ACTION_WRITE :

			iCSE->ErrCode	= *ErrCode;

			if ( (*StmIndex >= 0) && (*StmIndex < iPScrSize(*(iCSE->iPScr))) ) {
				iCSE->ErrIndex	= *StmIndex;

				err = CSE_MakeErrorMessage(iCSE, *StmIndex, 0, TRUE, "%H", ErrMessage, NULL);

			//	pe = iPScrEPnt(*(iCSE->iPScr), *StmIndex);
			//	if (err = LStr_CreateFromMem(&StmSource, LStrBuf(*(iCSE->Script)) + pe->stmOffset, pe->stmLength))
			//		goto out;
			//	err = LStrPrintf(iCSE->ErrMsg, "line %d: %H\n" "in\n" "%H", pe->stmLine, ErrMessage, StmSource );
			//	LStr_Destroy(StmSource);
			}
			else {
				iCSE->ErrIndex	= 0;
				err = LStr_Copy(iCSE->ErrMsg, ErrMessage);
			}
			break;

		case ACTION_RESET :
			iCSE->ErrCode	= estSuccess;
			iCSE->ErrIndex	= -1;
			err = LStr_Clear(iCSE->ErrMsg);
			break;
	}
//out:

	// *****  Return LabVIEW Manager Errors as CSE Execution Status
	if (err) {
		*ExecStatus = err;
	}

	// *****  Освободить CSE Instance Context
	CSE_Context_UnlockReliable(iCSE);

	return err;
}
