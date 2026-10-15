/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <extcode.h>

#include <CalcExpress.h>
#include <ceError.h>
#include <Operand_Routines.h>
#include "..\CE_Kernel\xx_Evaluator\Indexation\Indexation.h"

#include <celib\ceMemLib.h>

// ==================================================

MgErr __fastcall	OpdStack_Init( _CE_Instance *ip)
{
	ip->OpdNum = 0;
	return	noErr;
}

// =========================================================================

MgErr __fastcall	__operand_prepare(_Operand *op)
{
	op->type = OPTYPE_DBL;
	op->num  = 0.0;
	op->iip  = NULL;		// !!!!!!!!!!!!!
	op->vpnt = NULL;		// >>> #CE_3.0.2 2026-06-19
	op->deref= FALSE;		// >>> #CE_3.0.5 2025-07-14
	// *** создать строку
	return LStr_Create(&(op->str),0);
}

MgErr __fastcall	__operand_decrease(_Operand *op)
{
	op->type = OPTYPE_DBL;
	op->num  = 0.0;
	op->iip  = NULL;		// !!!!!!!!!!!!!
	op->vpnt = NULL;		// >>> #CE_3.0.2 2026-06-19
	op->deref= FALSE;		// >>> #CE_3.0.5 2025-07-14
	if (op->str)
		return	LStr_Clear(op->str);
	return	noErr;
}

MgErr __fastcall	__operand_dispose(_Operand *op)
{
	op->type = OPTYPE_DBL;
	op->num  = 0.0;
	op->iip  = NULL;		// !!!!!!!!!!!!!
	op->vpnt = NULL;		// >>> #CE_3.0.2 2026-06-19
	op->deref= FALSE;		// >>> #CE_3.0.5 2025-07-14
	if (op->str)
		return	LStr_Destroy(op->str);
	return	noErr;
}

// -----------------------------------------------
// Функция подготавливает элементы стека операндов
// -----------------------------------------------
MgErr __fastcall	OpdStack_ActOperands(
				_CE_Instance	*ip,
				int32			action,
				int32			beg,
				int32			end)
{
	_Operand	*ep;
	MgErr		err = noErr;

//	ep = &((*(ip->OpdStk))->data[beg]);
	ep = &(ip->OpdStkDat[beg]);		// >>> #CE_3.0.2 2026-06-20

	for ( ; beg < end ; beg++, ep++) {

		switch (action) {

		case opdsCmd_Prepare :	// выделить память под строку
			err = __operand_prepare(ep);
			break;

		case opdsCmd_FreeMem :	// уменьшить память под строку
			err = __operand_decrease(ep);
			break;

		case opdsCmd_Dispose :	// освободить память под строку
			err = __operand_dispose(ep);
			break;
		}
		if (err) goto out;
	}
out:
	return err;
}

// ======================================================================================

// ==============================
// Функция создает стек операндов
// ==============================
MgErr __fastcall	OpdStack_CreateStack( _CE_Instance *ip)
{
	MgErr		err = noErr;

	// ### create array
	// >>> #CE_3.0.4 2026-07-05
	ip->OpdStk = NULL;
	if (err = ce_CreateOrResizeAlignedArray((UHandle*)&(ip->OpdStk), OPDSTACK_BLKSIZE, sizeof(_Operand), sizeof(float64), offsetof(_OpdArray,data)))
		goto out;
//	(*(ip->OpdStk))->dimSize = OPDSTACK_BLKSIZE;	// no need, ce_CreateOrResizeAlignedArray sets DimSize
	ip->OpdStkDat = &((*(ip->OpdStk))->data[0]);	// >>> #CE_3.0.2 2026-06-20
	ip->OpdStkLen = OPDSTACK_BLKSIZE;				// >>> #CE_3.0.2 2026-06-20

	// Подготовить элементы стека
	err = OpdStack_ActOperands(ip, opdsCmd_Prepare, 0, OpdStack_Size(ip));
out:
	ip->OpdNum = 0;
	return err;
}

// ---------------------------------
// функция уничтожает стек операндов
// ---------------------------------
MgErr	__fastcall	OpdStack_DestroyStack(	_CE_Instance	*ip )
{
	MgErr		err = noErr;

	if (!(ip->OpdStk)) goto out;

	// разрушить элементы стека
	err = OpdStack_ActOperands(ip, opdsCmd_Dispose, 0, OpdStack_Size(ip));

	// разрушить стек
	err = DSDisposeHandle(ip->OpdStk);
out:
	ip->OpdStkDat = NULL;		// >>> #CE_3.0.2 2026-06-20
	ip->OpdStkLen = 0;			// >>> #CE_3.0.2 2026-06-20
	ip->OpdStk = NULL;
	ip->OpdNum = 0;
	return err;
}

