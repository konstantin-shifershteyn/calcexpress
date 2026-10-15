/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Get/Set 'errno' value
// -----------------------------------

#include <extcode.h>

#include <CalcExpress.h>
#include <ceAccessContext.h>
#include <rtErrorCode.h>


#define	GET_ERRNO	0
#define	SET_ERRNO	1

// ====================================
MgErr	__cdecl	viRTErrCode(
				LVRefNum	*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				uInt32		*action,
				int32		*errno_val,
				int32		*prev_errno_val,
				LVBoolean	*err_f)
{
	_CE_Instance		*ip;
	int32				vcHandle;
	MgErr				err = noErr;

	if (*err_f)
		return noErr;

	// ***** Convert LVRefNum to Instance Reference (CE 3.0)
	if ( !(ip = (_CE_Instance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// *****  Захватить контекст
	if (!_CEContext_Lock(ip)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*errno_val = *prev_errno_val = CE_ERR_INVINSTANCE;
		*err_f	= LVTRUE;
		return noErr;
	}

	// *****  Получить VC Handle
	vcHandle = ip->VCHandle;

	// *****  Освободить контекст
	_CEContext_UnlockReliable(ip);

	// ------------------
	switch(*action){
		case GET_ERRNO:
			*prev_errno_val = *errno_val = rtErrCode_Get(vcHandle);
			break;

		case SET_ERRNO:
			*prev_errno_val = rtErrCode_Set(vcHandle, *errno_val);
			break;
	}
	return noErr;
}
