/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CE_EXP_RESULT_H__
#define	__CE_EXP_RESULT_H__

#include <CalcExpress.h>

extern	MgErr	__cdecl	ceExpResult(
					_CE_Instance	*ip,
					float64			*num,
					LStrHandle		str,		// may be NULL or pointer to NULL
					uInt32			*type,
					LVBoolean		*err_f);

#endif
