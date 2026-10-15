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
#include <VHistory.h>
#include <VMEngine_Routines.h>

#include <celib\ceMemLib.h>
#include <celib\FlattenData.h>
#include <lvsup\lvsup.h>


// **********************************************************************************
// ***********************  Preset Variable History VI  ****************************
// **********************************************************************************

MgErr __cdecl	viHistPreset(
			int32		vch,
			LStrHandle	name,
			uInt32		type,
			_hLVA1Dbl	time,
			_hLVA1Poly	data,
			_hLVA1U32	wrid,		// array of writer IDs		[#CE_3.1.0 2026-08-20]
			LVBoolean	*append,
			//LVBoolean	*err_f,
			uInt16		*err_c)
{
	LVBoolean	errf = LVFALSE;
	MgErr		err;

	if (err = vmCmd_History_Preset(vch, name, type, time, data, wrid, *append, &errf, err_c))
		vm_LVError(err, &errf, err_c);

	return err;
}

