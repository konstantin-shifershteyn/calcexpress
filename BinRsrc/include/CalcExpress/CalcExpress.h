/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef __CalcExpress__
#define __CalcExpress__

#include <stdlib.h>
#include <string.h>

#include <extcode.h>
#include <lvsup\lvsup.h>

#include <CE_ErrorCodes.h>
#include <iDataTypes.h>
#include <Operands.h>
#include <Operators.h>
#include <Expression.h>
#include <FuncIdxOp.h>

#define	BitFALSE	0
#define	BitTRUE		1

// Signature of CalcExpress Instance Context
// -----------------------------------------
#define	CE_CONTEXT_SIGNATURE	'CEcx'

// ===============================
// Parameters influence Evaluation
// ===============================
typedef struct {
	unsigned int	en_cpx				: 1;	// Run: Enables complex arithmetic
	unsigned int	autoVar				: 1;	// Run: Enables auto-creating variables
	unsigned int	ErrorOfParsing		: 1;	// Parse/Run: Error detected during parsing
	unsigned int	testSyntaxOnly		: 1;	// Parse: is it used ???
	unsigned int	ParsingInProgress	: 1;	// Parse/Run: influence error messages
	unsigned int	enaNamedConst		: 1;	// Parse: enable Named Constants
	unsigned int	reserved			: 26;
} _CE_Proc_Params;

// ===================================================
// Структура описывает Контекст Экземпляра CalcExpress
// ===================================================
// >>> #CE_3.0.2 2026-06-12
typedef struct {
	uInt32					signature;	// contains signature if context is valid
	int32					SyncTag;	// 
	Bool32					CancelTag;	// 
							// ---
	int32					VCHandle;	// handle of Variable Context
							// ---
	LStrHandle				str;		// Expression String	//строка выражения
	_ExpArrayHandle			expArr;		// Handle to Expression Array		//массив элементов выражения
	_ExpresElement			*expArrDat;	// pointer to data array in the Expression Array [#CE_3.0.4 2026-06-22]
	int32					expArrLen;	// size of data array in the Expression Array [#CE_3.0.4 2026-06-22]
	Bool32					old_enaNamedConst;
//	int32					__pad_0__;
							// ---
	_OpdArrayHandle			OpdStk;		// Handle to Stack of Operands	//стек операндов
	_Operand				*OpdStkDat;	// pointer to data array in the Stack of Operands	[#CE_3.0.2 2026-06-20]
	int32					OpdStkLen;	// size of data array in the Stack of Operands	[#CE_3.0.2 2026-06-20]
	int32					OpdNum;		// actual number of elements in the Stack of Operands	//кол-во операндов в стеке
//	int32					__pad_1__;
							// ---
	_OpsArrayHandle			OpsStk;		// Handle to Stack of Operators	//стек операций
	_Operation				*OpsStkDat;	// pointer to data array in the Stack of Operators	[#CE_3.0.2 2026-06-20]
	int32					OpsStkLen;	// size of data array in the Stack of Operators	[#CE_3.0.2 2026-06-20]
	int32					OpsNum;		// actual number of elements in the Stack of Operators	//кол-во операций в стеке
//	int32					__pad_2__;
							// ---
	_FuncIdxOp_ArrayHandle	FIOStk;		// Handle to Stack of Functions & Indexations	//общий стек функций и операторов индексирования
	_FuncIdxOp				*FIOStkDat;	// pointer to data array in the Stack of Functions & Indexations	[#CE_3.0.2 2026-06-20]
	int32					FIOStkLen;	// size of data array in the Stack of Functions & Indexations	[#CE_3.0.2 2026-06-20]
	int32					FIONum;		// actual number of elements in the Stack of Functions & Indexations	//кол-во элементов в стеке
//	int32					__pad_3__;
							// ---
	int32					ErrCode;	// код ошибки
	int32					__pad_4__;
	LStrHandle				ErrMsg;		// сообщение об ошибке
							// ---
	_Operand				LOpd;		// временные операнды
	_Operand				ROpd;
							// ---
	_CE_Proc_Params			procPars;	//params;		// Evaluation Parameters
//	int32					__pad_5__;
	uInt32					writerID;		// this value is saved to variables' writerID when a value is written to a variable [#CE_3.0.7 2026-07-23]
	int32					oldVCH;			// used to check VC Handle changed between calls of CE Instance [#CE_3.0.2 2026-06-19]
	uInt32					vmChangeCnt;	// used to check VarMan variable(s) deleted, this can make variable pointers invalid [#CE_3.0.2 2026-06-19]
							// ---
//	int32					reserved[7];
							//
} _CE_Instance, *_CE_InstancePnt;
// <<< #CE_3.0.2 2026-06-12

// ===============================

// >>> #CE_3.0.2 2026-06-20

#define	OPDSTACK_BLKSIZE		64
#define	OpdStack_Size(ip)		((ip)->OpdStkLen)
#define	OpdStack_EPnt(ip,idx)	(&((ip)->OpdStkDat[(idx)]))

#define	OPSSTACK_BLKSIZE		64
#define	OpsStack_Size(ip)		((ip)->OpsStkLen)

#define	FIOSTACK_BLKSIZE		64
#define	FIOStack_Size(ip)		((ip)->FIOStkLen)

// <<< #CE_3.0.2 2026-06-20


// ==================================================
// VI Interface definitions
// ==================================================

// ***	Code Generation parameters
typedef struct {
	LVBoolean	fEnaNamedConst;		// enable Named Constants [#CE_3.1.0-r6 2026-09-28]
#if defined(_M_X64)
	uInt8		pad_3[3];			// >>> #CE_3.0 2024-12-12 : pading bytes
#endif
} _CEVI_ParseParams;

typedef	struct {
	LVBoolean	autoVar;		// Enable auto creation of variables
	LVBoolean	en_cpx;			// Enable complex arithmetic
} _CEVI_CalcParams;

#endif
