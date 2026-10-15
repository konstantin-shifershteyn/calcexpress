/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Complete Calculation: Parsing -> Evaluation -> Result


#include <extcode.h>

#include <ceKernel.h>
#include <ceError.h>
#include <ceAccessContext.h>
#include <ceExpResult.h>

#include <celib\ceMemLib.h>

// =================================================================



// =================================================================

MgErr	__cdecl	viCEComp(
				int32				*VCHandle,
				LStrHandle			script,
				int32				*Index,
				_CEVI_CalcParams	*CalcPars,
				LVRefNum			*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				int32				*OpdNum,
				LVBoolean			*NotEnd,

				float64				*num,
				LStrHandle			str,
				uInt32				*type,

				LVBoolean			*err_f,
				_CEVI_ParseParams	*ParsePars,
				int32				*RepeatCounter	)
{
	_CE_Instance		*ip;
	int32				cmd;
	_CEKernel_Par_Eval	ParEval;
	_CEKernel_Par_Parse	ParParse;
	MgErr				err = noErr;

//DbgPrintf("viCEComp(): RepeatCounter=%d. str is %s\n", *RepeatCounter, str == NULL ? "NULL" : "DEFINED");

	if (*RepeatCounter <= 0) {
QuickOut:	// *err_f не изменяем !!!
		*num	= 0.0;
		*type	= OPTYPE_DBL;
		if (str && *str) {
			LStr_Clear(str);
		}
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
		*err_f	= LVTRUE;
		goto QuickOut;
	}

	// ------------------------------------------
	// *****  Подготовить параметры для CE Kernel
	// ------------------------------------------
	ParParse.in_Script			= script;
//	ParParse.in_enaNamedConst	= TRUE;							// >>> #CE_3.1.0-r6 2026-09-25
	ParParse.in_enaNamedConst	= ParsePars->fEnaNamedConst;	// >>> #CE_3.1.0-r6 2026-09-29

	ParEval.SetInPars	= TRUE;
	ParEval.in_VCHandle	= *VCHandle;
	ParEval.in_AutoVar	= CalcPars->autoVar;
	ParEval.in_EnaCpx	= CalcPars->en_cpx;

//	ip->writerID = 0;		// >>> #CE_3.0.7 2026-07-25

	// если индекс > 0, то не транслировать
	// индекс может быть 0 только при первом вызове VI
	if (*Index)	cmd = CEK_CMD_Exec_Script;
	else		cmd = CEK_CMD_Parse_Auto | CEK_CMD_Exec_Script;

	do {
		// ------------------------
		// *****  Вызвать CE Kernel
		// ------------------------

		ParEval.io_ExpIndex	= *Index;

		err = cekrnl( cmd, &ip, &ParParse, &ParEval, err_f);

		*Index		= ParEval.io_ExpIndex;
		*OpdNum		= ParEval.out_NumOfFuncArgs;
		*NotEnd		= (LVBoolean)ParEval.out_NotEnd;

		if (*NotEnd || err)	// или выполнить FuncVI или HardError
			goto out;	//
		if (*err_f)		// нужно получить
			break;		// результат и сообщение об ошибке

		// *****  To perform evaluation again (from the beginning of script)
		*Index	= 0;
		cmd	= CEK_CMD_Exec_Script;

	} while (--(*RepeatCounter) > 0);

	// ---------------------------------------------------
	// *****  Вызвать CE Result когда выполнение закончено
	// ---------------------------------------------------
	err = ceExpResult( ip, num, str, type, err_f);

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
