/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Preparing parameters for FuncVI call
// -----------------------------------

#include <extcode.h>

#include <CalcExpress.h>
#include <CEError.h>
#include <ceAccessContext.h>
#include <lviDataConversion.h>
#include <Operand_Routines.h>

#include <celib\ceMemLib.h>


// *** Body resides at the end of file
MgErr viFuncVIPars_CopyOperand( _Operand *src, _OperandVI *dst);


// ==================================
MgErr	__cdecl	viFuncVIPars(
				LVRefNum	*pRefNum,	//_CE_InstancePnt		*InstanceRef,
				int32				*N_Opds,
				LStrHandle			FuncName,
				_OpdVIArrayHandle	ArgList,
				LVBoolean			*err_f)
{
	_CE_Instance		*ip;
	int32				n_args;
	int32				i;
	_Operand			*opd;
	_OperandVI			*opdar;
	MgErr				err = noErr;

	if (*err_f)
		return noErr;

	// ***** Convert LVRefNum to Instance Reference (CE 3.0)
	if ( !(ip = (_CE_Instance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// *****  Захватить контекст
	// -------------------------
	if (!_CEContext_Lock(ip)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*err_f = LVTRUE;	// error
		return noErr;
	}

	// Определить кол-во аргументов функции
	// ====================================
	n_args = (*N_Opds) - 1;
	if (n_args < 0) {
		*err_f = LVTRUE;
		err = CE_InternalError(ip, "'n_args' in the function call is less than zero", NULL,NULL);
		goto out;
	}

	opdar = (*ArgList)->data;

	// заполнить массив аргументов
	// ===========================
	for (i = n_args-1; i >= 0; i--) {
		opd = OpdStack_TopPnt(ip, err_f);
		OpdStack_DecLength(ip, err_f);
		if (*err_f)
			goto out;
		
		if (err = viFuncVIPars_CopyOperand(opd, &(opdar[i])) )
			goto out;
		
		// В CalcExpress данные храняться как unflattened
		_To_Flat_Data( ip, &(opdar[i]), err_f);
		if (*err_f)
			goto out;
	}
	// определить имя функции
	// ======================
	opd = OpdStack_TopPnt(ip, err_f);
	if (*err_f) {
		err = CE_Error(ip, "Function argument expected", NULL,NULL);
		goto out; 
	}
	if (opd->type != OPTYPE_IDENT) {
NonFunc:
		err = CE_Error(ip, "Call of nonfunction", NULL,NULL);
		*err_f = LVTRUE;
		goto out;
	}
	if ( ! LStrLen(*(opd->str)) )
		goto NonFunc;

	// ***	copy function name
	err = LStr_Copy( FuncName, opd->str );
	
out:
	if (err) {
		// ***	If memory error occurred
		CE_Error_LVMgErr(ip, err, err_f);
	}
	// *****  Освободить контекст
	_CEContext_UnlockReliable(ip);
	return err;
}

// ================================================
// Копирует Операнд
// ================================================
MgErr viFuncVIPars_CopyOperand(_Operand *src, _OperandVI *dst)
{
	int32	size;
	MgErr	err;

	dst->num = src->num;
	dst->type = src->type;
	switch(dst->type) {
		case OPTYPE_DBL :
		case OPTYPE_IDXRANGE :	size = 0;
								break;
		default :				size = LStrLen(*(src->str));
								break;
	}

	// скопировать строку (если нужно)
	// если не надо, то уменьшить размер до нуля
	if (!(err = LStr_ResizeOrCreate(&(dst->str), size)) ) {
		if (size > 0)	// >>> #CE_3.1.0 2026-08-02
			_MoveBlock( LStrBuf(*(src->str)), LStrBuf(*(dst->str)), size);
	}
	return err;
}
