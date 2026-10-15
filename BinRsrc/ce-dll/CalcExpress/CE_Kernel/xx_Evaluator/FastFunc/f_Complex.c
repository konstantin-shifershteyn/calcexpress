/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
 

// ====================================================================================
//			Функции построения комплексных чисел
// ====================================================================================

// =================================
// COMPLEX ( real [,image] )
// =================================
MgErr	_complex_cdb_low( register _cdb_ *op1, register _cdb_ *op2, register _cdb_ *res)
{
	res->re = op1->re;
	res->im = op2->re;
	return noErr;
}
MgErr	_complex_cdb( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	return  math_arg2cdb_template( in1, in2, out, _complex_cdb_low);
}
// =================================
// POLAR ( norm [,argument] )
// =================================
MgErr	_polar_cdb_low( register _cdb_ *op1, register _cdb_ *op2, register _cdb_ *res)
{
	res->re = op1->re * cos(op2->re);
	res->im = op1->re * sin(op2->re);
	return noErr;
}
MgErr	_polar_cdb( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	return  math_arg2cdb_template( in1, in2, out, _polar_cdb_low);
}

// ====================================================================================
// Функции выделения параметров комплексных чисел
// ====================================================================================
typedef	void (*__cpx_a1fs)( _cdb_ *cdb, float64 *dst);

MgErr __cpx_a1f(	 _dbl_operand	*arg,
			__cpx_a1fs	fexec)
{
	register	int32	size;
	register	float64	*dbl;
	register	_cdb_	*cdb;
			MgErr	err;

	if (err = dbl_to_cdb(arg)) goto out;

	dbl = arg->array;
	cdb = (_cdb_*)arg->array;

	// установить тип результата
	switch(arg->ResType) {
		case OPTYPE_CDB :	arg->ResType = OPTYPE_DBL;
					fexec( cdb, &(arg->scalar) );
					goto out;

		case OPTYPE_A1CDB :	arg->ResType = OPTYPE_A1DBL;	break;
		case OPTYPE_A2CDB :	arg->ResType = OPTYPE_A2DBL;	break;
	}
	for (size=arg->size1*arg->size2; size; size--)
		fexec( cdb++, dbl++ );
out:	return err;
}
// =================================
// REAL ( cdb )
// =================================
void _real_cdb_low( _cdb_ *cdb, float64 *dst) {
	*dst = cdb->re;
}
MgErr _real_cdb ( _dbl_operand *arg)  {
	return __cpx_a1f( arg, _real_cdb_low);
}
// =================================
// IMAG ( cdb )
// =================================
void _imag_cdb_low( _cdb_ *cdb, float64 *dst) {
	*dst = cdb->im;
}
MgErr _imag_cdb ( _dbl_operand *arg)  {
	return __cpx_a1f( arg, _imag_cdb_low);
}
// =================================
// ARG ( cdb )
// =================================
void _arg_cdb_low( _cdb_ *cdb, float64 *dst) {
	*dst = atan2( cdb->im, cdb->re);
}
MgErr _arg_cdb ( _dbl_operand *arg)  {
	return __cpx_a1f( arg, _arg_cdb_low);
}
// =================================
// NORM ( cdb )
// =================================
void _norm_cdb_low( _cdb_ *cdb, float64 *dst) {
	*dst = sqrt(cdb->re*cdb->re + cdb->im*cdb->im);
}
MgErr _norm_cdb ( _dbl_operand *arg)  {
	return __cpx_a1f( arg, _norm_cdb_low);
}

// ========

// =================================
// CONJ ( cdb )
// =================================
MgErr _conj_cdb ( _dbl_operand *arg)  {
	register	int32	size;
	register	_cdb_	*cdb;
			MgErr	err;

	if (err = dbl_to_cdb(arg)) return err;

	for ( cdb= (_cdb_*)arg->array, size= arg->size1*arg->size2; size; size--, cdb++)
		cdb->im = -(cdb->im);

	return noErr;
}
