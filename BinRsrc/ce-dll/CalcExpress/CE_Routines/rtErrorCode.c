/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <VarMan.h>
#include <VMEngine_Routines.h>
#include <rtErrorCode.h>

// =================================================================
// The function returns rtErrCode associated with a Variable Context
// =================================================================
int32 rtErrCode_Get( int32 vcHandle)
{
	LVBoolean			errf = FALSE;
	uInt16				errc = 0;
	VM_MiscAccessPars	par  = {VM_CMD_MISCACC_GET, 0, 0, 0};

	vmCmd_MiscFieldAccess( &vcHandle, &par, &errf, &errc);
/*
	VMEngine(	VM_CMD_MISCACC,		// cmd
			&vcHandle,		// *handle
			NULL,			// *glob
			NULL,			// name
			NULL,			// *par
			0,				// writerID		// >>> #CE_3.0.7 2026-07-23
			NULL,			// *extfunc
			&par,			// *extdata
			&errf,			// *err_f,
			&errc			// *err_c
		);
*/
	return par.val;
}

// ============================================================================
// The function assigns a value to rtErrCode associated with a Variable Context
// Returns a previous value
// ============================================================================
int32 rtErrCode_Set( int32 vcHandle, int32 value)
{
	LVBoolean			errf = FALSE;
	uInt16				errc = 0;
	VM_MiscAccessPars	par  = {VM_CMD_MISCACC_SET, 0, 0, 0};

	par.val = value;

	vmCmd_MiscFieldAccess( &vcHandle, &par,	&errf, &errc);
/*
	VMEngine(	VM_CMD_MISCACC,		// cmd
			&vcHandle,		// *handle
			NULL,			// *glob
			NULL,			// name
			NULL,			// *par
			0,				// writerID		// >>> #CE_3.0.7 2026-07-23
			NULL,			// *extfunc
			&par,			// *extdata
			&errf,			// *err_f,
			&errc			// *err_c
		);
*/
	return par.old;
}
