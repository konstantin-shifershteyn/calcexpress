/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Execute CSE Script
// -----------------------------------

#include <extcode.h>


#include <ceKernel.h>
#include <ceExpResult.h>
#include <ceError.h>

#include <CSE.h>
#include <CSE_AccessContext.h>

// =================================================================
//			TYPE DEFINITIONS
// =================================================================

// ----------------------------------------
// ===	Type describes execution parameters
// ----------------------------------------
typedef	struct {
	int32		pause_cnt;		// repeat counter
	int32		pause_ms;		// delay in ms (not used here)
	LVBoolean	autoVar;		// Enable auto creation of variables
	LVBoolean	en_cpx;			// Enable complex arithmetic
} _ExecParams;

// -------------------------------------------------------
// ===	Type describes array of break-points (Stn Indeces)
// -------------------------------------------------------
typedef struct {
	int32		dimSize;
//#if defined(_M_X64)
//	int32		pad_1;		// >>> #CE_3.0 2024-12-11
//#endif
	int32		data[1];
} _BPArray, **_BPArrayHdl;

// -----------------------------
// ===	Enum specifies Run Modes
// -----------------------------
enum {
	modeExecute	= 0,	// Regular mode
	modeStop	= 1,	// Abort execution
	modeRun		= 2,	// This and follows are debugging modes
	modeStepInto,
	modeStepOver,
	modeStepOut,
	//--
	modeRunLM,
	modeStepIntoLM,
	modeStepOverLM,
	modeStepOutLM
};

// ------------------------------------------------
// ===	Enum specifies Stop Reason (debugging mode)
// ------------------------------------------------
enum {
	stopFirstCall = 0,	// Is only used in a Callback VI
	stopActionCompleted,
	stopPauseToBreath
};

// =================================================================
// 		The function tests for breakpoints
// =================================================================
Bool32	TestForBreakPoint(
				int32		StmIndex,
				_BPArrayHdl	bps )
{
	int32	size, *pnt;
	if ( size = (*bps)->dimSize )
		for ( pnt = &((*bps)->data[0]); size; size--)
			if (*pnt++ == StmIndex)
				return TRUE;
	return FALSE;
}

