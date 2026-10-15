/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CE_OPERATIONS__
#define	__CE_OPERATIONS__

// ##############################################################################
//                            Operators definition
// ##############################################################################

// ----------------------
// Тип описывает операцию
// ----------------------
typedef struct {
	int32		code;		// code of operation
	uInt16		prior;		// priority (precedence)
	uInt16		n_ops;		// number of operands
	uInt16		category;	// operator category
	uInt16		direction;	// associativity
	int32		misc;		// miscellaneous (used at least for Func & Subscript)
} _Operation;
STATIC_ASSERT_4_BYTES_SIZE(_Operation);

// -----------------------
// Array of Operators
// -----------------------
typedef struct {
	int32		dimSize;
	_Operation	data[1];
} _OpsArray;
typedef _OpsArray **_OpsArrayHandle;
STATIC_ASSERT_4_BYTES_SIZE(_OpsArray);

// ##############################################################################

// ----------------------------------
// 'code' values for Unary operators
// ----------------------------------
#define	OP1_LogNOT		0
#define	OP1_BitNOT		1
#define	OP1_Plus		2
#define	OP1_Minus		3
// >>> #CE_3.0 2025-03-12
#define	OP1_IncPref		4
#define	OP1_IncPost		5
#define	OP1_DecPref		6
#define	OP1_DecPost		7
// <<< #CE_3.0

// -----------------------------------
// 'code' values for Binary operators
// -----------------------------------
#define	OP2_AssMul		0	// *=
#define	OP2_Mul			1	// *
#define	OP2_AssDiv		2	// /=
#define	OP2_Div			3	// /
#define	OP2_AssMod		4	// %=
#define	OP2_Mod			5	// %
#define	OP2_AssPlus		6	// +=
#define	OP2_Plus		7	// +
#define	OP2_AssMinus	8	// -=
#define	OP2_Minus		9	// -
#define	OP2_AssLShift	10	// <<=
#define	OP2_LShift		11	// <<
#define	OP2_LessOrEq	12	// <=
#define	OP2_Less		13	// <
#define	OP2_AssRShift	14	// >>=
#define	OP2_RShift		15	// >>
#define	OP2_GreatOrEq	16	// >=
#define	OP2_Great		17	// >
#define	OP2_NotEqual	18	// !=
#define	OP2_Equal		19	// ==
#define	OP2_Assign		20	// =
#define	OP2_LogAND		21	// &&
#define	OP2_AssBitAND	22	// &=
#define	OP2_BitAND		23	// &
#define	OP2_LogOR		24	// ||
#define	OP2_AssBitOR	25	// |=
#define	OP2_BitOR		26	// |
#define	OP2_AssBitXOR	27	// ^=
#define	OP2_BitXOR		28	// ^
#define	OP2_Comma		29	// ,
//--- new for CE 2.0 ---
#define	OP2_Power		30	// **	Exponentiation
#define	OP2_AssPower	31	// **=	Exponentiation Assignment
#define OP2_LogXOR		32	// ^^	Logical XOR
#define	OP2_IdxRange	33	// :	Index Range

// ------------------
// 'category' values
// ------------------
#define	CAT_OBRACKET	0	// ( - is only used for parsing
#define	CAT_CBRACKET	1	// ) - is only used for parsing
#define	CAT_COMMA		2	// ,
#define	CAT_FUNC		3	// FuncVI
#define	CAT_COMMON		4	// general operation
#define	CAT_FASTFUNC	5	// FastFunc
//--- new for CE 2.0 ---
#define	CAT_INDEX		6	// Subscript Operator (array indexation)
#define	CAT_OINDEX		7	// [ - начало индекса (is only used for parsing)
#define	CAT_CINDEX		8	// ] - конец индекса (is only used for parsing)
#define	CAT_SEMICOLON	9	// ; - ends expression-statement (is only used for parsing)
#define	CAT_ATDELIM		10	// @ - indexes separator x[0..2@0..2] (is only used for parsing)
//--
// >>> #CE_3.0 2025-03-22
#define	CAT_INCPREF		11	// Prefix Increment/Decrement (++/--)
#define	CAT_INCPOST		12	// Postfix Increment/Decrement (++/--)
// <<< #CE_3.0 2025-03-22
//--
#define	CAT_OPERAND		0xFFFF	// ---


// ------------------
// 'direction' values
// ------------------
#define	DIR_LR			0
#define	DIR_RL			1

#endif
