/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CE_CONSTRUCTS__
#define	__CE_CONSTRUCTS__

#define	FIO_TYPE_FUNC	0
#define	FIO_TYPE_IDXOP	1

// ----------------------------------
// Тип описывает информацию о функции
// ----------------------------------
typedef struct {
	int32		type;			// 0-Function; 1-Indexing Operator
	int32		nArg;			// кол-во аргументов
	int32		brLevel;		// уровень вложения скобок ()
	Bool32		w_arg;			// признак "есть аргументы"
	int32		FastFuncIdx;	// индекс FastFunc
	int32		misc;			// miscellaneous (different for Func and Subscript)
} _FuncIdxOp;
STATIC_ASSERT_4_BYTES_SIZE(_FuncIdxOp);

// -----------------------------------
// Массив элементов параметров функций
// -----------------------------------
typedef struct {
	int32		dimSize;
	_FuncIdxOp	data[1];
} _FuncIdxOp_Array;
typedef _FuncIdxOp_Array **_FuncIdxOp_ArrayHandle;
STATIC_ASSERT_4_BYTES_SIZE(_FuncIdxOp_Array);

#endif
