/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Get VC Handle of a CE Instance
// -----------------------------------

#include <extcode.h>

#include <CalcExpress.h>
#include <ceAccessContext.h>

// =========================================

MgErr	__cdecl	viVCHandle(	
				LVRefNum		*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				int32			*VCHandle,
				uInt32			*writerID,
				LVBoolean		*err_f)
{
	_CE_Instance		*ip;
	MgErr				err = noErr;

	// ***** Convert LVRefNum to Instance Reference (CE 3.0)
	if ( !(ip = (_CE_Instance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// *****  Захватить контекст
	if (!_CEContext_Lock(ip)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*VCHandle	= -1;
		*err_f		= LVTRUE;	// error
		return noErr;
	}

	*VCHandle = ip->VCHandle;
	*writerID = ip->writerID;		

	// *****  Освободить контекст
	_CEContext_UnlockReliable(ip);		// >>> #CE_3.1.0 2026-08-22

	return noErr;
}
