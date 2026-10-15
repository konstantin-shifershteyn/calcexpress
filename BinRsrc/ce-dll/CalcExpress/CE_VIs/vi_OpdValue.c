/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Obtain an operand value
// -----------------------------------

#include <extcode.h>

#include <CalcExpress.h>
#include <CEError.h>
#include <VarMan.h>
#include <VMEngine_Routines.h>

#include <ceAccessContext.h>
#include <lviDataConversion.h>

#include <celib\ceMemLib.h>


// Body is at the end of file
MgErr viOpdValue_Determine(_CE_Instance *ip, _OperandVI *opd, Bool32 fStr, Bool32 fCT, LVBoolean *err_f);


// ========================================
MgErr	__cdecl	viOpdValue(	
				LVRefNum		*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				_OperandVI		*opd,
				LVBoolean		*may_string,
				LVBoolean		*may_complex,
				LVBoolean		*err_f)
{
	_CE_Instance		*ip;
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
		*err_f = LVTRUE;	// error
		return noErr;
	}

	// *****  Определить значение
	if (err = viOpdValue_Determine(ip, opd, (Bool32)*may_string,(Bool32)*may_complex, err_f)) {

		CE_Error_LVMgErr(ip, err, err_f);
	}

	// *****  Освободить контекст
	_CEContext_UnlockReliable(ip);

	return err;
}


// ================================================================

static char *	ErrMsg_DblOrStr = "Invalid parameter type (either DBL Scalar or String expected)";
static char *	ErrMsg_DblOnly  = "Invalid parameter type (DBL Scalar expected)";
static char *	ErrMsg_NoString = "Invalid parameter type (String is not allowed)";

// -------------------------------------
// Функция определения значения операнда
// -------------------------------------
MgErr viOpdValue_Determine(	
				_CE_Instance	*ip,
				_OperandVI		*opd,
				Bool32			may_string,
				Bool32			may_complex,
				LVBoolean		*err_f)
{
	LVBoolean	glob = LVTRUE;
	uInt16		errc;
	MgErr		err = noErr;

	if (*err_f)
		goto out;

	// Если операнд-переменная, то определить значение
	// -----------------------------------------------
	if (opd->type == OPTYPE_IDENT) {
		if (err = vmCmd_Var_GetValue(ip->VCHandle, &glob, opd->str, (_VarInfo*)opd, NULL, err_f, &errc))
			goto out;
		if (*err_f) {
			if (errc == VM_ERR_ALIAS_NOTFOUND)
					err = CE_Error(ip, "Undefined alias: \'%H\'",	 opd->str, NULL);
			else	err = CE_Error(ip, "Undefined variable: \'%H\'", opd->str, NULL);
			goto out;
		}
		// ***	В VarMan данные храняться как unflatten
		_To_Flat_Data( ip, opd, err_f);
		if (*err_f) {
			err = CE_InternalError(ip, "Get Operand Value CIN",NULL,NULL);
			goto out;
		}
	}
	// Проверка типа
	// -------------
	switch (opd->type) {
		case OPTYPE_DBL :
			LStr_Clear(opd->str);	// освободить строку
			break;

		case OPTYPE_STR :
			if (!may_string) {
				err = CE_Error(ip, ErrMsg_NoString, NULL,NULL);
				*err_f = LVTRUE;
				goto out;
			}
			break;
		default :
			if (!may_complex) {
				if (may_string)
					err = CE_Error(ip, ErrMsg_DblOrStr, NULL,NULL);
				else	err = CE_Error(ip, ErrMsg_DblOnly,  NULL,NULL);
				*err_f = LVTRUE;
				goto out;
			}
			break;
	}
out:
	return err;
}
