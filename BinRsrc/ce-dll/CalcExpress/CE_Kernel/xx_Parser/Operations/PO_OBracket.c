/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


//#include "FastFuncSup.c"

// тела в конце файла
MgErr Proc_OBracket(	_CE_Instance *ip, int32 *expIdx, _Operation *op, Bool32 may_be_func);
int32 _FastFunc_Find(	_CE_Instance *ip, int32 idx );


// =====================================
// Выполняется если входная операция '('
// =====================================

MgErr PO_OBracket(	_CE_Instance	*ip,
					int32			*expIdx,
					_Operation		*op,
					Bool32			*prev_operand,	// *may_be_func
					Bool32			*prev_obracket,
					Bool32			maybe_func,
					LVBoolean		*err_f )
{
	MgErr	err;

	// !!!!!!!  18:56 19-03-03  !!!!!!!!!!!!!!!!!!!!!!!!
	if (*prev_operand && !maybe_func) {
		err = CE_Error(ip, "Call of nonfunction or missing operator before '('", NULL,NULL);
		*err_f = LVTRUE;
		return err;
	}

	if (err = Proc_OBracket( ip, expIdx, op, *prev_operand))
		return	err;

	*prev_operand	= FALSE;
	*prev_obracket	= TRUE;

	// Поместить '(' в вершину стека
	// =============================
	return	OpsStack_Push(ip, op);
}

// #############################################

MgErr Proc_OBracket(
			_CE_Instance	*ip,
			int32			*expIdx,
			_Operation		*op,
			Bool32			may_be_func )
{
	_FuncIdxOp	fi;
	LVBoolean	errf = LVFALSE;

	if (may_be_func) {
		fi.type		= FIO_TYPE_FUNC;
		fi.brLevel	= 1;
		fi.w_arg	= FALSE;
		fi.nArg		= 1;

		fi.FastFuncIdx = _FastFunc_Find( ip, *expIdx );

		if (fi.FastFuncIdx >= 0) {
			fi.nArg = 0;
			// *** удаляем имя функции из массива выражения
			ExAr_RemoveOneRecord( ip, expIdx );			// >>> #CE_3.0.4 2026-06-22
		}
		else {
			fi.misc = *expIdx-1;  // Index of Expression Element with a FuncVI name !19:23 22-03-02!
		}
		return	FIOStack_Push(ip, &fi);
	}

	FIOStack_GetTop(ip, &fi, &errf);
	fi.brLevel += 1;
	FIOStack_SetTop(ip, &fi, &errf);

	return noErr;
}

// #############################################
// 	ПОИСК FastFunc
int32 _FastFunc_Find( _CE_Instance *ip, int32 idx )
{
	LStrHandle	funcName;
	_ExpresElement	*el;

	// Определить имя функции
	// ======================
	if (--idx < 0)				return -1;
	el = &(ip->expArrDat[idx]);				// >>> #CE_3.0.4 2026-06-22
	if (el->IsOps)				return -1;
	if (el->opd.type != OPTYPE_IDENT)	return -1;

	funcName = getIdentString( &(el->opd));

	idx = __FastFunc_GetIndex(LStrBuf(*funcName), LStrLen(*funcName));

	if (idx < 0) {
		// === не нашли ===
		el->ops.category = CAT_FUNC;	// пометить идентификатор как имя FuncVI
	}
	return idx;
}
