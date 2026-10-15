/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	CE Error Manager
// --------------------------------------

#include <extcode.h>

#include <ceError.h>
#include <ceAccessContext.h>


#define	CEErr_CMD_INIT				0
#define	CEErr_CMD_ADD				1
#define	CEErr_CMD_GET				2
#define	CEErr_CMD_ADD_PURE			3
#define	CEErr_CMD_INSERT_CStr		4

#define	CEErr_VI_CMD_ParseError		0xF

// ========================

MgErr __cdecl	viCEError(
				LVRefNum	*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				int32		*cmd,
				int32		*ErrCode,
				LStrHandle	ErrStr,
				LVBoolean	*err_f)
{
	_CE_Instance	*ip;
	int32			errCode;
	char			*errSFmt;
	LVBoolean		errf = LVFALSE;
	MgErr			err = noErr;

	// ***** Convert LVRefNum to Instance Reference (CE 3.0)
	if ( !(ip = (_CE_Instance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// *****  Захватить контекст
	if (!_CEContext_Lock(ip)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*err_f	 = LVTRUE;
		*ErrCode = CE_ERR_INVINSTANCE;
		if (*cmd == CEErr_CMD_GET)
			return LStrPrintf(ErrStr, "Invalid CE Instance Reference");
		return noErr;
	}

	switch(*cmd) {
		case CEErr_CMD_INIT :
			if (!(*err_f))
				err = CE_Error_Init(ip);
			goto out;

		case CEErr_CMD_GET :
			err = CE_Error_GET( ip, ErrStr, ErrCode);
			if (*ErrCode)
				*err_f = LVTRUE;
			goto out;

		case CEErr_CMD_ADD :
			if (*err_f) {
				err = CE_Error( ip, NULL, ErrStr, NULL );
				if (*ErrCode)
					CE_Error_SetErrCode(ip, *ErrCode, &errf);
			}
			break;

		case CEErr_VI_CMD_ParseError :
			if (*err_f) {
				switch (errCode = *ErrCode) {
					case 0 :
						errCode	= CE_ERR_SYNTAX;
					default :
						errSFmt	= "Parsing Error: %H";
						break;
					case CE_ERR_SYNTAX :
						errSFmt	= "Syntax Error: %H";
						break;
					case CE_ERR_ABORT :
					case CE_ERR_RUNTIME :
						errSFmt	= "Runtime Error: %H";
						break;
				}
				err = CE_Error_AddPure(ip, errSFmt, ErrStr, NULL);
				CE_Error_SetErrCode(ip, errCode, &errf);
			}
			break;
	}

	if (err) {
		CE_Error_LVMgErr(ip, err, err_f);
		CE_Error_GET(ip, NULL, ErrCode);
	}
out:
	// *****  Освободить контекст
	_CEContext_Unlock(ip);
	return err;
}
