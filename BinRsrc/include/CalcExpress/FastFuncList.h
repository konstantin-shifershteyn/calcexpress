/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__FAST_FUNC_LIST__
#define	__FAST_FUNC_LIST__

#include <extcode.h>

#include <ceKernel.h>


// =========================================
// Тип определяет указатель на _FastFuncInfo
// =========================================
typedef struct _FastFuncInfo_Record *pntFFI;


// =========================================================================
// Тип определяет указатель на главную процедуру-обработчик FastFunc функции
// =========================================================================
typedef	MgErr	(*_func_proc)(	_CE_Instance	*ip,
				pntFFI		fInfo,		//указатель на описание ф-ии
				int32		arg_qty,	//кол-во аргументов
				LVBoolean	*err_f      );

// ========================================================
// Типы определяют указатели на частные исполнители функций
// ========================================================
typedef MgErr	(*_f_dbl)();
typedef MgErr	(*_f_cdb)();


// ==================================================
// Тип определяет информацию о функции: _FastFuncInfo
// ==================================================
#define	FF_MaxNameLen	sizeof(uInt64)	// Maximum length of FastFunc function name
typedef struct _FastFuncInfo_Record
{
	union {
		char		name[FF_MaxNameLen];	// Buffer for function name
		uInt64		ntag;					// 64-bit integer representation of function name
	};
	int32		minArg;			// minimum number of arguments
	int32		maxArg;			// maximum number of arguments
	_func_proc	processor;		// main processor function
	_f_dbl		exec_dbl;		// dbl sub-processor
	_f_cdb		exec_cdb;		// cdb sub-processor
} _FastFuncInfo;

// Macros used in definition of the FastFunc List constant (FuncList[])
#define	_FastFuncDef( _name, _minArg, _maxArg, _proc, _ex_dbl, _ex_cdb ) 	{ _name, _minArg, _maxArg, _proc, _ex_dbl, _ex_cdb }


// ============================================================

#ifdef	__cplusplus
extern "C" {
#endif

// ***  Массив описателей функций  ***
// ===================================
extern	_FastFuncInfo	FuncList[];		// array of FastFunc descriptors
extern	int32			FuncListLen;	// number of FastFunc functions

// ***  The function returns an index of a FastFunc function in the FuncList
// ***  if a search fails, the function returns -1
extern	int32	__FastFunc_GetIndex(char *name, int32 len);

#ifdef	__cplusplus
extern "C" {
#endif

#endif
