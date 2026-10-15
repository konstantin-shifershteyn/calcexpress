/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#ifndef __FUNC_TEMPLATE__
#define __FUNC_TEMPLATE__

// =============================================
// Исполнители Функций одного аргумента типа DBL
// =============================================
#define	_func1a_dbl(_fname_, _fexec_)							\
MgErr _fname_ ( _dbl_operand *arg)  {							\
	register	int32	size;							\
	register	float64	*nums;							\
	if ( arg->nDims)								\
		for (nums=arg->array, size=arg->size1*arg->size2; size; size--, nums++)	\
		{	*nums		= (_fexec_(*nums));		}		\
	else	{	arg->scalar	= (_fexec_(arg->scalar));	}		\
	return noErr;									\
}


// =============================================
// Исполнители Функций одного аргумента типа CDB
// =============================================
#define	_func1a_cdb(_fname_, _fexec_)							\
MgErr _fname_ ( _dbl_operand *arg)  {							\
	register	int32	size;							\
	register	_cdb_	*cdb;							\
	for(cdb=(_cdb_*)arg->array, size=arg->size1 * arg->size2; size; size--, cdb++)	\
		{	_fexec_( cdb, cdb );	}					\
	return noErr;									\
}

// =======================================================
// Функции одного аргумента Array of DBL с результатом DBL
// =======================================================
#define	_func1a_arr_dbl(_fname_, _fexec_)					\
MgErr _fname_ ( _dbl_operand *arg)  {						\
	int32	size;								\
	float64	*pnt;								\
	if ( arg->nDims )							\
	     {	pnt = arg->array;	size = arg->size1 * arg->size2;	}	\
	else {	pnt = &(arg->scalar);	size = 1;			}	\
	arg->scalar	= _fexec_(pnt, size);					\
	arg->ResType	= OPTYPE_DBL;						\
	arg->nDims	= 0;							\
	return noErr;								\
}
// ============================================================
// Функции двух аргументов Array of DBL и DBL с результатом DBL
// ============================================================
#define	_func2a_arr_dbl_dbl(_fname_, _fexec_, type)			\
MgErr _fname_(	_dbl_operand	*in1,					\
		_dbl_operand	*in2,					\
		_dbl_operand	*out)	{				\
	int32	size;	float64	*p;					\
	out->ResType	= OPTYPE_DBL;					\
	out->nDims	= 0;						\
	out->size1	= out->size2 = 1;				\
	/* *** 2-й аргумент должен быть DBL scalar *** */		\
	if (in2->ResType != OPTYPE_DBL) {				\
		out->scalar = 0.0;					\
		out->erMsg  = "Second argument must be DBL scalar";	\
		return noErr;						\
	}								\
	if (in1->ResType == OPTYPE_DBL)					\
	     { p= &(in1->scalar); size= 1;			}	\
	else { p= in1->array;     size= in1->size1*in1->size2;	}	\
	out->scalar = _fexec_(p, size, (type)(in2->scalar));		\
	return noErr;							\
}

// ====================================================
// Функции трех аргументов типа DBL (скаляры и массивы)
// ====================================================
#define	_func3a_dbl(_fname_, _fexec_)				\
MgErr _fname_(	_dbl_operand	*in1,				\
		_dbl_operand	*in2,				\
		_dbl_operand	*in3,				\
		_dbl_operand	*out )				\
{ return  math_arg3_template(in1,in2,in3,out, _fexec_); }	\


#endif
