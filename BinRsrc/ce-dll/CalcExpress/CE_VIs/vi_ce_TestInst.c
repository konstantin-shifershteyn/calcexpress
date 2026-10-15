/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Test Context
// -----------------------------------

#include <extcode.h>

#include <CalcExpress.h>
#include <ceAccessContext.h>

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// >>> #CE_3.0.7 2026-07-26

MgErr	__cdecl	viCEVarWriterID(
				LVRefNum	*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				int32		cmd,
				uInt32		*writerID,
				LVBoolean	*err_f)
{
	_CE_Instance	*ip;
	uInt32			oldWriterID;
//	MgErr			err = noErr;

	if (*err_f)
		return noErr;

	// ***** Convert LVRefNum to Instance Reference (CE 3.0)
	if ( !(ip = (_CE_Instance*)ceref_GetRefData(*pRefNum, NULL)) )
		goto err_InstanceReference;

	// *****  Захватить контекст
	// -------------------------
	if (!_CEContext_Lock(ip)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*err_f		= LVTRUE;
		return noErr;
	}
	// *****  EXECUTE
	if (cmd == 0) {
		// === read
		*writerID = ip->writerID;
	}
	else {
		// === write and return old value
		oldWriterID		= ip->writerID;
		ip->writerID	= *writerID;
		*writerID		= oldWriterID;
	}

	// *****  Освободить контекст
	// --------------------------
	_CEContext_UnlockReliable(ip);
	return noErr;
}
// <<< #CE_3.0.7 2026-07-26


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

MgErr	__cdecl	viCETestInst(
				LVRefNum	*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				LVBoolean	*Dynamic,
				LVBoolean	*err_f)
{
	_CE_Instance		*ip;
	MgErr				err = noErr;

	if (*err_f)
		goto err_InstanceReference;

	// ***** Convert LVRefNum to Instance Reference (CE 3.0)
	if ( !(ip = (_CE_Instance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	if (!_CEContext_Check(ip)) {
err_InstanceReference:
		// *** IF INPUT ERROR OR INVALID INSTANCE REFERENCE ***
		*Dynamic = LVFALSE;
		*err_f   = LVTRUE;
		return noErr;
	}
	// ----------------------------------------------
	// Если контекст зарегистрирован в Resource Store
	// то он открыт динамически
	// ----------------------------------------------
	if ( cerstor(cersCmd_TagFind, cersOwner_CEInstance, (size_t*)&ip))
			*Dynamic = LVFALSE;
	else	*Dynamic = LVTRUE;

	return noErr;
}
