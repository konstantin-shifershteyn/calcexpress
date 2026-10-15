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
#include <VMEngine.h>
#include <VMEngine_Routines.h>

#include <celib\ceMemLib.h>
#include <celib\FlattenData.h>


// ==================================================================
// =======  Variable R/W & Control (DBL Scalar) : VER 2.x.x  ========
// ==================================================================
MgErr __cdecl	viVarValDBL(
					uInt16		*cmd,
					int32		*VCH,
					LVBoolean	*glob,
					LStrHandle	name,
					float64		*num,
					LVBoolean	*err_f,
					uInt16		*err_c	)
{
	_VarInfo	par;
	MgErr		err;

	par.type= VM_TYPE_DBL;
	par.num	= *num;
	par.str	= NULL;

	switch (*cmd) {
		case VMCIN_DESTROY :
			err = vmCmd_Var_Destroy(*VCH, glob, name, err_f, err_c);
			break;
		case VMCIN_CREATE :
			err = vmCmd_Var_Create(*VCH, glob, name, &par, 0, NULL, err_f, err_c);
			break;
		case VMCIN_SETVAL :
			err = vmCmd_Var_SetValue(*VCH, glob, name, &par, 0, NULL, err_f, err_c);
			break;
		case VMCIN_GETVAL :
			err = vmCmd_Var_GetValue(*VCH, glob, name, &par, NULL, err_f, err_c);
			*num = par.num;
			if (!(*err_f) && (par.type != VM_TYPE_DBL)) {
				*err_f	= LVTRUE;
				*err_c	= VM_ERR_VAR_INVTYPE;
			}
			break;
		default :
			*err_f	= LVTRUE;
			*err_c	= VM_ERR_INVALID_PARAMS;
			return noErr;
	}
	if (err)
		vm_LVError(err, err_f, err_c);
	return err;
}

// >>> #CE_3.0.7 2026-07-23
// ##################################################################
// =======  Variable R/W & Control (DBL Scalar) : VER 3.0.7  ========
// ##################################################################
MgErr __cdecl	viVarValDBL2(
					uInt16		cmd,
					int32		vch,
					LVBoolean	*glob,
					LStrHandle	name,
					float64		*num,
					uInt32		*writerID,
					uInt16		*err_c	)
{
	_VarInfo	par;
	_VarRecord	*vp;
	LVBoolean	errf = LVFALSE;
	uInt16		errc = 0;
	MgErr		err = noErr;

	switch (cmd) {
		case VMCIN_DESTROY :
			err = vmCmd_Var_Destroy(vch, glob, name, &errf, &errc);
			break;
		case VMCIN_CREATE :
			par.type= VM_TYPE_DBL;
			par.num	= *num;
			par.str	= NULL;
			err = vmCmd_Var_Create(vch, glob, name, &par, *writerID, NULL, &errf, &errc);
			break;
		case VMCIN_SETVAL :
			par.type= VM_TYPE_DBL;
			par.num	= *num;
			par.str	= NULL;
			err = vmCmd_Var_SetValue(vch, glob, name, &par, *writerID, NULL, &errf, &errc);
			break;
		case VMCIN_GETVAL :
			VARMAN_LOCK_SHARED();
			if (vp = __GetVarPointer(vch, glob, name, NULL, &errc)) {
				*writerID = vp->writerID;
				if (vp->inf.type == VM_TYPE_DBL)
					*num = vp->inf.num;
				else
					errc = VM_ERR_VAR_INVTYPE;
			}
			VARMAN_UNLOCK_SHARED();
			break;
		default :
			errc	= VM_ERR_INVALID_PARAMS;
			break;
	}
	if (err)
		vm_LVError(err, &errf, &errc);
	if (errc)
		*err_c = errc;
	return err;
}
// <<< #CE_3.0.7 2026-07-23

// >>> #CE_3.0.7 2026-07-23
// ##################################################################
// =======  Variable R/W & Control (STRING) : VER 3.0.7  ========
// ##################################################################
MgErr __cdecl	viVarValSTR2(
					uInt16		cmd,
					int32		vch,
					LVBoolean	*glob,
					LStrHandle	name,
					LStrHandle	value,
					uInt32		*writerID,
					uInt16		*err_c	)
{
	_VarInfo	par;
	_VarRecord	*vp;
	LVBoolean	errf = LVFALSE;
	uInt16		errc = 0;
	MgErr		err = noErr;

	switch (cmd) {
		case VMCIN_DESTROY :
			err = vmCmd_Var_Destroy(vch, glob, name, &errf, &errc);
			break;
		case VMCIN_CREATE :
			par.type= VM_TYPE_STR;
			par.num	= 0.0;
			par.str	= value;
			err = vmCmd_Var_Create(vch, glob, name, &par, *writerID, NULL, &errf, &errc);
			break;
		case VMCIN_SETVAL :
			par.type= VM_TYPE_STR;
			par.num	= 0.0;
			par.str	= value;
			err = vmCmd_Var_SetValue(vch, glob, name, &par, *writerID, NULL, &errf, &errc);
			break;
		case VMCIN_GETVAL :
			VARMAN_LOCK_SHARED();
			if (vp = __GetVarPointer(vch, glob, name, NULL, &errc)) {
				*writerID = vp->writerID;
				if (vp->inf.type == VM_TYPE_STR)
					err = LStr_Copy(value, vp->inf.str);
				else
					errc = VM_ERR_VAR_INVTYPE;
			}
			VARMAN_UNLOCK_SHARED();
			break;
		default :
			errc = VM_ERR_INVALID_PARAMS;
			break;
	}
	if (err)
		vm_LVError(err, &errf, &errc);
	if (errc)
		*err_c = errc;
	return err;
}
// <<< #CE_3.0.7 2026-07-23

