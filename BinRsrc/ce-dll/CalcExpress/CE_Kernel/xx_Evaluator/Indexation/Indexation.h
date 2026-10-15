/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__Indexation_H__
#define	__Indexation_H__

// Типы используются функциями индексации
// --------------------------------------
enum idxType { idxScalar, idxArray, idxIncremental, idxDecremental };

typedef struct {
	int32			idxAmount;	// Number of indexes
	Bool32			compound;	// indexes are compound
	Bool32			allocArray;	// is there a need to allocate memory?
	enum  idxType	i1type;		// index1 type info
	enum  idxType	i2type;		// index2 type info
} _idx_HowToProc;

// --------------------------------------------

MgErr IndexingOperator(	_CE_Instance	*ip,
			_Operation	*op,
			LVBoolean	*err_f);


MgErr GetIndexedValue(	_CE_Instance	*ip,
			_OperandBase	*in,
			_OperandBase	*out,
			_indexes_info	*idxs,
			LVBoolean	*err_f);

MgErr SetIndexedValue(	_CE_Instance	*ip,
			_OperandBase	*arr,
			_OperandBase	*val,
			_indexes_info	*idxs,
			LVBoolean	*err_f);

MgErr IdxRangeOperator(	_CE_Instance	*ip,
			_Operand	*Lopd,
			_Operand	*Ropd,
			LVBoolean	*err_f);
#endif
