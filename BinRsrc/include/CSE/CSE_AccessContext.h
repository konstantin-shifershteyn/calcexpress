/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CSE_InstanceAccess_H__
#define	__CSE_InstanceAccess_H__

#include <CSE.h>
#include <RsrcManager.h>

// ===============================================================
// 	lock/unlock CSE Instance Context
// ===============================================================
#define	CSE_Context_Lock(ip)			cersamg(cermCmd_LockRsrc,		(__cersamg_RsrcHeader*)ip,	CSE_CONTEXT_SIGNATURE)
#define	CSE_Context_Unlock(ip)			cersamg(cermCmd_FreeRsrc,		(__cersamg_RsrcHeader*)ip,	CSE_CONTEXT_SIGNATURE)
#define	CSE_Context_UnlockReliable(ip)	cersamg(cermCmd_FreeReliable,	(__cersamg_RsrcHeader*)ip,	CSE_CONTEXT_SIGNATURE)
#define	CSE_Context_Check(ip)			cersamg(cermCmd_CheckRsrc,		(__cersamg_RsrcHeader*)ip,	CSE_CONTEXT_SIGNATURE)
#define	CSE_Context_Abort(ip)			cersamg(cermCmd_SetAsyncMsg,	(__cersamg_RsrcHeader*)ip,	CSE_CONTEXT_SIGNATURE)

#endif
