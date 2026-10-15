/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Evaluator
// -----------------------------------

#include <extcode.h>

#include <ceKernel.h>
#include <ceError.h>
#include <ceExpResult.h>
#include <ceAccessContext.h>


// =================================================================

MgErr	__cdecl	viCEEval(
				LVRefNum			*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				int32				*VCHandle,
				int32				*Index,
				_CEVI_CalcParams	*CalcPars,
				int32				*OpdNum,
				LVBoolean			*NotEnd,
				float64				*num,
				uInt32				*type,
				LVBoolean			*err_f,
				int32				*RepeatCounter )
{
	_CE_Instance		*ip;
	_CEKernel_Par_Eval	ParEval;
	MgErr				err = noErr;

	if (*RepeatCounter <= 0) {
QuickOut:	// *err_f не изменяем !!!
		*num	= 0.0;
		*type	= OPTYPE_DBL;
		*Index	= -1;
		*OpdNum	= 0;
		*NotEnd	= LVFALSE;
		return noErr;
	}

	// ***** Convert LVRefNum to Instance Reference (CE 3.0)
	if ( !(ip = (_CE_Instance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// -------------------------
	// *****  Захватить контекст
	// -------------------------
	if (!_CEContext_Lock(ip)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*err_f		= LVTRUE;
		goto QuickOut;
	}

	// writerID can be overridden while execution by calling a function
	//ip->writerID = CalcPars->writerID;		// >>> #CE_3.0.7 2026-07-25

	do {
		// ------------------------
		// *****  Вызвать CE Kernel
		// ------------------------
		ParEval.SetInPars	= TRUE;
		ParEval.in_VCHandle	= *VCHandle;
		ParEval.in_AutoVar	= CalcPars->autoVar;
		ParEval.in_EnaCpx	= CalcPars->en_cpx;
		ParEval.io_ExpIndex	= *Index;

		err = cekrnl( CEK_CMD_Exec_Script, &ip, NULL, &ParEval, err_f);

		*Index		= ParEval.io_ExpIndex;
		*OpdNum		= ParEval.out_NumOfFuncArgs;
		*NotEnd		= (LVBoolean)ParEval.out_NotEnd;

		if (*NotEnd || *err_f || err) {
			*num	= 0.0;
			*type	= OPTYPE_DBL;
			goto out;
		}
		// *****  To perform evaluation again (from the beginning of script)
		*Index = 0;
	} while (--(*RepeatCounter) > 0);

	// ---------------------------------------------------
	// *****  Вызвать CE Result когда выполнение закончено
	// ---------------------------------------------------
	err = ceExpResult( ip, num, NULL, type, err_f);

out:
	if (err) {
		CE_Error_LVMgErr(ip, err, err_f);
	}

	// --------------------------
	// *****  Освободить контекст
	// --------------------------
	_CEContext_UnlockReliable(ip);

	return err;
}
