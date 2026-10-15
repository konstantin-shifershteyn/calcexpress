/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CE_ERR_CODES__
#define	__CE_ERR_CODES__


// ***	CalcExpress Error Codes
// =============================
#define	CE_ERR_NOERROR			0
#define	CE_ERR_INVINSTANCE		7101
#define	CE_ERR_SYNTAX			7102
#define	CE_ERR_RUNTIME			7103
#define	CE_ERR_ABORT			7104
#define	CE_ERR_GENERIC			7105
#define	CE_ERR_FUNCVI_NOTFOUND	7106
#define	CE_ERR_FUNCVI_INVALID	7107

// ***	CSE Error Codes
// =============================
#define	CSE_ERR_NOERROR			0
#define	CSE_ERR_INVINSTANCE		7100
#define	CSE_ERR_SYNTAX			CE_ERR_SYNTAX
#define	CSE_ERR_RUNTIME			CE_ERR_RUNTIME
#define	CSE_ERR_ABORT			CE_ERR_ABORT
#define	CSE_ERR_GENERIC			CE_ERR_GENERIC


// ***	VarMan Error Codes
// =============================

#include <VarMan\VarMan_Errors.h>


#endif
