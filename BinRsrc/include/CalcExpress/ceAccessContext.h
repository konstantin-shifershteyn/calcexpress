/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CE_ACCESS_CONTEXT_ROUTINES_H__
#define	__CE_ACCESS_CONTEXT_ROUTINES_H__

#include <extcode.h>
#include <CalcExpress.h>

#include <RsrcManager\RsrcManager.h>


// ===============================================================
// 	lock/unlock CE Instance Context
// ===============================================================

#define	_CEContext_Lock(ip)				cersamg(cermCmd_LockRsrc,		(__cersamg_RsrcHeader*)ip,	CE_CONTEXT_SIGNATURE)
#define	_CEContext_Unlock(ip)			cersamg(cermCmd_FreeRsrc,		(__cersamg_RsrcHeader*)ip,	CE_CONTEXT_SIGNATURE)
#define	_CEContext_UnlockReliable(ip)	cersamg(cermCmd_FreeReliable,	(__cersamg_RsrcHeader*)ip,	CE_CONTEXT_SIGNATURE)
#define	_CEContext_Check(ip)			cersamg(cermCmd_CheckRsrc,		(__cersamg_RsrcHeader*)ip,	CE_CONTEXT_SIGNATURE)
#define	_CEContext_Abort(ip)			cersamg(cermCmd_SetAsyncMsg,	(__cersamg_RsrcHeader*)ip,	CE_CONTEXT_SIGNATURE)


// ===============================================================
//	verifies a pointer to context
// ===============================================================
__inline MgErr __CECheckReference( _CE_Instance *ip )
{
	if ( DSCheckPtr(ip))						return mZoneErr;
	if (ip->signature != CE_CONTEXT_SIGNATURE)	return mZoneErr;
	return noErr;
}


#endif
