/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	_INTERNAL_DATA_TYPES_H_
#define	_INTERNAL_DATA_TYPES_H_

#include <CE_DataTypes.h>


// ===========================================================
// Внутреннее представление операндов числовых типов dbl и cdb
// ===========================================================
typedef struct {
	uInt32		ResType;	// тип результата
	int32		nDims;		// Number of Dimentions (0 for scalar)
	int32		size1;		// size of dim 1
	int32		size2;		// size of dim 2
	float64		scalar;		// scalar value
	float64		*array;		// pointer to array
	Bool32		alloc;		// flag of memory allocation for array
	int32		__pad_1__;
	char		*erMsg;		// pointer to error message
}_dbl_operand;
STATIC_ASSERT_8_BYTES_OFFSET(_dbl_operand,scalar);
STATIC_ASSERT_8_BYTES_SIZE(_dbl_operand);

// ==================================================
// Внутреннее представление операндов строковых типов
// ==================================================
typedef struct {
	uInt32		ResType;	// тип результата
	int32		nDims;		// Number of Dimentions (0 for scalar)
	int32		size1;		// size of dim 1
	int32		size2;		// size of dim 2
	float64		scalar;		// not used
	LStr		*(*array);	// pointer to array of pointers to LStr
	Bool32		alloc;		// flag of memory allocation for array
	int32		__pad_1__;
	char		*erMsg;		// pointer to error message
}_str_operand;
STATIC_ASSERT_8_BYTES_OFFSET(_str_operand,scalar);
STATIC_ASSERT_8_BYTES_SIZE(_str_operand);

// ==================================================
// Внутреннее представление строковых операндов через LStrHandle
// ==================================================
typedef struct {
	uInt32		ResType;	// тип результата
	int32		nDims;		// Number of Dimentions (0 for scalar)
	int32		size1;		// size of dim 1
	int32		size2;		// size of dim 2
	float64		scalar;		// not used
	LStrHandle	*array;		// pointer to array of LStrHandles
	Bool32		alloc;		// flag of memory allocation for array
	int32		__pad_1__;
	char		*erMsg;		// pointer to error message
}_LStr_operand;
STATIC_ASSERT_8_BYTES_OFFSET(_LStr_operand,scalar);
STATIC_ASSERT_8_BYTES_SIZE(_LStr_operand);

// --------------------------------------------------
// Обобщенное описание числовых и строковых операндов
// --------------------------------------------------
typedef struct {
	uInt32		ResType;	// тип результата
	int32		nDims;		// Number of Dimentions (0 for scalar)
	int32		size1;		// size of dim 1
	int32		size2;		// size of dim 2
	float64		scalar;		// scalar value
	void		*array;		// pointer to array
	Bool32		alloc;		// flag of memory allocation for array
	int32		__pad_1__;
	char		*erMsg;		// pointer to error message
}_gen_operand;
STATIC_ASSERT_8_BYTES_OFFSET(_gen_operand,scalar);
STATIC_ASSERT_8_BYTES_SIZE(_gen_operand);


// ==================================================
// Тип описывает информацию об одном индексе массива
// ==================================================
typedef struct {
	int32		nDims;		// Number of Dimentions (0 for scalar)
	int32		size1;		// size of dim 1
	int32		size2;		// size of dim 2
	int32		scalar;		// scalar index
	int32		*values;	// pointer to array of indexes, dynamically allocated when need
	Bool32		alloc;		// flag of memory allocation for values
//	int32		__pad_1__;
	int32		allocVals;	// >>> #CE_3.0.4 2026-07-04 : number of allocated values, = allocated_bytes/sizeof(int32)	
	const char	*erMsg;		// pointer to error message
}_idx_info;
STATIC_ASSERT_8_BYTES_SIZE(_idx_info);

// ==================================================
// Тип описывает информацию об индексах массива
// --------------------------------------------
// this structure is dynamically created by Parser when it puts an operand to a parsed expression with ExAr_PutOperand()
// Evaluator accesses this structure through _Operand.iip pointer
// Evaluator doesn't create/destroy the structure but can allocate/release index arrays:
//    _indexes_info.idx1.values and _indexes_info.idx2.values
// ==================================================
typedef struct {
	int32		qty;		// number of indexes
	Bool32		compound;	// contains either IDX_SUBSCRIPT_1D and IDX_SUBSCRIPT_2D, see definitions below
	_idx_info	idx1;		// index 1 info
	_idx_info	idx2;		// index 2 info
}_indexes_info;
STATIC_ASSERT_8_BYTES_SIZE(_indexes_info);

#define	IDX_SUBSCRIPT_1D	0	// x[i @ j]
#define	IDX_SUBSCRIPT_2D	1	// x[i , j]

#endif
