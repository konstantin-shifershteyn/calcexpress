/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <lviDataConversion.h>
#include <CEError.h>


// ----------------------------------------
MgErr  _To_Flat_Data(
			_CE_Instance	*ip,
			_OperandBase	*op,
			LVBoolean		*err_f)
{
	MgErr	err = noErr;

static	char*	erMsg = "Operand value does not match to its type (Type Code = %d). "
	"CalcExpress kernel encountered the error during convertion to flattened data format.";

	if (*err_f)	return err;

	_flat_data(op->type, op->str, err_f);
	if (*err_f) {
		err = CE_Error(ip, erMsg, (LStrHandle)(size_t)op->type, NULL);
	}
	return err;
}

// ----------------------------------------
MgErr  _To_Unflat_Data(
			_CE_Instance	*ip,
			_OperandBase	*op,
			LVBoolean		*err_f)
{
	MgErr	err = noErr;

static	char*	erMsg = "Operand type does not match to its flattened data (Type Code = %d). "
	"CalcExpress kernel detected the error during convertion from flattened data format.";

	if (*err_f)	return err;

	_unflat_data(op->type, op->str, err_f);
	if (*err_f) {
		err = CE_Error(ip, erMsg, (LStrHandle)(size_t)op->type, NULL);
	}
	return err;
}
