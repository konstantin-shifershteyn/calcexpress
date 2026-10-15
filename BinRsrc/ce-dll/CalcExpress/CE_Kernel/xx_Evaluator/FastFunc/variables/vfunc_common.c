/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef __VCFUNC_COMMON_C__
#define __VCFUNC_COMMON_C__

static	char * ErrMsg_bad_Var_Name	= "The 1st parameter must have String datatype";
static	char * ErrMsg_bad_VCHandle2	= "The 2nd parameter must be numeric scalar";
static	char * ErrMsg_bad_VCHandle3	= "The 3rd parameter must be numeric scalar";

static	char * ErrMsg_bad_init_value	= "Invalid datatype of the initial value (2nd parameter)";

//__inline float64 __vfunc_err_code(uInt16 errc) { return -(float64)errc; }
__forceinline float64 __vfunc_err_code(uInt16 errc) { return errc ? -((float64)errc) : 0.0; }

// -------------------------------------------

#include "vfunc_copy.c"
#include "vfunc_nm.c"
#include "vfunc_vch.c"
#include "vfunc_nm_vch.c"
#include "vfunc_nm_val_vch.c"
#include "vfunc_ident-string.c"


#endif
