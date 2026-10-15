/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Get Statement Info
// -----------------------------------

#include <extcode.h>

#include <celib\ceMemLib.h>

#include <CSE.h>
#include <CSE Parser.h>
#include <CSE_AccessContext.h>

typedef struct {
	LVRefNum	CERefNum;		// _CE_Instance	*ip;		// pointer to CE Instance (NULL if expression is non-calculated)
	int32		jumpAction;
	int32		jumpIndex;
	int32		stmType;		// 0:expression, 1: jump; 2:srcReference, 3:jump-stub
	int32		NestingLevel;	// !!!
	// *** Links to statement in source script
	int32		stmOffset;		// offset of a statement involving the expression
	int32		stmLength;		// length of the statement (in bytes)
	int32		stmLine;		// line number where the statement started
	// *** Links to expression in source script
	int32		expOffset;		// offset of expression in a script (an index)
	int32		expLength;		// length of the expression (in bytes)
	int32		expLine;		// line number where the expression started
	// ---
	int32		parentIndex;
} _StmInfo;

//CSE_PRIVATEDATA
//_StmInfo	stmInfoStub	= {	NULL, JA_JUMP, -1, 0, 0,
//					-1, 0, -1, 	-1, 0, -1,	-1 };
CSE_PRIVATEDATA
_StmInfo	stmInfoStub	= {	__NotRefNum, JA_JUMP, -1, 0, 0,
					-1, 0, -1, 	-1, 0, -1,	-1 };


// =================================================================

MgErr	__cdecl	cseVIStmInfo(
				LVRefNum		*pRefNum,	// iPScrInstancePnt	*iCSE_Hdl,
				int32			StmIndex,
				_StmInfo		*stmInfo,
				LVBoolean		*fNeedScript,
				LStrHandle		Script,
				int32			*arrSize,
				int32			*ExecStatus )
{
	iPScrInstance	*iCSE;
	iPScrElement	*pe;
	MgErr			err = noErr;

	*arrSize = 0;
	*stmInfo = stmInfoStub;

	if (*ExecStatus != estSuccess)
		return noErr;

	// ***** Convert LVRefNum to CSE Instance Reference (CE 3.0)
	if ( !(iCSE = (iPScrInstance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

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

	// ***	Copy script if required
	if (*fNeedScript)
		if (err = LStr_Copy(Script, iCSE->Script))
			goto out;

	*arrSize = iPScrSize(*(iCSE->iPScr));
	if ((StmIndex < 0) || (StmIndex >= *arrSize))
		goto out;

	pe = iPScrEPnt(*(iCSE->iPScr), StmIndex);

//	stmInfo->ip				= pe->ip;
	stmInfo->CERefNum		= ceref_GetRefnum((LVRefNumData)(pe->ip), NULL);
	stmInfo->jumpAction		= pe->jumpAction;
	stmInfo->jumpIndex		= pe->jumpIndex;
	stmInfo->stmType		= pe->stmType;
	stmInfo->NestingLevel	= pe->NestingLevel;
	stmInfo->stmOffset		= pe->stmOffset;
	stmInfo->stmLength		= pe->stmLength;
	stmInfo->stmLine		= pe->stmLine;
	stmInfo->expOffset		= pe->expOffset;
	stmInfo->expLength		= pe->expLength;
	stmInfo->expLine		= pe->expLine;
	stmInfo->parentIndex	= pe->parentIndex;

out:
	// *****  Return LabVIEW Manager Errors as CSE Execution Status
	if (err) {
		*ExecStatus = err;
	}

	// *****  Освободить CSE Instance Context
	CSE_Context_UnlockReliable(iCSE);

	return err;
}
