/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CE_EXPRESSION__
#define	__CE_EXPRESSION__


// Expression Element 
// ------------------
typedef struct {
	Bool32		IsOps;
	int32		__pad_1__;
	_Operand	opd;
	_Operation	ops;
} _ExpresElement;
STATIC_ASSERT_8_BYTES_OFFSET(_ExpresElement,opd);
STATIC_ASSERT_8_BYTES_SIZE(_ExpresElement);


// Expression Array
// ----------------
typedef struct {
	int32		dimSize;		// размер массива
	int32		__pad_1__;		// >>> #CE_3.0.4 2026-07-05
	_ExpresElement	data[1];	// массив эл-ов
} _ExpresArray;
STATIC_ASSERT_8_BYTES_OFFSET(_ExpresArray,data);
STATIC_ASSERT_8_BYTES_SIZE(_ExpresArray);

typedef _ExpresArray	**_ExpArrayHandle;


#endif
