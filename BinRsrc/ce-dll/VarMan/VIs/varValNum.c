/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VarMan	VIs Interface
// -----------------------------------

#include <extcode.h>

#include <VarMan.h>
#include <VarMan_CIN.h>
#include <VMEngine_Routines.h>

#include <celib\ceMemLib.h>

/*
// =====================================
// Структура, через которую передаются
// параметры в функции расширения VarMan
// =====================================
typedef struct {
	uInt32		type;
#if defined(_M_X64)
	int32		pad_1;	// >>> #CE_3.0 2024-12-13 : padding bytes
#endif
	UHandle		val;
} vmcin_extdata;
*/

// Функции расширения VarMan
// =========================
#include "varValNum_Get.c"
#include "varValNum_Set.c"


// ===============================================================================
// =======  Variable Value (DBL/CDB Scalars and 1D/2D Arrays) : VER 2.x.x  =======
// ===============================================================================
MgErr __cdecl	viVarValNUM(
				uInt16		*cmd,
				int32		*VCH,
				LVBoolean	*glob,
				LStrHandle	name,
				UHandle		value,
				uInt32		*type,
				LVBoolean	*err_f,
				uInt16		*err_c	)
{
	vmcin_extdata	extdata;
	_VarInfo		tmppar;
	MgErr			err;

	tmppar.type		= *type;
	tmppar.str		= NULL;

	extdata.type	= *type;
	extdata.val		= value;

	switch (*cmd) {
		case VMCIN_DESTROY :
			err = vmCmd_Var_Destroy(*VCH, glob, name, err_f, err_c);
			break;
		case VMCIN_CREATE :
			err = vmCmd_Var_CreateEx(*VCH, glob, name, &tmppar, 0, NULL, vmExtFunc_setval, &extdata, err_f, err_c);
			break;
		case VMCIN_SETVAL :
			err = vmCmd_Var_SetValueEx(*VCH, glob, name, &tmppar, 0, NULL, vmExtFunc_setval, &extdata, err_f, err_c);
			break;
		case VMCIN_GETVAL :
			err = vmCmd_Var_GetValueEx(*VCH, glob, name, &tmppar, NULL, vmExtFunc_getval, &extdata, err_f, err_c);
			break;
		default:
			*err_f	= LVTRUE;
			*err_c	= VM_ERR_INVALID_PARAMS;
			return noErr;
	}
	if (err)
		vm_LVError(err, err_f, err_c);
	return err;
}

// >>> #CE_3.1.0 2026-08-22
// ===============================================================================
// =======  Variable Value (DBL/CDB Scalars and 1D/2D Arrays) : VER 3.1.0  =======
// ===============================================================================
MgErr __cdecl	viVarValNUM3(
				uInt16				cmd,
				int32				vch,
				LStrHandle			name,
				UHandle				value,
				uInt32				type,
				uInt32				writerID,
				_VM_VI_WriteStats	*wrStats,		// >>> #CE_3.1.0-r6 2026-09-18
				uInt16				*err_c	)
{
	vmcin_extdata	extdata;
	_VarInfo		tmppar;
	LVBoolean		glob = (vch == -1 ? LVTRUE : LVFALSE);	// >>> #CE_3.1.0 2026-08-22
	LVBoolean		errf = LVFALSE;
	MgErr			err;

	tmppar.type		= type;
	tmppar.str		= NULL;

	extdata.type	= type;
	extdata.val		= value;

	switch (cmd) {
		case VMCIN_DESTROY :
			err = vmCmd_Var_Destroy(vch, &glob, name, &errf, err_c);
			break;
		case VMCIN_CREATE :
			err = vmCmd_Var_CreateEx(vch, &glob, name, &tmppar, writerID, wrStats, vmExtFunc_setval, &extdata, &errf, err_c);
			break;
		case VMCIN_SETVAL :
			err = vmCmd_Var_SetValueEx(vch, &glob, name, &tmppar, writerID, wrStats, vmExtFunc_setval, &extdata, &errf, err_c);
			break;
		case VMCIN_GETVAL :
			err = vmCmd_Var_GetValueEx(vch, &glob, name, &tmppar, wrStats, vmExtFunc_getval, &extdata, &errf, err_c);
			break;
		default:
			errf	= LVTRUE;
			*err_c	= VM_ERR_INVALID_PARAMS;
			return noErr;
	}
	if (err)
		vm_LVError(err, &errf, err_c);
	return err;
}
// <<< #CE_3.1.0 2026-08-22
