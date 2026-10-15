/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Abort Evaluation
// -----------------------------------

#include <extcode.h>

#include <ceAccessContext.h>

MgErr	__cdecl	viCEAbort(
				LVRefNum	*pRefNum,	//_CE_InstancePnt *InstanceRef,
				LVBoolean	*err_f)
{
	_CE_Instance	*ip;
	MgErr			err = noErr;

	if (*err_f)
		return err;

	// ***** Convert LVRefNum to Instance Reference (CE 3.0)
	if ( !(ip = (_CE_Instance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	if ( ! _CEContext_Abort(ip)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*err_f = LVTRUE;
	}
	return err;
}
