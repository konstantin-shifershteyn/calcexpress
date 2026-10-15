/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__VAR_INFO_POUTINES_H__
#define	__VAR_INFO_POUTINES_H__


typedef struct {
	uChar		*pntName;
	int32		lenName;
	uChar		*pntAlias;
	int32		lenAlias;
	int32		vch;		// vc handle
	uInt32		type;		// var type
	LVBoolean	fHist;		// history flag
} _VarInfoReport;

// ====================================

extern	int32	VInfo_To_CString	(uChar *dst, _VarInfoReport *vir);
extern	uChar *	VInfo_From_CString	(uChar *src, _VarInfoReport *vir);

#endif
