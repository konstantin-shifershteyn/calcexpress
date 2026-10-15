/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Destroy Context
// -----------------------------------

#include <extcode.h>

#include <ceKernel.h>
#include <ceAccessContext.h>

// =================================================================

MgErr	__cdecl	viCEDestroyInst(
				LVRefNum	*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				LVBoolean	*Destroyed,
				LVBoolean	*err_f)
{
	_CE_Instance		*ip;
	int32				cmd;
	MgErr				err = noErr;

	// ***** Convert LVRefNum to Instance Reference (CE 3.0)
	if ( !(ip = (_CE_Instance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// ----------------------------------------------
	// ≈сли контекст зарегистрирован в Resource Store
	// (т.е. открыт динамически), то разрушить его,
	// иначе освободить (уменьшить) занимаемую пам€ть
	// ----------------------------------------------
	if ( cerstor(cersCmd_TagRemove, cersOwner_CEInstance, (size_t*)&ip)) {
			cmd = CEK_CMD_Cntx_FreeMem;	*Destroyed = LVFALSE;
	}
	else {	cmd = CEK_CMD_Cntx_Destroy;	*Destroyed = LVTRUE; }

	// -------------------------
	// *****  «ахватить контекст
	// -------------------------
	if (!_CEContext_Lock(ip)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*Destroyed	= LVFALSE;
		*err_f		= LVTRUE;
		return noErr;
	}

	// ------------------------
	// *****  ¬ызвать CE Kernel
	// ------------------------
	err = cekrnl( cmd, &ip, NULL, NULL, err_f);

	// --------------------------------------------------
	// *****  ќсвободить контекст (ничего страшного, если уничтожили)
	// --------------------------------------------------
	_CEContext_Unlock(ip);

	return noErr;
}
