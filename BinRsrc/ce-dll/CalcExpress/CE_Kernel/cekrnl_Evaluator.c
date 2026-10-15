/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <iDataTypes.h>
#include <celib\cdb.h>

#include <Operand_Routines.h>

#include "xx_Evaluator\CalcOperator.h"

#include "xx_Evaluator\DataConversion\DataConversion.h"
#include "xx_Evaluator\Indexation\Indexation.h"

// ==================

#include "xx_Evaluator\Subr.c"
#include "xx_Evaluator\Subr_Variables.c"
#include "xx_Evaluator\Subr_Operands.c"
#include "xx_Evaluator\Subr_Operators.c"

#include "xx_Evaluator\CalcOperator.c"

#include "xx_Evaluator\DataConversion\OperandInfo.c"
#include "xx_Evaluator\DataConversion\NumConverts.c"
#include "xx_Evaluator\DataConversion\StrConverts.c"
#include "xx_Evaluator\DataConversion\LStrConverts.c"
#include "xx_Evaluator\DataConversion\IdxConverts.c"

#include "xx_Evaluator\Indexation\Indexation.c"
#include "xx_Evaluator\Indexation\IndexRange.c"

#include "xx_Evaluator\FastFunc\FastFunc.c"
#include "xx_Evaluator\CpxFunc.c"
#include "xx_Evaluator\FuncVI_Params.c"

//#include "xx_Evaluator\FunctionCall.c"


// =============================================================

MgErr	CEKernel_Evaluator(	_CE_Instance		*ip,
							_CEKernel_Par_Eval	*ParEval,
							LVBoolean			*err_f)
{
	_ExpresElement	*expData;
	_ExpresElement	*expEl;
	int32			expIdx;
	int32			expSize;
	_Operation		*ops;
	_Operand		*Lopd;
	int32			ffIdx;
	LVBoolean		errf = *err_f;
	MgErr			err = noErr;

	expIdx	= ParEval->io_ExpIndex;
	expSize	= ip->expArrLen;					// >>> #CE_3.0.4 2026-06-22
	expData	= ip->expArrDat;					// >>> #CE_3.0.4 2026-06-22

	ParEval->out_NotEnd			= FALSE;	// признак завершения (изменяется только для FuncVI)
	ParEval->out_NumOfFuncArgs	= 0;		// кол-во аргументов в вызове FuncVI


	// ### check input error status
	// ============================
	if (errf || ip->procPars.ErrorOfParsing) {
		errf = LVTRUE;
		goto out;
	}

	// ### when the first call (very beginning)
	// ========================================
	if (!expIdx) {
		// *** Initializing
		if (err = ExecPrepare( ip ))
			goto out;

		ip->CancelTag	= FALSE;	/* !!!!! */
//		ip->writerID	= 0;		// >>> #CE_3.0.7 2026-07-23
	}
	else if (expIdx < 0) {		// *****  Ошибка  *****
			errf = LVTRUE;
			err = CE_InternalError(ip, "Expression Index is less than zero", NULL,NULL);
			goto out;
	}

	ClearVarPointersIfChangedVCH(ip);	// >>> #CE_3.0.2 2026-06-19

	// ### EXECUTION LOOP ###
	// ======================
	while( expIdx < expSize) {
		expEl = &(expData[expIdx]);

		if ( expEl->IsOps ) {
			// ### the element of expression is an Operator
			ops = &(expEl->ops);

			switch (ops->category) {
				case CAT_SEMICOLON :
					Lopd = OpdStack_TopPnt( ip, &errf);
					OpdStack_DecLength( ip, &errf);
					if ((Lopd->type == OPTYPE_IDENT) && (Lopd->vpnt == NULL)) {	// >>> #CE_3.0.5 2025-07-15 : check only if variable was not evaluated
						OpdStack_VerifyOperand( ip, Lopd, &errf);
					}
					// >>> #CE_3.0.5 2025-07-15 : disabled: indexation is only used with OPTYPE_IDENT, but if we keep var name in the Operands Stack, we also must keep its indexation
					//_IndexInfo_ResetIndexation(Lopd);		// >>> #CE_3.0.4 2026-07-05
					break;
				case CAT_INDEX:
					if (err = IndexingOperator(ip, ops, &errf))
						goto out;
					break;
				case CAT_FASTFUNC :
					if (err = BuildInFunc( ip, ops->code, ops->n_ops, &errf))
						goto out;
					break;
				case CAT_FUNC :
					if ((ffIdx = _FastFunc_GetIndex(ip, ops->n_ops)) >= 0) {
					// ===== call FastFunc =====
						if (err = _FastFunc_IndirectCall(ip, ffIdx, ops->n_ops, &errf))
							goto out;
						break;
					}
					// ===== call FuncVI =====
					expIdx++;
					ParEval->out_NumOfFuncArgs = (int32)ops->n_ops;
					// *** prepare actual parameters to call FuncVI
					err = FuncVI_Params( ip, ParEval->out_NumOfFuncArgs, &errf);
					if (!(err || errf)) {
						ParEval->out_NotEnd = TRUE;
					}
					goto out;

				default :
					if (err = EvalOperator(ip, ops, &errf))
						goto out;
					break;
			}
		}
		else {
			// ### if element of expression is Operand, push it to the Operands Stack
			if (err = OpdStack_PUSH( ip, &(expEl->opd)))
				goto out;
		}
		// ### element of expression is processed
		expIdx++;
		if (errf || ip->CancelTag)
			break;
	}
	// ### expression evaluation has done
	if (errf)
		goto out;

	if (ip->CancelTag) {
		ip->CancelTag = FALSE;
		err = CE_Error(ip, "Processing is aborted (an abort signal has been received)", NULL,NULL);
		CE_Error_SetErrCode( ip, CE_ERR_ABORT, &errf);	//	errf = LVTRUE; it is set in the CE_Error_SetErrCode()
		goto out;
	}
	if (ip->OpdNum > 1) {
		err = CE_Error(ip, "Unprocessed operands remained (missing operator?)", NULL, NULL);
		errf = LVTRUE;
		goto out;
	}
out:
	ParEval->io_ExpIndex = expIdx;

	if (err) {
		CEKernel_Context_FreeMem(ip, FALSE);	// to free only memory allocated during execution
		CE_Error_LVMgErr( ip, err, &errf);		// save LabVIEW error code
	}
	if (errf) {
		*err_f = errf;
	}
	return err;
}
