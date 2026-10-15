/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CE_OPERANDS_H__
#define	__CE_OPERANDS_H__

#include <CE_DataTypes.h>
#include <VarMan.h>



// ##############################################################################
//                         Internal definitions for Operands
// ##############################################################################

// Internal representation of Operand
// ------------------------------------------------
typedef struct {
	uInt32			type;		// datatype code
#if defined(_M_X64)
	int32			__pad_1__;	// >>> #CE_3.0 2024-12-16
#endif
	LStrHandle		str;		// this handle is allocated and must be released
	union {						// >>> #CE_3.0 2024-12-16
		float64		num;
		LStrHandle	hdl;		// >>> #CE_3.0 2024-12-16	this handle is not allocated (just a copy)
	};
	_indexes_info	*iip;
	_VarInfo		*vpnt;		// pointer to _VarInfo of the variable in VarMan	>>> #CE_3.0.2 2026-06-19
	Bool32			deref;		// >>> #CE_3.0.5 2025-07-14 : true for dereferenced variable name (used by assignments and prefix/postfix increment/decrement)
	int32			__pad_2__;	// >>> #CE_3.0.5 2025-07-14
} _Operand;
STATIC_ASSERT_8_BYTES_OFFSET(_Operand,num);
STATIC_ASSERT_8_BYTES_SIZE(_Operand);

// ------------------------------
// Array of Operands
// ------------------------------
typedef struct {
	int32		dimSize;
//#if defined(_M_X64)			// >>> #CE_3.0.4 2026-07-04 : allways add padding to allign 'data' to 8 bytes
	int32		__pad_1__;			// >>> #CE_3.0 2024-12-16
//#endif						// >>> #CE_3.0.4 2026-07-04
	_Operand	data[1];
} _OpdArray;
STATIC_ASSERT_8_BYTES_OFFSET(_OpdArray,data);
STATIC_ASSERT_8_BYTES_SIZE(_OpdArray);

// Handle массива операндов
// ------------------------
typedef _OpdArray **_OpdArrayHandle;


// ##############################################################################
//                         External definitions for Operators
// ##############################################################################


// Operand for external communication (= VIs)
// ---------------------------------------------------
typedef	_VarInfo	_OperandVI;		// Base structure (same as in versions prior 2.0)
typedef	_VarInfo	_OperandBase;	// for interfacing with VIs

// Array of Operands
// ------------------------------
typedef struct {
	int32		dimSize;
#if defined(_M_X64)
	int32		pad_1;			// >>> #CE_3.0 2024-12-16
#endif
	_OperandVI	data[1];
} _OpdVIArray;

typedef _OpdVIArray **_OpdVIArrayHandle;


#endif
