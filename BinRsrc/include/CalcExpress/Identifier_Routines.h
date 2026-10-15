/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__IDENTIFIERS_H__
#define	__IDENTIFIERS_H__

#include <extcode.h>

#include <ceKernel.h>

// ========================================================================================
// Returns LStrHandle to an Identifier name
// ========================================================================================
// для ускорения обработки (действий со стеком операндов) собственно строки идентификаторов
// не копируются.
// If _Operand.type==OPTYPE_IDENT , then _Operand.hdl member contains either a handle of
// the actual string or NULL, in wich case an actual string handle is in _Operand.str.
// ========================================================================================
__forceinline LStrHandle getIdentString(_Operand *var)		// >>> #CE_3.0.2 2026-06-09 
{
	if ( var->hdl )		
		return var->hdl;
	return var->str;
}

// ---------------------------------------------------------------
// returns TRUE if LStrHandle of an Identifier is in _Operand.hdl
// returns FALSE if LStrHandle of an Identifier is in _Operand.str
// ---------------------------------------------------------------
#define	IdentInNum(var)	((var)->hdl != NULL)


#endif
