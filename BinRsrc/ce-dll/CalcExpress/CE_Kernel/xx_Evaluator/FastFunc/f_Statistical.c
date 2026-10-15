/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include "prob_&_stat\ps-lib.c"


// *****  Statistical Functions  *****

_func1a_arr_dbl (	_rms_dbl,	rms	);	// RMS(x)
_func1a_arr_dbl (	_madev_dbl,	madev	);	// madev(x)  : Mean Absolute Deviation
_func1a_arr_dbl (	_var_dbl,	var	);	// var(x)    : variance
_func1a_arr_dbl (	_vars_dbl,	vars	);	// _var(x)   : sample variance
_func1a_arr_dbl (	_stdev_dbl,	stdev	);	// stdev(x)  : standard deviation
_func1a_arr_dbl (	_stdevs_dbl,	stdevs	);	// _stdev(x) : sample standard deviation

_func1a_arr_dbl (	_median_dbl,	median	);	// median(x) : median

_func2a_arr_dbl_dbl(	_mode_dbl, 	mode	, int32);	// mode(x [,intervals])
_func2a_arr_dbl_dbl(	_moment_dbl,	moment	, int32);	// moment(x, order)


// *****  Special Functions  *****

_func1a_dbl	(	_erf_dbl,		__erf	);	// erf(x)
_func1a_dbl	(	_erfc_dbl,		__erfc	);	// erfc(x)
_func1a_dbl	(	_gamma_dbl,		__gamma	);	// gamma(x)

_func2a_dbl	(	_beta_dbl,		__beta,		_op2a_dbl_fdir	);	// beta(z,w)
_func2a_dbl	(	_gammai_dbl,	__gammai,	_op2a_dbl_fdir	);	// gammai(a,x)

_func3a_dbl	(	_betai_dbl,		__betai	);	// betai(a,b,x)


// *****  Bessel Functions  *****

_func1a_dbl(	_j0_dbl,	_j0	);			// _j0(x)
_func1a_dbl(	_j1_dbl,	_j1	);			// _j1(x)
_func1a_dbl(	_y0_dbl,	_y0	);			// _y0(x)
_func1a_dbl(	_y1_dbl,	_y1	);			// _y1(x)

_func2a_dbl(	_jn_dbl,	_jn,	_op2a_dbl_f1int	);	// _jn(n,x)
_func2a_dbl(	_yn_dbl,	_yn,	_op2a_dbl_f1int	);	// _yn(n,x)


// ============================================================================


//_func1a_dbl	(	_factr_dbl,	factr	);	// factr(x)	n!

// =============================
// factr(x) : x factorial
// =============================
MgErr _factr_dbl ( _dbl_operand *arg)
{
	if ( arg->nDims) {
		int32	size;
		float64	*nums;
		for (nums = arg->array, size = arg->size1*arg->size2; size; size--, nums++)
			*nums		= __factr(_DblToI32(*nums));
	}
	else	arg->scalar	= __factr(_DblToI32(arg->scalar));
	return noErr;
}

// =============================
// mse(x, y) : Mean Square Error
// =============================
MgErr _mse_dbl(	_dbl_operand	*in1,
		_dbl_operand	*in2,
		_dbl_operand	*out)
{
	int32	s1,s2;
	float64	*p1,*p2;

	out->ResType	= OPTYPE_DBL;
	out->nDims	= 0;
	out->size1	= out->size2 = 1;

	// *** Аргументы должны иметь одинаковый тип ***
	if (in1->ResType != in2->ResType) {
		out->scalar = 0.0;
		out->erMsg  = "Function parameters must have the same type";
		return noErr;
	}
	if (in1->ResType == OPTYPE_DBL) {
		p1 = &(in1->scalar);
		p2 = &(in2->scalar);
		s1 = s2 = 1;
	}
	else {
		p1 = in1->array;	s1 = in1->size1 * in1->size2;
		p2 = in2->array;	s2 = in2->size1 * in2->size2;
	}
	out->scalar = mse(p1,s1,p2,s2);
	return noErr;
}

// =============================================================
// ===============  Histogram Functions  =======================
// =============================================================
MgErr _hist_dbl_general(	_dbl_operand	*in1,
				_dbl_operand	*in2,
				_dbl_operand	*out,
				int32		what)
{
	int32	xsize, hsize;
	float64	*p;

	if (in1->ResType == OPTYPE_DBL)
	     { p= &(in1->scalar); xsize= 1;			}
	else { p= in1->array;     xsize= in1->size1*in1->size2;	}

	// *** 2-й аргумент должен быть DBL scalar ***
	if (in2->ResType != OPTYPE_DBL) {
		out->ResType	= OPTYPE_DBL;
		out->scalar	= 0.0;
		out->erMsg 	= "The 2nd parameter must be DBL scalar";
		return noErr;
	}
	if (in2->scalar < 1)	hsize = xsize/2;
	else					hsize = _DblToI32(in2->scalar);
	if (hsize < 1)			hsize = 1;

	// *** Подготовить структуру результата ***
	out->ResType	= OPTYPE_A1DBL;
	out->nDims	= 1;
	out->size1	= hsize;
	out->size2	= 1;
	out->array	= ce_DSNewPtr(sizeof(float64) * hsize);
	out->alloc	= (out->array != NULL);
	if (!(out->alloc))
		return mFullErr;

	return histogram(what, p, xsize, out->array, hsize);
}

// =====================
// histy(x [,intervals])
// =====================
MgErr	_histy_dbl(_dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	return _hist_dbl_general(in1, in2, out, 0);
}
// =====================
// histx(x [,intervals])
// =====================
MgErr	_histx_dbl(_dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	return _hist_dbl_general(in1, in2, out, 1);
}

