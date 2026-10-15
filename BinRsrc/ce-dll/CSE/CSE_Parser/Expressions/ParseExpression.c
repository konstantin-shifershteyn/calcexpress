/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <ceKernel.h>

#include <CSE.h>
#include <CSE Parser.h>

CSE_PRIVATEFUNC
MgErr	CSE_ParseExpression(
				_ScrParserInst	*pi,
				int32			idx,
				Bool32			fEmptyJump,
				LVBoolean		*err_f )
{
	iPScrElement			*pe;
	_CEKernel_Par_Parse		ParParse;
	MgErr					err = noErr;

	ParParse.in_Script = NULL;	// !!!

	if (*err_f)
		return	noErr;

	if (idx < 0)	idx = pi->PScrIdx;		//valid idx is required to treat errors
	pe = iPScrEPnt(*(pi->iPScr), idx);

	if (pe->stmType != STMTYPE_EXPRESS)		// exit if not expression
		return	noErr;

	if ( !(pe->expLength) ) {
		if (fEmptyJump)	pe->stmType = STMTYPE_JUMP;			// in order to 'return;' can be treated by executor
		else			pe->stmType = STMTYPE_SRCREFSTUB;	//STMTYPE_SRCREF;
		return	noErr;
	}

	if ( pe->expLength == 1) {
		if ( *(pi->scr + pe->expOffset) == ';') {
		//	pe->stmType = STMTYPE_SRCREF;		// don't treat empty statements like ";;"
			pe->stmType = STMTYPE_SRCREFSTUB;	// don't treat empty statements like ";;"
			return	noErr;
		}
	}

	// ***	1. Call CE Kernel (Create CE Instance Context)
	if (err  = cekrnl(CEK_CMD_Cntx_Create, &(pe->ip), NULL, NULL, err_f))
		goto out;

	// ***	2. Prepare parsing parameters for CE Kernel
//	ParParse.in_enaNamedConst	= TRUE;						// >>> #CE_3.1.0-r6 2026-09-25
	ParParse.in_enaNamedConst	= pi->fEnaNamedConst;		// >>> #CE_3.1.0-r6 2026-09-28
	if (err = LStr_CreateFromMem(&(ParParse.in_Script), pi->scr + pe->expOffset, pe->expLength))
		goto out;

	// ***	3. Call CE Kernel (Parse Expression)
	if (err = cekrnl(CEK_CMD_Parse, &(pe->ip), &ParParse, NULL, err_f))
		goto out;

	// ***	4. Error handling
	if (*err_f) {

		err = CSE_MakeErrorMessage(pi->iCSE, idx, 0, TRUE, "%H", pe->ip->ErrMsg, NULL);

//		LStrHandle	stmSource = NULL;
//		if (err = LStr_CreateFromMem(&stmSource, pi->scr + pe->stmOffset, pe->stmLength))
//			goto out;
//		err = ceError(pi, pe->stmLine,
//				"%H\n"	"in %s\n" "%H", pe->ip->ErrMsg, (LStrHandle)stmName, stmSource);
//
//		LStr_Destroy(stmSource);
	}
out:
	if (ParParse.in_Script) {
		LStr_Destroy(ParParse.in_Script);
		ParParse.in_Script = NULL;
	}
	return err;
}
