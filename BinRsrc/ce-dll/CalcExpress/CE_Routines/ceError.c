/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <CalcExpress.h>
#include <ceError.h>

#include <celib\ceMemLib.h>


/*
	========================================================
	These functions defined in ceError.h as inline functions
	========================================================
MgErr	CE_Error_Init(_CE_Instance *ip) {
	ip->ErrCode = CE_ERR_NOERROR;
	return LStr_SetSizeOrCreate(&(ip->ErrMsg), 0);
}

void	CE_Error_SetErrCode(_CE_Instance *ip, int32 ErrCode, LVBoolean *err_f) {
	if (ip->ErrCode = ErrCode)
		*err_f = LVTRUE;
}
*/

// =================
int32	CE_Error_LVMgErr(_CE_Instance *ip, int32 ErrCode, LVBoolean *err_f) {
	if (ip->ErrCode = ErrCode)
		*err_f = LVTRUE;
	if (LStrPrintf(ip->ErrMsg, "LabVIEW Manager error: %d", ErrCode))
		LStr_Clear(ip->ErrMsg);
	return ErrCode;
}


// =================
MgErr	CE_Error(_CE_Instance *ip, CStr cfmt, LStrHandle str1, LStrHandle str2)
{
	MgErr	err = noErr;
	if (LStrLen(*(ip->ErrMsg)) == 0) {
		if (ip->procPars.ParsingInProgress) {
//				err = LStrPrintf(ip->ErrMsg, "Syntax Error:\n");	// "Syntax Error: "
				err = LStrPrintf(ip->ErrMsg, "Parsing Error:\n");	// "Syntax Error: "
		}
		else	err = LStrPrintf(ip->ErrMsg, "Runtime Error:\n");	// "Runtime Error: "
	}
	if (err)
		return (ip->ErrCode = err);

	return CE_Error_AddPure(ip, cfmt, str1, str2);
}

// =================
MgErr	CE_Error_AddPure(_CE_Instance *ip, CStr cfmt, LStrHandle str1, LStrHandle str2)
{
	LStrHandle	msg = NULL;
	MgErr		err = noErr;

	if (ip->procPars.ParsingInProgress)	ip->ErrCode = CE_ERR_SYNTAX;
	else								ip->ErrCode = CE_ERR_RUNTIME;

	if (cfmt) {
		// ***  Если указана строка формата, то LStrPrintf
		if (err = LStr_Create(&msg, 0))					goto out;
		if (err = LStrPrintf(msg, cfmt, str1, str2))	goto out;
		if (err = LStr_Cat(ip->ErrMsg, msg))			goto out;

		LStr_Destroy(msg);
	}
	else {	// ***  Если нет строки формата, то сообщение из str1 и str2
		if (str1) if (err = LStr_Cat(ip->ErrMsg, str1))	goto out;
		if (str2) if (err = LStr_Cat(ip->ErrMsg, str2))	goto out;
	}
out:
	if (err)
		ip->ErrCode = err;
	return err;
}

// =================
MgErr	CE_Error_GET(_CE_Instance *ip, LStrHandle errMsg, int32 *errCode)
{
	MgErr	err = noErr;

	if (errCode) {
		*errCode = ip->ErrCode;
	}
	if (errMsg) {
		if (ip->ErrCode)	err = LStr_Copy(errMsg, ip->ErrMsg);
		else			err = LStr_Clear(errMsg);
	}
	return err;
}
