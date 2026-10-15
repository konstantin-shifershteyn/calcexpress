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


#define	VMCIN_ATTR_READ		0
#define	VMCIN_ATTR_WRITE	1

// ===============================================================================
// 		Variable Attribute Read/Write
// ===============================================================================

MgErr __cdecl	viVarAttribute(
					uInt16		cmd,
					int32		vch,
					LVBoolean	*glob,
					LStrHandle	name,
					LStrHandle	attribute,
					float64		*nval,
					uInt32		*type,
					uInt32		*writerID,					// >>> #CE_3.0.7 2026-07-26
					//LVBoolean	*err_f,
					uInt16		*err_c )
{
	LVBoolean	errf = LVFALSE;
	MgErr		err = noErr;

//	if (*err_f) {
//		*err_c = VM_ERR_INPUTERROR;
//		return noErr;
//	}

	switch(cmd) {
		default :
			//*err_f	= LVTRUE;
			*err_c	= VM_ERR_INVALID_PARAMS;
			return noErr;

		case	VMCIN_ATTR_READ :
			err = vmCmd_Var_GetAttribute(vch, glob, name, attribute, nval, type, writerID, &errf, err_c);
			break;

		case	VMCIN_ATTR_WRITE :
			err = vmCmd_Var_SetAttribute(vch, glob, name, attribute, nval, type, &errf, err_c);
			break;
	}
//out:
	if (err) {
		LStr_Clear(attribute);
		return vm_LVError(err, &errf, err_c);
	}
	if (errf)
		LStr_Clear(attribute);
	return err;
}
