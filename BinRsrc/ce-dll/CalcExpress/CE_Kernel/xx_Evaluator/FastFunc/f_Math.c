/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ========================================================================================
// ==========================  Разные Математические Функции   ============================
// ========================================================================================

/*
abs(x)
rcpl(x)
sqrt(x)

clip(x[,hi[,lo]])
sign(x)
spike(x)
square(x)
step(x)

_finite(x)
_isnan(x)
_infton(x)

e(x)
deg(x)
rad(x)
pi(x)

ceil(x)
floor(x)
fract(x)
getexp(x)
getman(x)
int(x)
*/

#define	_sign_(x)	(!(x) ? 0: (x)>0 ? 1:-1)
#define	_spike_(x)	((x>=0) && (x<1))
#define	_square_(x)	((_DblToI32(floor(x))) % 2 == 0)
#define	_step_(x)	(x>=0)
#define	_isinf_(x)	(!_finite(x) && !_isnan(x))


	_func1a_dbl(	_sign_dbl	, _sign_	);	// sign
	_func1a_dbl(	_spike_dbl	, _spike_	);	// spike
	_func1a_dbl(	_square_dbl	, _square_	);	// square
	_func1a_dbl(	_step_dbl	, _step_	);	// step

	_func1a_dbl(	_abs_dbl	, fabs		);	// abs : For cdb abs(x) is analog of norm(x)
	_func1a_dbl(	_rcpl_dbl	, 1/		);	// rcpl  (1/x)
	_func1a_dbl(	_sqrt_dbl	, sqrt		);	// sqrt

	_func1a_dbl(	_finite_dbl	, (float64)_finite);	// _finite(x)
	_func1a_dbl(	_isnan_dbl	, (float64)_isnan);		// _isnan(x)
	_func1a_dbl(	_isinf_dbl	, (float64)_isinf_);	// _isinf_(x) macro

// *** complex numbers ***
	_func1a_cdb(	_rcpl_cdb	, rcpl_cdb	);	// rcpl  (1/x)
	_func1a_cdb(	_sqrt_cdb	, sqrt_cdb	);	// sqrt


#undef	_sign_
#undef	_spike_
#undef	_square_
#undef	_step_



// --------------------------------------------------------------------------------------
//	Функции, реализация которых отличается от определяемой макросом _func1a_dbl (cdb)
// --------------------------------------------------------------------------------------

// ================================
// Infinity to representable number
// ================================
MgErr _infton_dbl( _dbl_operand *arg)
{
	int32	size;
	float64	*p;

	if ( arg->nDims) {	p = arg->array;		size = arg->size1 * arg->size2;	}
	else {			p = &(arg->scalar);	size = 1;	}

	while(size--)
		switch (_fpclass(*p)) {
			case _FPCLASS_NINF :	*p++ = -DBL_MAX; break;
			case _FPCLASS_PINF :	*p++ =  DBL_MAX; break;
		}
	return noErr;
}

// ===============================
// ===== DEG =====================
// ===============================
MgErr _deg_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--)
			*nums++		*= _180_dev_PI_;
	else	arg->scalar	*= _180_dev_PI_;
	return noErr;
}
MgErr _deg_cdb( _dbl_operand *arg)
{
	int32	size;
	_cdb_	*cdb;
	for( cdb= (_cdb_*)arg->array, size=arg->size1 * arg->size2;  size;  size--) {
		cdb->re *= _180_dev_PI_;
		cdb->im *= _180_dev_PI_;
	}
	return noErr;
}

// ===============================
// ===== RAD =====================
// ===============================
MgErr _rad_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--)
			*nums++		*= _PI_dev_180_;
	else	arg->scalar	*= _PI_dev_180_;
	return noErr;
}
MgErr _rad_cdb( _dbl_operand *arg)
{
	int32	size;
	_cdb_	*cdb;
	for( cdb= (_cdb_*)arg->array, size=arg->size1 * arg->size2;  size;  size--) {
		cdb->re *= _PI_dev_180_;
		cdb->im *= _PI_dev_180_;
	}
	return noErr;
}

// =================================
// ===== FRACT =====================
// =================================
MgErr _fract_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	float64	int_part;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= modf(*nums,		&int_part);
	else	arg->scalar	= modf(arg->scalar,	&int_part);
	return noErr;
}
// ===============================
// ===== INT =====================
// ===============================
MgErr _int_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	float64 fract_part;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			fract_part = modf(*nums,		nums);
	else	fract_part = modf(arg->scalar,	&arg->scalar);
	return noErr;
}
// ===============================
// ===== FLOOR ===================
// ===============================
MgErr _floor_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= floor(*nums);
	else	arg->scalar	= floor(arg->scalar);
	return noErr;
}
// ===============================
// ===== CEIL ====================
// ===============================
MgErr _ceil_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= ceil(*nums);
	else	arg->scalar	= ceil(arg->scalar);
	return noErr;
}

