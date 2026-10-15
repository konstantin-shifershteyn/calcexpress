/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__LVI_DataConversion_H__
#define	__LVI_DataConversion_H__

#include <extcode.h>

#include <CalcExpress.h>
#include <celib\FlattenData.h>

// ----------------------------------------
extern	MgErr	__cdecl	_To_Flat_Data  (_CE_Instance *ip, _OperandBase *op, LVBoolean *err_f);
extern	MgErr	__cdecl	_To_Unflat_Data(_CE_Instance *ip, _OperandBase *op, LVBoolean *err_f);

#endif
