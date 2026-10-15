/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <celib\ceTime.h>


// ==================================
//	GetTicks()
// ==================================
MgErr f_GetTicks(	_CE_Instance	*ip,
					_FastFuncInfo	*fInfo,
					int32			arg_qty,	// кол-во аргументов
					LVBoolean		*err_f)
{
	// *** функция без аргумента
	if (arg_qty) {				// просто заглушка
		ip->OpdNum -= arg_qty;	// убрать все(!!!) из стека
	}
	// *** результат
	return	ff_PseudoArg(ip, (float64)MillisecsTimer());
}

// ==================================
//	GetTime()
// ==================================
MgErr f_GetTime(	_CE_Instance	*ip,
					_FastFuncInfo	*fInfo,
					int32			arg_qty,	// кол-во аргументов
					LVBoolean		*err_f)
{
	// *** функция без аргумента
	if (arg_qty) {				// просто заглушка
		ip->OpdNum -= arg_qty;	// убрать все(!!!) из стека
	}
	// *** результат
	return	ff_PseudoArg(ip, __GetTime_Labview_DBL());
}

