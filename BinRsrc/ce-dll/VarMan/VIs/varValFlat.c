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
#include <celib\FlattenData.h>

// The function body is in this file below
_VM_EXT_FUNC	__SetFlatValue;


// ===============================================================================
// ========  Variable Value (STRING OR FLATTENED TO STRING) : VER 2.x.x  =========
// ===============================================================================
MgErr __cdecl	viVarValFLAT(	
				uInt16		*cmd,
				int32		*VCH,
				LVBoolean	*glob,
				LStrHandle	name,
				LStrHandle	val,
				uInt32		*type,
				LVBoolean	*err_f,
				uInt16		*err_c	)
{
	_VarInfo	par;
	MgErr		err = noErr;

	if (*err_f) {
		*err_c = VM_ERR_INPUTERROR;
		return noErr;
	}

	switch(*cmd) {
		default :
			*err_f	= LVTRUE;
			*err_c	= VM_ERR_INVALID_PARAMS;
			return noErr;

		case VMCIN_DESTROY :
			if (err = vmCmd_Var_Destroy(*VCH, glob, name, err_f, err_c))
				vm_LVError(err, err_f, err_c);
			return err;

		case VMCIN_CREATE :
			// !!! Test for allowed type values is performed in VarMan
			// if (*type == OPTYPE_ANY_FLATTENED) {	// This type is not allowed !!!
			//	*err_f = LVTRUE; *err_c = VM_ERR_VAR_INVTYPE; return noErr;
			// }
			if (Test_Data_Flattened_To_LVString(*type, val, err_f)) {
				*err_c	= VM_ERR_BADDATA;	// *err_f is already TRUE
				return noErr;
			}
			par.type = *type;
			par.str  = val;
			if (err = vmCmd_Var_CreateEx(*VCH, glob, name, &par, 0, NULL, __SetFlatValue, NULL, err_f, err_c))
				vm_LVError(err, err_f, err_c);
			return err;

		case VMCIN_SETVAL :
			if (Test_Data_Flattened_To_LVString(*type, val, err_f)) {
				*err_c	= VM_ERR_BADDATA;	// *err_f is already TRUE
				return noErr;
			}
			par.type = *type;
			par.str  = val;
			if (err = vmCmd_Var_SetValueEx(*VCH, glob, name, &par, 0, NULL, __SetFlatValue, NULL, err_f, err_c))
				vm_LVError(err, err_f, err_c);
			return err;

		case VMCIN_GETVAL :
			break;
	}
	// ===================================
	//	GET VALUE OF A VARIABLE
	// ===================================
	par.str	= val;

	err = vmCmd_Var_GetValue(*VCH, glob, name, &par, NULL, err_f, err_c);
	if (err || *err_f)
		goto Out_Get;

	if (par.type != *type) {
		if (*type != OPTYPE_ANY_FLATTENED) {
			*err_f	= LVTRUE;
			*err_c	= VM_ERR_VAR_INVTYPE;
			goto Out_Get;
		}
		// *** If required any type
		*type = par.type;	// СОХРАНИТЬ
	}
	if ((par.type == VM_TYPE_DBL) || (par.type ==  VM_TYPE_IDXRANGE)) {
		if (err = LStr_SetSize(val, sizeof(float64)))
			goto Out_Get;
		_MoveBlock( &(par.num), LStrBuf(*val), sizeof(float64));
		reverse_dbl(LStrBuf(*val), 1);
	}
	else {
		_flat_data(par.type, val, err_f);	//flats only predefined types
		if (*err_f) {
			*err_c	= VM_ERR_BADDATA;
		}
	}
Out_Get:
	// ---------------------
	if (err) {
		LStr_Clear(val);
		return vm_LVError(err, err_f, err_c);
	}
	if (*err_f)
		LStr_Clear(val);
	return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

// >>> #CE_3.1.0 2026-08-22
// ===============================================================================
// ========  Variable Value (STRING OR FLATTENED TO STRING) : VER 2.x.x  =========
// ===============================================================================
MgErr __cdecl	viVarValFLAT3(	
				uInt16				cmd,
				int32				vch,
				//LVBoolean			*glob,
				LStrHandle			name,
				LStrHandle			val,
				uInt32				*type,
				uInt32				writerID,
				_VM_VI_WriteStats	*wrStats,		// >>> #CE_3.1.0-r6 2026-09-18
				//LVBoolean			*err_f,
				uInt16				*err_c	)
{
	_VarInfo	par;
	LVBoolean	glob = (vch == -1 ? LVTRUE : LVFALSE);	// >>> #CE_3.1.0 2026-08-22
	LVBoolean	errf = LVFALSE;
//	uInt16		errc = 0;
	MgErr		err = noErr;

	switch(cmd) {
		default :
			*err_c	= VM_ERR_INVALID_PARAMS;
			return noErr;

		case VMCIN_DESTROY :
			if (err = vmCmd_Var_Destroy(vch, &glob, name, &errf, err_c))
				vm_LVError(err, &errf, err_c);
			return err;

		case VMCIN_CREATE :
			// !!! Test for allowed type values is performed in VarMan
			// if (*type == OPTYPE_ANY_FLATTENED) {	// This type is not allowed !!!
			//	*err_f = LVTRUE; *err_c = VM_ERR_VAR_INVTYPE; return noErr;
			// }
			if (Test_Data_Flattened_To_LVString(*type, val, &errf)) {
				*err_c	= VM_ERR_BADDATA;	// *err_f is already TRUE
				return noErr;
			}
			par.type = *type;
			par.str  = val;
			if (err = vmCmd_Var_CreateEx(vch, &glob, name, &par, writerID, wrStats, __SetFlatValue, NULL, &errf, err_c))
				vm_LVError(err, &errf, err_c);
			return err;

		case VMCIN_SETVAL :
			if (Test_Data_Flattened_To_LVString(*type, val, &errf)) {
				*err_c	= VM_ERR_BADDATA;
				return noErr;
			}
			par.type = *type;
			par.str  = val;
			if (err = vmCmd_Var_SetValueEx(vch, &glob, name, &par, writerID, wrStats, __SetFlatValue, NULL, &errf, err_c))
				vm_LVError(err, &errf, err_c);
			return err;

		case VMCIN_GETVAL :
			break;
	}
	// ===================================
	//	GET VALUE OF A VARIABLE
	// ===================================
	par.str	= val;

	err = vmCmd_Var_GetValue(vch, &glob, name, &par, wrStats, &errf, err_c);
	if (err || errf)
		goto Out_Get;

	if (par.type != *type) {
		if (*type != OPTYPE_ANY_FLATTENED) {
			errf	= LVTRUE;
			*err_c	= VM_ERR_VAR_INVTYPE;
			goto Out_Get;
		}
		// *** If required any type
		*type = par.type;	// СОХРАНИТЬ
	}
	if ((par.type == VM_TYPE_DBL) || (par.type ==  VM_TYPE_IDXRANGE)) {
		if (err = LStr_SetSize(val, sizeof(float64)))
			goto Out_Get;
		_MoveBlock( &(par.num), LStrBuf(*val), sizeof(float64));
		reverse_dbl(LStrBuf(*val), 1);
	}
	else {
		_flat_data(par.type, val, &errf);	//flats only predefined types
		if (errf) {
			*err_c	= VM_ERR_BADDATA;
		}
	}
Out_Get:
	// ---------------------
	if (err) {
		LStr_Clear(val);
		return vm_LVError(err, &errf, err_c);
	}
	if (errf)
		LStr_Clear(val);
	return err;
}
// <<< #CE_3.1.0 2026-08-22

////////////////////////////////////////////////////////////////////////////////////////////////////////

// ===================================================
// Extension function is called by VarMan
// ===================================================

MgErr __SetFlatValue(	LStrHandle	name,		// name of variable (UNUSED)
			_VarInfo	*vp_int,	//_VarInfo inside VarMan
			_VarInfo	*vp_ext,	//_VarInfo outside VarMan
			void		*extdata,	// external data (UNUSED)
			LVBoolean	*err_f,
			uInt16		*err_c)
{
	MgErr	err;

	if (*err_f)	return noErr;

// При SETVAL проверку типа выполняет VarMan
// При CREATE проверку типа выполнять не надо
//	if (vp_int->type != vp_ext->type)	//поверка соответствия типов
//		goto invalid_type;

	switch(vp_int->type) {
		case VM_TYPE_DBL :
		case VM_TYPE_IDXRANGE :
			// ***	Unflatten numeric value from a string and save it in vp_int->num
			_MoveBlock( LStrBuf(*(vp_ext->str)), &(vp_int->num), sizeof(float64));
			reverse_dbl( (void*)&(vp_int->num), 1);
			return noErr;
	}
	// ***	All other types

	// 1.	Copy string
//	if (err = LStr_CopyOrCreate(&(vp_int->str), vp_ext->str))
	if (err = ce_CopyOrCreate_OperandString(&(vp_int->str), vp_ext->str, vp_int->type))		// >>> #CE_3.0.4 2026-07-06
		return err;

	// 2.	Unflat data (predefined types except String)
	_unflat_data(vp_int->type, vp_int->str, err_f);
	if (*err_f)
		*err_c	= VM_ERR_BADDATA;
	return noErr;
}
