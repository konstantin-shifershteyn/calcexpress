/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef __VarMan_CIN_H__
#define __VarMan_CIN_H__

#include <VarMan.h>

// Codes of operations used in CINs
#define	VMCIN_CREATE	0
#define	VMCIN_DESTROY	1
#define	VMCIN_SETVAL	2
#define	VMCIN_GETVAL	3

//This macro converts CIN operation code to VarMan operation code
//#define VM_ACTION(code)	((uInt16)((code) + VM_CMD_VAR_CREATE))

#endif
