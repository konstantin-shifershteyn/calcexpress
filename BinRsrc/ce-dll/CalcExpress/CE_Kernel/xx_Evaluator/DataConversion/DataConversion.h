/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__Data_Conversion_H__
#define	__Data_Conversion_H__

/* OperandInfo.c */

//int32	__fastcall	_operand_get_nDims_BaseType(uInt32 type);

extern	int32	__fastcall	_operand_get_nDims(uInt32 type);
extern	uInt32	__fastcall	_operand_get_BaseType(uInt32 type);
extern	uInt32	__fastcall	_operand_get_TargetType(uInt32 baseType, int32 nDims);
extern	void	__fastcall	_general_operand_info(_OperandBase *arg, _gen_operand *data);

/* NumConverts.c */
extern	Bool32	__fastcall	_prepare_operand(_OperandBase *arg, _dbl_operand *data, Bool32 *itComplex);
extern	MgErr	__fastcall	_save_array(_dbl_operand *data, LStrHandle *sh);

/* StrConverts.c */
extern	MgErr	_prepare_strings(	_OperandBase *arg, _str_operand *data);
extern	MgErr	_save_strings(		_str_operand *data, LStrHandle *sh);

/* LStrConverts.c */
extern	MgErr	_prepare_LStr_result(	_str_operand  *in1, _str_operand *in2, _LStr_operand *out);
extern	MgErr	_prepare_LStr_result_1Dc(_str_operand *in1, _str_operand *in2, _LStr_operand *out);
extern	MgErr	_dispose_LStr_result(	_LStr_operand *out);
extern	MgErr	_save_LStrings(		_LStr_operand *data, LStrHandle *sh);


#endif
