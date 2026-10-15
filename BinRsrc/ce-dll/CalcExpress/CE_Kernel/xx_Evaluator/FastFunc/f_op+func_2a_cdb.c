/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
 

// ======================================================================================
// =====================  Операции с Комплексными числами  ==============================
// ======================================================================================

// ===== СЛОЖЕНИЕ =====
MgErr	_add_cdb_low( register _cdb_ *op1, register _cdb_ *op2, register _cdb_ *res)
{
	res->re = op1->re + op2->re;
	res->im = op1->im + op2->im;
	return noErr;
}
MgErr	_add_cdb( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	return  math_arg2cdb_template( in1, in2, out, _add_cdb_low);
}

// ===== ВЫЧИТАНИЕ =====
MgErr	_sub_cdb_low( register _cdb_ *op1, register _cdb_ *op2, register _cdb_ *res)
{
	res->re = op1->re - op2->re;
	res->im = op1->im - op2->im;
	return noErr;
}
MgErr	_sub_cdb( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	return  math_arg2cdb_template( in1, in2, out, _sub_cdb_low);
}

// ===== УМНОЖЕНИЕ =====
MgErr	_mul_cdb_low( register _cdb_ *op1, register _cdb_ *op2, register _cdb_ *res)
{
	res->re = op1->re * op2->re - op1->im * op2->im;
	res->im = op1->re * op2->im + op1->im * op2->re;
	return noErr;
}
MgErr	_mul_cdb( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	return  math_arg2cdb_template( in1, in2, out, _mul_cdb_low);
}

// ===== ДЕЛЕНИЕ =====
MgErr	_div_cdb_low( register _cdb_ *op1, register _cdb_ *op2, register _cdb_ *res)
{
	res->re = (op1->re*op2->re + op1->im*op2->im) / (op2->re*op2->re + op2->im*op2->im);
	res->im = (op1->im*op2->re - op1->re*op2->im) / (op2->re*op2->re + op2->im*op2->im);
	return noErr;
}
MgErr	_div_cdb( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	return  math_arg2cdb_template( in1, in2, out, _div_cdb_low);
}


// ======================================================================================
// =======================  Функции Комплексных чисел  ==================================
// ======================================================================================

// === ATAN2 ====
MgErr	_atan2_cdb( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	return  math_arg2cdb_template( in1, in2, out, atan2_cdb);
}

// === POW ======
MgErr	_pow_cdb( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	return  math_arg2cdb_template( in1, in2, out, pow_cdb);
}

// === LOG =====
MgErr	_log_cdb( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	return  math_arg2cdb_template( in1, in2, out, log_cdb);
}