////////////////////////////////////////////////////////////////////////////////////////////////////////////

// >>> #CE_3.1.0 2026-08-22
// ##################################################################
// =======  Variable R/W & Control (DBL Scalar) : VER 3.1.0  ========
// ##################################################################
MgErr __cdecl	viVarValDBL3 (
					uInt16				cmd,
					int32				vch,
					LStrHandle			name,
					float64				*num,
					uInt32				writerID,
					_VM_VI_WriteStats	*wrStats,
					uInt16				*err_c	)
{
	_VarInfo	par;
	_VarRecord	*vp;
	LVBoolean	glob = (vch == -1 ? LVTRUE : LVFALSE);	// >>> #CE_3.1.0 2026-08-22
	LVBoolean	errf = LVFALSE;
	uInt16		errc = 0;
	MgErr		err = noErr;

	switch (cmd) {
		case VMCIN_DESTROY :
			err = vmCmd_Var_Destroy(vch, &glob, name, &errf, &errc);
			break;
		case VMCIN_CREATE :
			par.type= VM_TYPE_DBL;
			par.num	= *num;
			par.str	= NULL;
			err = vmCmd_Var_Create(vch, &glob, name, &par, writerID, wrStats, &errf, &errc);
			break;
		case VMCIN_SETVAL :
			par.type= VM_TYPE_DBL;
			par.num	= *num;
			par.str	= NULL;
			err = vmCmd_Var_SetValue(vch, &glob, name, &par, writerID, wrStats, &errf, &errc);
			break;
		case VMCIN_GETVAL :
			VARMAN_LOCK_SHARED();
			if (vp = __GetVarPointer(vch, &glob, name, NULL, &errc)) {
				//*writerID = vp->writerID;
				__ReadVarWritingStats(vp, wrStats);
				if (vp->inf.type == VM_TYPE_DBL)
					*num = vp->inf.num;
				else
					errc = VM_ERR_VAR_INVTYPE;
			}
			VARMAN_UNLOCK_SHARED();
			break;
		default :
			errc	= VM_ERR_INVALID_PARAMS;
			break;
	}
	if (err)
		vm_LVError(err, &errf, &errc);
	if (errc)
		*err_c = errc;
	return err;
}
// <<< #CE_3.1.0 2026-08-22

// >>> #CE_3.1.0 2026-08-22
// ##################################################################
// =======  Variable R/W & Control (STRING) : VER 3.1.0  ========
// ##################################################################
MgErr __cdecl	viVarValSTR3 (
					uInt16				cmd,
					int32				vch,
					LStrHandle			name,
					LStrHandle			value,
					uInt32				writerID,
					_VM_VI_WriteStats	*wrStats,
					uInt16				*err_c	)
{
	_VarInfo	par;
	_VarRecord	*vp;
	LVBoolean	glob = (vch == -1 ? LVTRUE : LVFALSE);	// >>> #CE_3.1.0 2026-08-22
	LVBoolean	errf = LVFALSE;
	uInt16		errc = 0;
	MgErr		err = noErr;

	switch (cmd) {
		case VMCIN_DESTROY :
			err = vmCmd_Var_Destroy(vch, &glob, name, &errf, &errc);
			break;
		case VMCIN_CREATE :
			par.type= VM_TYPE_STR;
			par.num	= 0.0;
			par.str	= value;
			err = vmCmd_Var_Create(vch, &glob, name, &par, writerID, wrStats, &errf, &errc);
			break;
		case VMCIN_SETVAL :
			par.type= VM_TYPE_STR;
			par.num	= 0.0;
			par.str	= value;
			err = vmCmd_Var_SetValue(vch, &glob, name, &par, writerID, wrStats, &errf, &errc);
			break;
		case VMCIN_GETVAL :
			VARMAN_LOCK_SHARED();
			if (vp = __GetVarPointer(vch, &glob, name, NULL, &errc)) {
				//*writerID = vp->writerID;
				__ReadVarWritingStats(vp, wrStats);
				if (vp->inf.type == VM_TYPE_STR)
					err = LStr_Copy(value, vp->inf.str);
				else
					errc = VM_ERR_VAR_INVTYPE;
			}
			VARMAN_UNLOCK_SHARED();
			break;
		default :
			errc = VM_ERR_INVALID_PARAMS;
			break;
	}
	if (err)
		vm_LVError(err, &errf, &errc);
	if (errc)
		*err_c = errc;
	return err;
}
// <<< #CE_3.1.0 2026-08-22
