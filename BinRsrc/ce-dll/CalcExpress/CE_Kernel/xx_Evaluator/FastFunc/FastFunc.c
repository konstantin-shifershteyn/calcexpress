/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// -------  Functions in this file ----------
MgErr	BuildInFunc	(	_CE_Instance *ip, int32 funcIndex, int32 argNum, LVBoolean *err_f);
MgErr	_FastFunc_IndirectCall(	_CE_Instance *ip, int32 funcIndex, int32 argNum, LVBoolean *err_f);
int32	_FastFunc_GetIndex(	_CE_Instance *ip, int32 opdNum);
// ------------------------------------------

#include "FuncTemplate.h"
#include "FuncTemplate2a.h"

//	#include "++FuncOperTempl.h"

#include "FastFunc_Subr.c"

#include "fmath1.c"
#include "fmath2.c"
#include "fmath2cdb.c"
#include "fmath30.c"
#include "fmathN.c"
#include "fmathNx.c"
#include "fmathNx-LVErr.c"
#include "fgeneralNx.c"

#include "f_op_1a_dbl+cdb.c"
#include "f_op+func_2a_dbl.c"
#include "f_op+func_2a_cdb.c"
#include "f_Math.c"
#include "f_Trig.c"
#include "f_Logarithm.c"
#include "f_Complex.c"
#include "f_AverageArg.c"

#include "f_Statistical.c"

#include "arrays\arrays.c"
#include "strings\strings.c"
#include "time\time_func.c"
#include "variables\vfunc_common.c"

#include "analysis\analysis.c"

#include "f_CE_Info.c"
#include "f_Misc.c"


// =========================================
// The function performs a FastFunc function
// =========================================
MgErr BuildInFunc(	_CE_Instance	*ip,
					int32			funcIndex,
					int32			argNum,
			 		LVBoolean		*err_f )
{
	_FastFuncInfo	*func;
	LVBoolean		errf = LVFALSE;
	MgErr			err;

//	if (*err_f)   return noErr;

	func = &(FuncList[funcIndex]);

	if (argNum < func->minArg) {
		err = CE_Error( ip, "Too few parameters in the call of function", NULL,NULL);
		errf = LVTRUE;
		goto out;
	}
	if (argNum > func->maxArg) {
		err = CE_Error( ip, "Too many parameters in the call of function", NULL,NULL);
		errf = LVTRUE;
		goto out;
	}
	// === Выполнение функции
	err = func->processor(ip, func, argNum, &errf);

out:	// ===== ВЫХОД =====
	if (errf) {
		if (!err) {
			// Добавить имя функции в конец сообщения об ошибке
			err = CE_Error(ip, ": %.8s()", (LStrHandle)func->name, NULL);
		}
		*err_f = errf;
	}
	return err;
}

// ==========================================================
// Function returns an index of FastFunc function
// if a search failed, the function returns -1
// ==========================================================
int32	_FastFunc_GetIndex(	_CE_Instance *ip, int32 opdNum)
{
	_Operand	*opd;
	LStrHandle	name;

	if (opdNum > ip->OpdNum)
		return -1;

	// Get pointer to operand containing a function name
	opd = OpdStack_EPnt(ip, ip->OpdNum - opdNum + 1);

	name = getIdentString(opd);

	return __FastFunc_GetIndex( LStrBuf(*name), LStrLen(*name) );
}

// =========================================================================
// Function performs FastFunc function when it is indirectly called
// An indirect call implies that a function name is in the stack of operands
// =========================================================================
MgErr	_FastFunc_IndirectCall(
				_CE_Instance	*ip,
				int32			funcIndex,
				int32			argNum,
				LVBoolean		*err_f )
{
	_Operand	*funcResult, *funcName;
	MgErr		err;

	err = BuildInFunc( ip, funcIndex, argNum-1, err_f);
	if (err || *err_f)
		goto out;

	// ---------------------------------------------------
	// Remove the function name from stack of operands
	// and put the function result to the top of the stack
	// ---------------------------------------------------
	funcResult	= OpdStack_TopPnt( ip, err_f);	// pointer to function result
	OpdStack_DecLength( ip, err_f );		// pop result
	funcName	= OpdStack_TopPnt( ip, err_f);	// pointer to function name
	if (!(*err_f)) {
		// puts function result to the top of operand stack
		Swap_Operands(funcResult, funcName);
	}
	else {
		err = CE_InternalError(ip, "Operands Stack is empty",NULL,NULL);
	}
out:
	return err;
}
