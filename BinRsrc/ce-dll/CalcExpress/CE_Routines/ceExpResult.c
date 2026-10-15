/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <extcode.h>

#include <CEKernel.h>
#include <CEError.h>
#include <ceExpResult.h>

#include <celib\ceMemLib.h>
#include <celib\FlattenData.h>


void	clear_LVString(	LStrHandle  str);
void	value_on_error( float64 *num, LStrHandle str, uInt32 *type, LVBoolean *err_f);


// ************************** Subroutine Entry *****************************

MgErr __cdecl	ceExpResult(
					 _CE_Instance	*ip,
					 float64		*num,
					 LStrHandle		str,
					 uInt32			*type,
					 LVBoolean		*err_f)
{
	_Operand	*opdPnt;
	_Operand	*resPnt, resData;		// >>> #CE_3.0 2025-04-21
	MgErr		err = noErr;

	// ***  (1) Эти проверки Выполнить первыми !!!
	if (str) {
		if (!(*str))	str	= NULL;
	}

	// ***  (2) не раньше чем (1)
	if (*err_f) {
		value_on_error( num, str, type, err_f);
		goto out;
	}

	// Если стек пустой
	// ================
	if (ip->OpdNum <= 0) {
		// ****  Stack is empty  ****
		// это значит что
		// - либо была ошибка и ее уже обнаружили (и сюда попасть не могли),
		// - либо была пустая строка выражения
		// - либо скрипт заканчивается с ';'
		// т.о. новую ошибку не порождаем!!!
		value_on_error( num, str, type, NULL);	// установить значения по умолчанию
		*err_f	= LVFALSE;			// но сбросить признак ошибки
		goto out;
	}
	// Если в стеке больше одного операнда, то ошибка
	// ==============================================
	if (ip->OpdNum > 1) {
		err = CE_Error(ip, "Too many operands remained after evaluation (missed operator?)", NULL, NULL);
		value_on_error( num, str, type, err_f);
		goto out;
	}
	// Получить указатель на операнд
	// -----------------------------
	opdPnt = OpdStack_EPnt(ip, 1 /*ip->OpdNum*/ );
// >>> #CE_3.0 2025-04-21
	// Idea of the code is to avoid copying strings from VarMan to 'opdPnt->str'
	// in GetVariable function below, when it's not requested ('str' argument is NULL).
	// GetVariable() allows a destination string be NULL.
	if (str) {
		// resPnt points to real operand where '->str' is not NULL
		resPnt = opdPnt;
	}
	else {
		// resPnt ponts to fake operand where '->str' member is NULL
		resPnt = &resData;
		memset(&resData, 0, sizeof(resData));
	}
// <<< #CE_3.0 2025-04-21

//#define _TRACE_ME
#if defined(_TRACE_ME)
	#include "ceExpResult_Dbg.c"
	*err_f = LVTRUE;
	goto out;
#undef	_TRACE_ME
#endif

// >>> #CE_3.0 2025-04-21
	// Определить значение выражения
	// =============================
	if (opdPnt->type == OPTYPE_IDENT) {
		err = GetVariable( ip, opdPnt, (_OperandBase*)resPnt, err_f);	// >>> #CE_3.0 2025-04-21
		if (err || *err_f) {
			value_on_error( num, str, type, err_f);
			goto out;
		}
	}
	else {
		resPnt->type	= opdPnt->type;
		resPnt->num		= opdPnt->num;
	}
	// Сформировать выходные значения
	// ==============================
	*type	= resPnt->type;

	// If Expression type is DBL Scalar
	// --------------------------------
	switch(resPnt->type) {
		case OPTYPE_DBL :
		case OPTYPE_IDXRANGE :
			*num = resPnt->num;
			if (str) {
				clear_LVString(str);
			}
			goto out;
	}

	// If Expression datatype is either String or one of Complex datatypes
	// -----------------------------------------------------------
	*num = 0;				// numeric value
	if (!str)				// The End if invalid LStrHandle
		goto out;

	if (err = LStr_Copy(str, resPnt->str))	// Скопировать строку (данные)
		goto out;

	// *** В CalcExpress данные храняться как unflatten ***
	_flat_data( resPnt->type, str, err_f);
	if (*err_f) {
		err = CE_Error(ip, "Type of expression result doesn't match to its flattened data."
			   " Type Code = %d <flat>", (LStrHandle)(size_t)resPnt->type, NULL);
		goto out;
	}
// <<< #CE_3.0 2025-04-21
out:
	if (err) {
		CE_Error_LVMgErr( ip, err, err_f);	//save LabVIEW error code
	}
	return err;
}
//////////////////////////////////////////////////////////////////////////////////////

// ======================================================================
void clear_LVString(	LStrHandle  str)
{
	if (str) {				// if LStrHandle is valid
		if (LStrLen(*str))	// if string is not empty
			LStr_Clear(str);
	}
}

void value_on_error( float64 *num, LStrHandle str, uInt32 *type, LVBoolean *err_f)
{
	if (err_f)	*err_f	= LVTRUE;	// if non-NULL pointer
	*type	= OPTYPE_DBL;
	*num	= 0;
	clear_LVString(str);
}
