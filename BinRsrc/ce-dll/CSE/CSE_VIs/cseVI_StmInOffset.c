/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Get Index of Statement at the specified offset
// -----------------------------------

#include <extcode.h>

#include <celib\ceMemLib.h>

#include <CSE.h>
#include <CSE_AccessContext.h>


// =================================================================

MgErr	__cdecl	cseVIStmInOffset(
					LVRefNum	*pRefNum,	// iPScrInstancePnt	*iCSE_Hdl,
					int32		Offset,
					int32		*pStmIndex,
					int32		*ExecStatus )
{
	iPScrInstance	*iCSE;
	iPScrElement	*pe;
	int32		size, StmIndex;
	int32		i, off, line;
	char		*pnt;
	MgErr		err = noErr;

	*pStmIndex = -1;

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

	if ((Offset < 0) || (Offset >= LStrLen(*(iCSE->Script))) )
		goto out;

	size = iPScrSize(*(iCSE->iPScr));
	if (size <= 0)
		goto out;

	// *** determine line number at Offset
	// --------------------------------------
	for (off = 0, line = 1, pnt = LStrBuf(*(iCSE->Script)); off < Offset; off++, pnt++)
		if (*pnt == '\n')
			line += 1;


	// *****  Execution
	// --------------------------------------
	for (StmIndex = 0; StmIndex < size; StmIndex++) {
		pe = iPScrEPnt(*(iCSE->iPScr), StmIndex);

		if (pe->stmOffset <= Offset) {
			if ((pe->stmOffset + pe->stmLength) > Offset) {
				if (pe->stmType <= STMTYPE_SRCREF) {
					*pStmIndex = StmIndex;
				}
			}
			continue;
		}
		// ***	A statement after Offset is achieved
		break;
	}

	if (*pStmIndex < 0) {
		// ***	If exact statement was not found

		if (StmIndex < size) {
			// ***	check current statement
			if ((pe->stmLine == line) && (pe->stmType <= STMTYPE_SRCREF)) {
				*pStmIndex = StmIndex;
				goto out;
			}
		}

		// ***	check previous statements
		for (off = i = 0; (--StmIndex >= 0) && (i < 10); i++) {
			pe = iPScrEPnt(*(iCSE->iPScr), StmIndex);
			if (pe->stmLine != line)
				break;
			if (pe->stmType <= STMTYPE_SRCREF) {
				if ((pe->stmOffset + pe->stmLength) > off) {
					off = pe->stmOffset + pe->stmLength;
					*pStmIndex = StmIndex;
				}
			}
		}
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