// =================================================================
//		SCRIPT EXECUTOR (MAIN FUNCTION)
// =================================================================
MgErr	__cdecl	cseVIExecute(
				LVRefNum		*pRefNum,		// iPScrInstancePnt	*iCSE_Hdl,
				LVRefNum		*pCERefNum,		//_CE_InstancePnt		*CEInstanceRef,
				int32			*expVCHandle,
				_ExecParams		*execParams,
				int32			*expIdx,
				int32			*expNumOfArgs,
				LVBoolean		*expFuncVI,
				float64			*ReturnValue,
				int32			*ExecStatus,
				int32			*StatementIndex,
				int32			*iRunMode_oStopReason,
				_BPArrayHdl		BreakPoints,
				LVBoolean		*fFirstStep,
				LVBoolean		*STOP	)
{
	iPScrInstance		*iCSE;
	iPScrElement		*pe	= NULL;		// required
	iPScrElement		*peArrPnt;		// >>> #CE_3.0.5 2026-07-16
	int32				peArrSize;
	_CEKernel_Par_Eval	ParEval;
	int32				RunMode;
	int32				StmIndex;
	float64				ExpressValue;
	uInt32				ExpressType;
	int32				RepeatCounter;
	LVBoolean			errf = LVFALSE;
	MgErr				err	= noErr;
// -------------------------------------


	// ---------------------------------
	// ***	Initialize working variables
	// ---------------------------------
	RunMode			= *iRunMode_oStopReason;
	StmIndex		= *StatementIndex;
	RepeatCounter	= execParams->pause_cnt;

	// -----------------------
	// ***	Initialize outputs
	// -----------------------
//	*CEInstanceRef		= NULL;
	*pCERefNum			= __NotRefNum;
	*ReturnValue		= 0.0;
	*expNumOfArgs		= 0;
//		!!! FORBIDDEN HERE !!!		*expFuncVI		= LVFALSE;
	*STOP			= LVFALSE;
	*iRunMode_oStopReason	= stopActionCompleted;


	// ***** Convert LVRefNum to CSE Instance Reference (CE 3.0)
	if ( !(iCSE = (iPScrInstance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// ---------------------------------------
	// ***	Захватить CSE Instance Context
	// ---------------------------------------
	if (!CSE_Context_Lock(iCSE)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*StatementIndex	= -1;
		*expFuncVI	= LVFALSE;
		*ExecStatus	= estInvalidRefNum;
		*STOP		= LVTRUE;
		return noErr;
	}
	peArrSize = iPScrSize(*(iCSE->iPScr));	// кол-во statements

	// ************************
	iCSE->rtlRunMode = RunMode;
	iCSE->rtlCallee  = NULL;		// pointer to a called CSE instance
	// ************************

	// ---------------------------------------------
	// ***	Exit if script was not parsed completely
	// ---------------------------------------------
	if ( !(iCSE->fScriptParsed)) {
		*ExecStatus	= iCSE->ErrCode;
		goto out;
	}
	// -------------------------------------------------
	// ***	Exit if error occurred during call of FuncVI
	// -------------------------------------------------
	if (*ExecStatus != estSuccess) {
		// ***	pe is NULL, it will be validated later if required
		goto out;
	}

	// =================================================
	// ###	Initializing on the first call
	// =================================================
	if (*fFirstStep && !(*StatementIndex) && !(*expIdx)) {
		iCSE->CancelTag	= FALSE;	// Cancel trigger
		// ***	Error info
		iCSE->ErrCode	= estSuccess;
		iCSE->ErrIndex	= -1;
		LStr_Clear(iCSE->ErrMsg);
	}

	// ----------------------------------------
	// ***  Подготовить параметры для CE Kernel
	// ----------------------------------------
	ParEval.SetInPars	= TRUE;
	ParEval.in_VCHandle	= *expVCHandle;
	ParEval.in_AutoVar	= execParams->autoVar;
	ParEval.in_EnaCpx	= execParams->en_cpx;
	ParEval.io_ExpIndex	= *expIdx;

	// ----------------------------------------
	// ***	Validate StmIndex !!!
	// ----------------------------------------
	if ((StmIndex < 0) || (StmIndex >= peArrSize)) {
		// ***	pe equals NULL here, it will be validated later if required
		goto EndOfScript;
	}

	peArrPnt = iPScrBuff(*(iCSE->iPScr));	// >>> #CE_3.0.5 2026-07-16

	// ------------------------------------------------------
	// ###	Resume evaluating expression after call of FuncVI
	// ------------------------------------------------------
	if (*expFuncVI) {
		pe = &peArrPnt[StmIndex];			// >>> #CE_3.0.5 2026-07-16
		*expFuncVI = LVFALSE;	// ??? does not required
		goto	EvalExpression;
	}

start:
	// ======================================================
	// ###	FIND EXECUTABLE STATEMENT
	// ======================================================
	while ((StmIndex >= 0) && (StmIndex < peArrSize)) {
		pe = &peArrPnt[StmIndex];			// >>> #CE_3.0.5 2026-07-16

		if (RunMode == modeExecute) {
			if (pe->stmType <= STMTYPE_JUMP)
				goto ExecStatement;
		}
		else {
			if (pe->stmType <= STMTYPE_SRCREF)
				goto ExecStatement;
		}
		StmIndex = pe->jumpIndex;
	}
EndOfScript:
	StmIndex	= -1;
	*ExecStatus	= estNormalTermination;
	goto out;


ExecStatement:
	// ==========================================================
	// ***	We are here only for STMTYPE_EXPRESS and STMTYPE_JUMP
	// ==========================================================
	// HERE: ParEval.io_ExpIndex == 0
	// and   StmIndex is updated
	// ==========================================================
	if (RunMode != modeExecute) {
		// --------------------------
		// ***	One of Debugging mode. Test for stop condition
		// --------------------------
		if (*fFirstStep) {
			iCSE->DbgStmLine = -10;	// *** non-line mode (any impossible value)
			switch(RunMode) {
				case modeStop :			goto ScriptAborted;
				case modeRunLM :		iCSE->DbgStmLine		= pe->stmLine;
				case modeRun :			break;
				case modeStepIntoLM :	iCSE->DbgStmLine		= pe->stmLine;
				case modeStepInto :		break;
				case modeStepOverLM :	iCSE->DbgStmLine		= pe->stmLine;
				case modeStepOver :		iCSE->DbgNestingLevel	= pe->NestingLevel;
										break;
				case modeStepOutLM :	iCSE->DbgStmLine		= pe->stmLine;
				case modeStepOut :		iCSE->DbgNestingLevel	= pe->NestingLevel;
										break;
			}
			*fFirstStep = FALSE;
		}
		else {
			// ***	check line mode condition
			if (pe->stmLine != iCSE->DbgStmLine) {
				// ---
				// *** Either non-line mode or statement is in new line
				// ---
				iCSE->DbgStmLine = -10;	// *** sign: new line was met (any impossible value)

				// ***	Test for breakpoints
				if (TestForBreakPoint(StmIndex, BreakPoints)) {
					*STOP = LVTRUE;	goto out;
				}
				// ***	Mode dependent actions
				switch(RunMode) {
					case modeStop :		goto ScriptAborted;

					case modeRunLM :
					case modeRun :		break;

					case modeStepIntoLM :
					case modeStepInto :	*STOP = LVTRUE;	goto out;

					case modeStepOverLM :
					case modeStepOver :
						if (pe->NestingLevel <= iCSE->DbgNestingLevel) {
							*STOP = LVTRUE;	goto out;
						}
						break;
					case modeStepOutLM :
					case modeStepOut :
						if (pe->NestingLevel < iCSE->DbgNestingLevel) {
							*STOP = LVTRUE;	goto out;
						}
						break;
				}
			}
		}
	}
	// ---------------------------------
	// ###  Test for the 'Cancel' signal
	// ---------------------------------
	if (iCSE->CancelTag) {
ScriptAborted:
		iCSE->CancelTag	= FALSE;		// reset
		*ExecStatus	= estCancelled;
		goto out;
	}
	// ---------------------------------
	// ###	Test the 'Repeat' counter
	// ---------------------------------
	if (--RepeatCounter < 0) {
		if (RunMode)			// if debugging mode
			*STOP = LVTRUE;		// stop in order to call Callback VI
		*iRunMode_oStopReason = stopPauseToBreath;
		goto out;
	}

	// -------------------------------------------------------------
	// ###	Perform a STMTYPE_JUMP statement:
	// ###	break, continue, goto, return; ('return' w/o expression)
	// -------------------------------------------------------------
	if (pe->stmType != STMTYPE_EXPRESS) {
		StmIndex = pe->jumpIndex;
		goto start;
	}

EvalExpression:
	// ===============================================
	// ###  EVALUATE EXPRESSION (EXPRESSION-STATEMENT)
	// ===============================================
	pe->ip->VCHandle		= ParEval.in_VCHandle;
	pe->ip->procPars.autoVar= ParEval.in_AutoVar ? BitTRUE : BitFALSE;
	pe->ip->procPars.en_cpx	= ParEval.in_EnaCpx  ? BitTRUE : BitFALSE;
	pe->ip->writerID		= iCSE->writerID;		// >>> #CE_3.0.7 2026-07-25
	err = CEKernel_Evaluator(pe->ip, &ParEval, &errf);
	if (err || errf) {
		*ExecStatus	= estRuntimeError;
		goto out;
	}
	iCSE->writerID = pe->ip->writerID;				// >>> #CE_3.0.7 2026-07-25

	// ***	assign and check a flag of FuncVI call
	if (*expFuncVI = (LVBoolean)ParEval.out_NotEnd)	{
		// ----------------------------------------------
		// ***	Evaluation of expression is not completed
		// ***	it is required to call a FuncVI function
		// ----------------------------------------------
//		*CEInstanceRef	= pe->ip;		// CE Instance Context Reference
		*pCERefNum		= ceref_GetRefnum((LVRefNumData)(pe->ip), NULL);	
		*expIdx			= ParEval.io_ExpIndex;
		*expNumOfArgs	= ParEval.out_NumOfFuncArgs;
//		*expFuncVI		= LVTRUE;
		goto out;
	}
	// ***	expression was evaluated completely
	ParEval.io_ExpIndex	= 0;		// index within expression for next CE call

	// *** if it is just an expression-statement
	if (pe->jumpAction == JA_JUMP) {
		StmIndex = pe->jumpIndex;
		goto	start;
//		goto	EndOfStat;
	}

	// ### come here for the 'return' and conditional jumps
	// *** take a value of the expression
	errf = LVFALSE;
	err = ceExpResult( pe->ip, &ExpressValue, NULL, &ExpressType, &errf);
	if (err || errf) {
		*ExecStatus = estRuntimeError;
		goto out;
	}
	// =====================================
	// *** The 'return' statement. do exit
	// =====================================
	if (pe->jumpAction == JA_STOP) {
		*ReturnValue	= ExpressValue;
		goto EndOfScript;
	}
	// =====================================
	// ***	Conditional jumps
	// =====================================
	if (ExpressType != OPTYPE_DBL) {
		err = CE_Error(pe->ip, "Non-scalar conditional expression", NULL, NULL);
		*ExecStatus = estRuntimeError;
		goto out;
	}
	// *** determine indext of the next statement
	StmIndex++;
	if (pe->jumpAction == JA_JUMP_IFF) {	// *** Jump if FALSE
		if ( ! _DblToI32(ExpressValue) )
			StmIndex = pe->jumpIndex;
	}
	else {									// *** Jump if TRUE
		if ( _DblToI32(ExpressValue) )
			StmIndex = pe->jumpIndex;
	}
	goto start;
//EndOfStat:
//	ParEval.io_ExpIndex	= 0;		// index within expression
//	*expFuncVI		= LVFALSE;	// flag of FuncVI call
//	goto start;


out:
	// ======================================================
	// ***	EXIT (with keeping error info, if any)
	// ======================================================

	if (err) {
		// ***	return LabVIEW Manager Errors as CSE Execution Status
		*ExecStatus = err;
		iPScr_MinElements_RTData(iCSE->iPScr);	// minimize memory allocated during execution
	}
	iCSE->ErrCode	= *ExecStatus;
	switch(*ExecStatus) {

		case	estSuccess :
				iCSE->ErrIndex	= -1;
				break;

		case	estRuntimeError :
		case	CE_ERR_FUNCVI_NOTFOUND :
		case	CE_ERR_FUNCVI_INVALID :
				iCSE->ErrIndex	= StmIndex;
				// ***	pe can be NULL if error was passed from outside
				if (!pe) {
					if ((StmIndex < 0) || (StmIndex >= peArrSize)) {
						err = CSE_MakeErrorMessage(iCSE, iCSE->ErrIndex, 0, TRUE,
							"Unexpected Error: StmIndex is out of range (=%d)", (void*)(size_t)StmIndex, NULL);
						StmIndex = -1;
						break;
					}
					pe = iPScrEPnt(*(iCSE->iPScr), StmIndex);
				}
				err = CSE_MakeErrorMessage(iCSE, iCSE->ErrIndex, 0, TRUE, "%H", pe->ip->ErrMsg, NULL);
				StmIndex = -1;
				break;

		case	estCancelled :
				iCSE->ErrIndex	= StmIndex;
				// ***	pe is not used here
				err = CSE_MakeErrorMessage(iCSE, iCSE->ErrIndex, estCancelled, TRUE, "", NULL, NULL);
				StmIndex = -1;
				break;

		case	estNormalTermination :
		default:	StmIndex = -1;
				break;
	}

	// !!! Returned values !!! Here
	*expIdx		= ParEval.io_ExpIndex;
	*StatementIndex	= StmIndex;

	if (*ExecStatus) {		// (*ExecStatus || (StmIndex < 0))
		*STOP	= LVTRUE;
	}

	// ------------------------------------
	// ***	Освободить CSE Instance Context
	// ------------------------------------
	CSE_Context_UnlockReliable(iCSE);

	return err;
}

