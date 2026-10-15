/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef __CE_ERROR__
#define __CE_ERROR__

#include <CalcExpress.h>
#include <celib\ceMemLib.h>

#ifdef	__cplusplus
extern "C" {
#endif

__inline MgErr	CE_Error_Init(_CE_Instance *ip) {
	ip->ErrCode = CE_ERR_NOERROR;
	return LStr_SetSizeOrCreate(&(ip->ErrMsg), 0);
}

__inline int32	CE_Error_SetErrCode(_CE_Instance *ip, int32 ErrCode, LVBoolean *err_f) {
	if (ip->ErrCode = ErrCode)	*err_f = LVTRUE;
	return ErrCode;
}

int32	__cdecl	CE_Error_LVMgErr(_CE_Instance *ip, int32 ErrCode, LVBoolean *err_f);
MgErr	__cdecl	CE_Error_AddPure(_CE_Instance *ip, CStr cfmt, LStrHandle str1, LStrHandle str2);
MgErr	__cdecl	CE_Error(		 _CE_Instance *ip, CStr cfmt, LStrHandle str1, LStrHandle str2);

MgErr	__cdecl	CE_Error_GET(	 _CE_Instance *ip, LStrHandle errMsg, int32 *errCode);


#define CE_InternalError(inst,msg,p1,p2)	CE_Error(inst, (CStr)("Internal Error: " ## msg ## " [function "__FUNCTION__"()]"), (LStrHandle)(size_t)(p1), (LStrHandle)(size_t)(p2))


#ifdef	__cplusplus
}
#endif


#endif
