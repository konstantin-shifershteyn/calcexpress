/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Get Indexes of Statements in the specified line
// -----------------------------------

#include <extcode.h>

#include <celib\ceMemLib.h>

#include <CSE.h>
#include <CSE Parser.h>
#include <CSE_AccessContext.h>


// Type describes array of Statement Indexes
// -----------------------------------------
typedef struct {
	int32		dimSize;
//#if defined(_M_X64)
//	int32		pad_1;		// >>> #CE_3.0 2024-12-11
//#endif
	int32		data[1];
} _StmIdxArray, **_StmIdxArrayHdl;

// =================================================================

MgErr	__cdecl	cseVIStmInLine(	
				LVRefNum			*pRefNum,	// iPScrInstancePnt	*iCSE_Hdl,
				int32				Line,
				_StmIdxArrayHdl		StmIdxArray,
				int32				*ExecStatus )
{
	iPScrInstance	*iCSE;
	iPScrElement	*pe;
	int32			*pntOut, stmNum;
	int32			step, size, StmIndex;
	MgErr			err = noErr;


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


	size = iPScrSize(*(iCSE->iPScr));

	for (step=1, stmNum = 0; step <= 2; step++) {
		for (StmIndex = 0; StmIndex < size; StmIndex++) {
			pe = iPScrEPnt(*(iCSE->iPScr), StmIndex);
			if (pe->stmLine > Line)			break;
			if (pe->stmLine < Line)			continue;
			if (pe->stmType > STMTYPE_SRCREF)	continue;

			if (step == 1)	stmNum++;
			else		*pntOut++ = StmIndex;

		}
		if (step == 1) {
			err = ce_1DArraySetSize((UHandle*)&StmIdxArray, stmNum, sizeof(int32));
			if (err) goto out;
			pntOut = &((*StmIdxArray)->data[0]);
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