// ===============================
// ===== GETEXP ==================
// ===============================
MgErr _getexp_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	float64	mantis;
	int32	_exp;
	if ( arg->nDims)
		for (nums=arg->array, size=arg->size1 * arg->size2;  size; size--, nums++) {
			mantis = frexp(*nums, &_exp);
			*nums = (float64)_exp;
	}
	else	{
			mantis = frexp(arg->scalar, &_exp);
			arg->scalar = (float64)_exp;
	}
	return noErr;
}
// ===============================
// ===== GETMAN ==================
// ===============================
MgErr _getman_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	int32	_exp;
	if ( arg->nDims)
		for (nums=arg->array, size=arg->size1 * arg->size2;  size; size--, nums++)
			*nums		= frexp(*nums, &_exp);
	else	arg->scalar	= frexp(arg->scalar, &_exp);
	return noErr;
}

// >>> #CE_3.1.0-r6 2026-09-26
// ===============================
// ===== i32s =====================
// ===============================
MgErr _i32s_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= (float64)_DblToI32(*nums);
	else	arg->scalar	= (float64)_DblToI32(arg->scalar);
	return noErr;
}
// ===============================
// ===== i32b ====================
// ===============================
MgErr _i32b_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= (float64)_DblToI32Bits(*nums);
	else	arg->scalar	= (float64)_DblToI32Bits(arg->scalar);
	return noErr;
}
// ===============================
// ===== u32s =====================
// ===============================
MgErr _u32s_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= (float64)_DblToU32(*nums);
	else	arg->scalar	= (float64)_DblToU32(arg->scalar);
	return noErr;
}
// ===============================
// ===== u32b ====================
// ===============================
MgErr _u32b_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;
	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--, nums++)
			*nums		= (float64)_DblToU32Bits(*nums);
	else	arg->scalar	= (float64)_DblToU32Bits(arg->scalar);
	return noErr;
}
// <<< #CE_3.1.0-r6 2026-09-26

// =============================================================================================
// ========================= Функции с переменным числом аргументов ============================
// =============================================================================================

// ================================
// ===== PI( [x] ) ================
// ================================
MgErr _pi_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;

	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--)
			*nums++		*= _PI_;
	else	arg->scalar	*= _PI_;
	return noErr;
}
MgErr _pi_cdb( _dbl_operand *arg)
{
	int32	size;
	_cdb_	*cdb;
	for( cdb= (_cdb_*)arg->array, size=arg->size1 * arg->size2;  size;  size--) {
		cdb->re *= _PI_;
		cdb->im *= _PI_;
	}
	return noErr;
}

// ================================
// ===== E( [x] ) ================
// ================================
MgErr _e_dbl ( _dbl_operand *arg)
{
	int32	size;
	float64	*nums;

	if ( arg->nDims)
		for ( nums=arg->array , size=arg->size1 * arg->size2;  size;  size--)
			*nums++		*= _E_;
	else	arg->scalar	*= _E_;
	return noErr;
}
MgErr _e_cdb( _dbl_operand *arg)
{
	register	int32	size;
	register	_cdb_	*cdb;
	for( cdb= (_cdb_*)arg->array, size=arg->size1 * arg->size2;  size;  size--) {
		cdb->re *= _E_;
		cdb->im *= _E_;
	}
	return noErr;
}

// =========================================================================================

// ========================================
//	clip( x [,high [,low]] )
// ========================================
MgErr _clip_dbl(_dbl_operand	*in,
				int32			qty,
				_dbl_operand	*out)
{
	int32		i;
	float64		hi=1.0, lo=0.0;
	float64		*pnt;
	_dbl_operand	*arg;

	for (i=1; i<qty; i++) {
		arg = in + i;
		if (arg->ResType != OPTYPE_DBL) {
			out->erMsg = "Invalid parameter type (scalar expected)";
			goto out;
		}
		switch(i) {
			case 1:	hi = arg->scalar; break;
			case 2: lo = arg->scalar; break;
		}
	}
	if (lo > hi)
		SwapBlock(&hi, &lo, sizeof(hi));
	pnt = in->array;
	for (i = in->size1 * in->size2; i > 0; i--, pnt++) {
		if (*pnt > hi)	*pnt = hi;
		if (*pnt < lo)	*pnt = lo;
	}
out:
	out->ResType	= in->ResType;	// все остальное уже на месте
	out->scalar	= in->scalar;	// 

	return noErr;
}

// >>> #CE_3.1.0-r6 2026-09-24

// ==================================
//	nan()
// ==================================
MgErr f_nan(	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,
				int32			arg_qty,
				LVBoolean		*err_f)
{	// *** функция без аргумента
	if (arg_qty) 				// просто заглушка
		ip->OpdNum -= arg_qty;	// убрать все(!!!) из стека
	return	ff_PseudoArg(ip, _NaN_);
}
// ==================================
//	inf()
// ==================================
MgErr f_inf(	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,
				int32			arg_qty,
				LVBoolean		*err_f)
{	// *** функция без аргумента
	if (arg_qty) 				// просто заглушка
		ip->OpdNum -= arg_qty;	// убрать все(!!!) из стека
	return	ff_PseudoArg(ip, _Inf_P_);
}
// ==================================
//	infn()
// ==================================
MgErr f_infn(	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,
				int32			arg_qty,
				LVBoolean		*err_f)
{	// *** функция без аргумента
	if (arg_qty) 				// просто заглушка
		ip->OpdNum -= arg_qty;	// убрать все(!!!) из стека
	return	ff_PseudoArg(ip, _Inf_N_);
}
// <<< #CE_3.1.0-r6 2026-09-24


