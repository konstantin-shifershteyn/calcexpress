/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		1) Test Context Instance RefNum
//						2) Get/Set Variables Writer ID
// -----------------------------------

#include <extcode.h>

#include <CSE.h>
#include <RsrcManager.h>
#include <CSE_AccessContext.h>


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// >>> #CE_3.0.7 2026-07-26

MgErr	__cdecl	cseVIVarWriterID(
				LVRefNum		*pRefNum,
				int32			cmd,
				uInt32			*writerID,
				int32			*ExecStatus)
{
	iPScrInstance	*iCSE;
	uInt32			oldWriterID;

	// ***** Convert LVRefNum to CSE Instance Reference (CE 3.0)
	if ( !(iCSE = (iPScrInstance*)ceref_GetRefData(*pRefNum, NULL)) )
		goto err_InstanceReference;

	// *****  Захватить CSE Instance Context
	// -------------------------------------
	if (!CSE_Context_Lock(iCSE)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*ExecStatus = estInvalidRefNum;
		return noErr;
	}
	// *****  EXECUTE
	if (cmd == 0) {
		// === read
		*writerID = iCSE->writerID;
	}
	else {
		// === write and return old value
		oldWriterID		= iCSE->writerID;
		iCSE->writerID	= *writerID;
		*writerID		= oldWriterID;
	}
	// *****  Освободить CSE Instance Context
	CSE_Context_UnlockReliable(iCSE);
	return noErr;
}
// <<< #CE_3.0.7 2026-07-26


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

MgErr	__cdecl	cseVITestInst(	
				LVRefNum	*pRefNum,	// iPScrInstancePnt	*iCSE_Hdl,
				LVBoolean	*Dynamic,
				LVBoolean	*err_f)
{
	iPScrInstance	*iCSE;
	MgErr			err = noErr;

	if (*err_f)
		goto err_InstanceReference;

	// ***** Convert LVRefNum to CSE Instance Reference (CE 3.0)
	if ( !(iCSE = (iPScrInstance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	if (!CSE_Context_Check(iCSE)) {
err_InstanceReference:
		// *** IF AN INPUT ERROR OR AN INVALID INSTANCE REFERENCE ***
		*Dynamic = LVFALSE;
		*err_f   = LVTRUE;
		return noErr;
	}
	// ----------------------------------------------
	// Если контекст зарегистрирован в Resource Store
	// то он открыт динамически
	// ----------------------------------------------
	if ( cerstor(cersCmd_TagFind, CSE_RESSTOR_OWNER, (size_t*)&iCSE))
			*Dynamic = LVFALSE;
	else	*Dynamic = LVTRUE;
	return noErr;
}