// ---------------------------------------
// функция изменяет размер стека операндов
// ---------------------------------------
MgErr	__fastcall	OpdStack_ResizeStack( _CE_Instance *ip, int32 newsize )
{
	int32	size = OpdStack_Size(ip);
	MgErr	err = noErr;

	if (newsize == size)
		return noErr;

//DbgPrintf(__FUNCTION__"(): %s - %d\n", newsize < size ? "compress" : " expand ", newsize);

	// *** Если массив уменьшается, то освободить не нужные LStrHandle
	if (newsize < size) {
		err = OpdStack_ActOperands(ip, opdsCmd_Dispose, newsize, size);
	}

	// >>> #CE_3.0.4 2026-07-05
	if (err = ce_CreateOrResizeAlignedArray((UHandle*)&(ip->OpdStk), newsize, sizeof(_Operand), sizeof(float64), offsetof(_OpdArray,data)))
		goto out;
//	(*(ip->OpdStk))->dimSize = newsize;					// no need, ce_CreateOrResizeAlignedArray sets DimSize
	ip->OpdStkDat = &((*(ip->OpdStk))->data[0]);		// >>> #CE_3.0.2 2026-06-20
	ip->OpdStkLen = newsize;							// >>> #CE_3.0.2 2026-06-20

	// *** Если массив расширяется, то подготовить новые операнды
	if (newsize > size) {
		err = OpdStack_ActOperands(ip, opdsCmd_Prepare, size, newsize);
	}
out:
	return err;
}

// ========================================
// функция добавляет один элемент в стек
// ========================================
MgErr __fastcall	OpdStack_PUSH( _CE_Instance *ip, _Operand *el)
{
	MgErr	err;

//	if (*err_f)	return noErr;

//DbgPrintf(__FUNCTION__"()\n");
//if (el->iip == NULL)	DbgPrintf(__FUNCTION__"(): op->iip == NULL, type == %d\n", el->type);
//if (el->str == NULL)	DbgPrintf(__FUNCTION__"(): op->str == NULL, type == %d\n", el->type);
	ip->OpdNum += 1;
	if (ip->OpdNum >= OpdStack_Size(ip)) {
		// ***  Если таблица заполнена, то расширить
		if (err = OpdStack_ResizeStack(ip, OpdStack_Size(ip) + OPDSTACK_BLKSIZE))
			goto out;
	}

	// >>> #CE_3.0.5 2026-07-16
	// actally, we need to reset indexation of an operand which is pushed to the Operand Stack
	// but _Operand.iip is a pointer which duplicates the value of respective _ExpressElement.opd.iip
	// so, we reset indexation in the source operand and then it will be pushed to the Operand Stack
	_IndexInfo_ResetIndexation(el);	
	// <<< #CE_3.0.5 2026-07-16

	// ***  заполнить элемент таблицы
	err = copy_operand( el, OpdStack_EPnt(ip, ip->OpdNum) );

//	_IndexInfo_ResetIndexation(el);		// >>> #CE_3.0.4 2026-07-05
out:
	return err;
}

// >>> #CE_3.0 2025-04-21
// ================================================
// if operand is Identifier, check existanse of respective variable
// ================================================
Bool32	__fastcall	OpdStack_VerifyOperand(	_CE_Instance *ip, _Operand *opd, LVBoolean *err_f)
{
	_VarInfo	*vip;
	Bool32		fGood = TRUE;

	if (opd->type == OPTYPE_IDENT) {
//DbgPrintf(__FUNCTION__"(): '%H'\n", getIdentString(opd));
		VARMAN_LOCK_SHARED();

		fGood = GetPointerToVariable2( ip, &vip, opd, err_f, NULL);		// >>> #CE_3.0.2 2026-06-19

		VARMAN_UNLOCK_SHARED();
	}
//else DbgPrintf(__FUNCTION__"(): CONST\n");
	return fGood;
}
// <<< #CE_3.0 2025-04-21

// #######################################################
// >>> #CE_3.0.5 2026-07-12
MgErr	OpdStack_OpdVarDataRef(_CE_Instance *ip, Bool32 enaVarRef, _Operand *opd_in, _OperandBase **val_out, LVBoolean *err_f, uInt32 *pVMLOCKFLAGS, uInt32 VMLOCKCODE)
{
	uInt32			vmlock = 0;
	_OperandBase	*vip;
	MgErr			err = noErr;

	*val_out = (_OperandBase*)opd_in;

	if (opd_in->type != OPTYPE_IDENT)
		return noErr;

	if (*pVMLOCKFLAGS == OPDVARDATAREF_NOLOCK) {
		*pVMLOCKFLAGS |= VMLOCKCODE;
		if      (VMLOCKCODE == OPDVARDATAREF_LOCKSOFT)	VARMAN_LOCK_SHARED();
		else if (VMLOCKCODE == OPDVARDATAREF_LOCKHARD)	VARMAN_LOCK_EXCLUSIVE();
	}

	if (IsOperandIndexed(opd_in)) {
		// !!! Copy indexed variable value !!!
		if ( GetPointerToIndexedVar2(ip, &vip, opd_in, err_f)) {
			err = GetIndexedValue(ip, vip, *val_out, opd_in->iip, err_f);
		}
		goto out;
	}
	// *** not-indexed variable
	if (GetPointerToVariable2(ip, &vip, opd_in, err_f, NULL)) {
//		if (enaVarRef && (opd_in->type != OPTYPE_DBL)) {
		if (enaVarRef) {
			// !!! don't copy variable value !!!
			*val_out = vip;
//DbgPrintf("use VarRef '%H'\n", getIdentString(opd_in));
		}
		else {
			// !!! Copy variable value !!!
			err = Var_Copy(	*val_out, vip);
		}
	}
out:
	return err;
}
// <<< #CE_3.0.5 2026-07-12
