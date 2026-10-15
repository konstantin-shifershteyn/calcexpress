/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#ifndef __FUNC_TEMPLATE_2A__
#define __FUNC_TEMPLATE_2A__


// ######################################################################################
// 	ОБОБЩЕННЫЙ МАКРОС ВЫПОЛНЕНИЯ ОПЕРАЦИЙ И ФУНКЦИЙ 2-х АРГУМЕНТОВ
// ######################################################################################

#define	_func2a_dbl	_op2a_dbl		// alias

#define _op2a_dbl( fname, ACTION, MDF )	\
										\
MgErr fname (	_dbl_operand	*in1,	\
				_dbl_operand	*in2,	\
				_dbl_operand	*out)	\
{										\
	int		size;						\
	float64	*pnt;						\
	MgErr	err;						\
										\
	if (err = res2_init(in1,in2,out))	\
		return err;						\
										\
	/* ---  Оба операнда - Скаляры */	\
	if (!out->nDims) {					\
		MDF(ACTION, out->scalar, in1->scalar, in2->scalar);	\
		return noErr;	\
	}					\
						\
	pnt = out->array;	\
						\
	/* ---  Только Первый Скаляр */	\
	if ( !in1->nDims ) {			\
		register float64 *p2;		\
		p2 = in2->array;			\
		for(size= out->size1 * out->size2; size; size--, pnt++, p2++)	\
			MDF(ACTION, *pnt, in1->scalar, *p2);			\
		return noErr;	\
	}					\
	/* ---  Только Второй Скаляр */		\
	if ( !in2->nDims ) {	\
		float64 *p1;		\
		p1 = in1->array;	\
		for(size= out->size1 * out->size2; size; size--, pnt++, p1++)	\
			MDF(ACTION, *pnt, *p1, in2->scalar);		\
		return noErr;	\
	}					\
	/* --- Массивы одной размерности */	\
	if ( in1->nDims == in2->nDims ) {	\
		if (in1->nDims == 1) {			\
			/*Одномерные массивы*/		\
			register float64 *p1, *p2;	\
			p1 = in1->array;			\
			p2 = in2->array;			\
			for (size=out->size1; size ; size--, pnt++,p1++,p2++) {	\
				MDF(ACTION, *pnt, *p1, *p2);			\
			}			\
		}				\
		else {			\
			/* Двумерные массивы */		\
			int i, j;					\
			float64 *p1, *p2;			\
			for (i=0; i < out->size1; i++) {		\
				p1 = in1->array + i * in1->size2;	\
				p2 = in2->array + i * in2->size2;	\
				for (j=0; j < out->size2; pnt++, j++) {	\
					MDF(ACTION, *pnt, p1[j], p2[j]);	\
				}		\
			}			\
		}				\
		return noErr;	\
	}					\
	/* --- Массивы разных размерностей */	\
	{					\
		int32	row, col;	\
		float64 *p1, *p2;	\
		for (row=0; row < out->size1; row++) {				\
			p1 = in1->array;								\
			if (in1->nDims == 2)	p1 += row * in1->size2;	\
			p2 = in2->array;								\
			if (in2->nDims == 2)	p2 += row * in2->size2;	\
			for (col=0; col < out->size2; pnt++,p1++,p2++,col++) {	\
				MDF(ACTION, *pnt, *p1, *p2);	\
			}			\
		}				\
	}					\
	return noErr;		\
}

// ======================================================
//   Шаблоны операций и функций для обобщенного макроса
// ======================================================

// Шаблоны операций (OPERATORS)	для обобщенного макроса
#define	_op2a_dbl_opdbl(OPER, out, in1, in2)	(out) = (in1) OPER (in2)
//#define	_op2a_dbl_opi32(OPER, out, in1, in2)	(out) = ((int32)(in1)) OPER ((int32)(in2))
//#define	_op2a_dbl_opi32(OPER, out, in1, in2)	(out) = (_DblToInt(in1)) OPER (_DblToInt(in2))	// >>> #CE_3.0	2025-06-15
#define	_op2a_dbl_opi32(OPER, out, in1, in2)	(out) = (_DblToI32Bits(in1)) OPER (_DblToI32Bits(in2))	// >>> #CE_3.1.0-r6 2026-09-25
#define	_op2a_dbl_cmpdbl(OPER, out, in1, in2)	(out) = ((in1) OPER (in2) ? 1.0 : 0.0)			// >>> #CE_3.0.5 2025-07-17

// Шаблоны функций (FASTFUNC) для обобщенного макроса
#define	_op2a_dbl_fdir(FUNC, out, in1, in2)		(out) = FUNC((in1),(in2))
#define	_op2a_dbl_frev(FUNC, out, in1, in2)		(out) = FUNC((in2),(in1))
#define _op2a_dbl_f2rev(FUNC, out, in1, in2)	(out) = (FUNC(in2)/FUNC(in1))

//#define	_op2a_dbl_f1int(FUNC, out, in1, in2)	(out) = FUNC((int32)(in1),(in2))
#define	_op2a_dbl_f1int(FUNC, out, in1, in2)	(out) = FUNC(_DblToI32(in1),(in2))				// >>> #CE_3.0	2025-06-15
//#define	_op2a_dbl_frint(FUNC, out, in1, in2)	(out) = FUNC((in1),(int32)(in2))
#define	_op2a_dbl_frint(FUNC, out, in1, in2)	(out) = FUNC((in1),_DblToI32(in2))				// >>> #CE_3.0	2025-06-15


#endif
