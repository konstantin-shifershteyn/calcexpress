/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__OPERAND_ROUTINES_H__
#define	__OPERAND_ROUTINES_H__

#include <extcode.h>

#include <CalcExpress.h>
#include <ceKernel.h>

#include <Identifier_Routines.h>
#include <Index_Routines.h>

#include <celib\ceMemLib.h>


// ##########  CE_Kernel\xx_Evaluator\Operand_Routines.c  ##########
MgErr	__fastcall	__operand_prepare(_Operand *op);
MgErr	__fastcall	__operand_decrease(_Operand *op);
MgErr	__fastcall	__operand_dispose(_Operand *op);

// ------------------------------------------
// *** Copy operand
// >>> #CE_3.0.4 2026-07-02 : much faster on 32-bit platform
__forceinline MgErr __fastcall copy_operand( _Operand *src, _Operand *dst)		// >>> #CE_3.0.2 2026-06-09
{
//if (((size_t)&(src->num) % 8) != 0)	DbgPrintf(__FUNCTION__"(): &(src->num) is not aligned\n");
//if (((size_t)&(dst->num) % 8) != 0)	DbgPrintf(__FUNCTION__"(): &(dst->num) is not aligned\n");

// >>> #CE_3.1.0 2026-08-21 :
//	indexing is only used for OPTYPE_IDENT...
//	BUT, the top element of the Operand Stack can be converted to OPTYPE_IDENT (e.g. idn() function)
//	SO, we copy the pointer to index info (iip) for all data types
	dst->iip = src->iip;					// pointer to index info
	switch( dst->type = src->type ) {		// data type
		case OPTYPE_DBL :
		case OPTYPE_IDXRANGE :
				dst->num = src->num;		// numeric value
				dst->vpnt = NULL;			// >>> #CE_3.0.2 2026-06-19
				return noErr;
		case OPTYPE_IDENT :
				dst->deref= src->deref;				// >>> #CE_3.0.5 2025-07-14
//				dst->iip  = src->iip;				// pointer to index info	// >>> #CE_3.0.5 2025-07-14
				dst->vpnt = src->vpnt;				// >>> #CE_3.0.2 2026-06-19
				if ((dst->hdl = src->hdl) != NULL)	// если handle имени в поле hdl, то готово
					return noErr;					// иначе надо скопировать всю строку
//DbgPrintf(__FUNCTION__"(): copy IDENT '%H'\n", src->str);
				break;
		default:
				dst->num = src->num;		// numeric value (???)
				dst->vpnt = NULL;			// >>> #CE_3.0.2 2026-06-19
//DbgPrintf(__FUNCTION__"(): copy data str\n");
				break;
	}
//	return	LStr_Copy(dst->str, src->str);
	return	ce_CopyOrCreate_OperandString(&(dst->str), src->str, dst->type);		// >>> #CE_3.0.4 2026-07-06
}

// ###################################
MgErr	__fastcall	OpdStack_Init( _CE_Instance *ip);
MgErr	__fastcall	OpdStack_CreateStack( _CE_Instance *ip);
MgErr	__fastcall	OpdStack_DestroyStack( _CE_Instance *ip );
MgErr	__fastcall	OpdStack_ResizeStack( _CE_Instance *ip, int32 newsize );


// ***	Возвращает указатель на верхний элемент стека
__forceinline	_Operand * OpdStack_TopPnt ( _CE_Instance *ip, LVBoolean *err_f)		// >>> #CE_3.0.2 2026-06-09
{
	if (ip->OpdNum <= 0)	*err_f = LVTRUE;
	return  OpdStack_EPnt(ip, ip->OpdNum);		// указатель на верхний эл-т стека
}

// ***	Уменьшает кол-во элементов в стеке
__forceinline	void	OpdStack_DecLength( _CE_Instance *ip, LVBoolean *err_f)			// >>> #CE_3.0.2 2026-06-09
{
	if (ip->OpdNum <= 0)	*err_f = LVTRUE;
	//if (!(*err_f))			ip->OpdNum -= 1;	// уменьшить кол-во элементов
	else			ip->OpdNum -= 1;	// уменьшить кол-во элементов
}

// *** returns TRUE if an operand is indexed
__forceinline	Bool32	IsOperandIndexed (_Operand *opd)
{	return  opd->iip->qty;	}

// *** clears indexing info for an operand
__forceinline	void	ClearOperandIndex(_Operand *opd)
{	opd->iip->qty = 0;	}

// *** swaps two operands
__forceinline	MgErr	Swap_Operands(_Operand *opd1, _Operand *opd2)
{
	_Operand tmp = *opd1;
	*opd1 = *opd2;
	*opd2 = tmp;
	return noErr;
}

// *** Returs an operand value
__forceinline	MgErr	OpdStack_OpdValue(_CE_Instance *ip, _Operand *opd, LVBoolean *err_f)	// >>> #CE_3.0.2 2026-06-09
{
	if (opd->type == OPTYPE_IDENT) {
		return GetVariable( ip, opd, (_OperandBase*)opd, err_f);
	}
	return noErr;
}

MgErr	__fastcall	OpdStack_PUSH(_CE_Instance *ip, _Operand *el);
Bool32	__fastcall	OpdStack_VerifyOperand(_CE_Instance *ip, _Operand *opd, LVBoolean *err_f);
MgErr	__fastcall	OpdStack_ActOperands(_CE_Instance *ip, int32 action, int32 beg, int32 end);
// Action code for OpdStack_ActOperands() subroutine
enum {
	opdsCmd_Prepare = 0,
	opdsCmd_FreeMem,
	opdsCmd_Dispose
};

// >>> #CE_3.0.5 2026-07-12
//MgErr	OpdStack_NonIndexedOpdVarDataRef(_CE_Instance *ip, _Operand *opd_in, _Operand **val_out, LVBoolean *err_f, uInt32 *pVMLOCKFLAGS);
#define	OPDVARDATAREF_NOLOCK	0
#define	OPDVARDATAREF_LOCKSOFT	1
#define	OPDVARDATAREF_LOCKHARD	2
MgErr	OpdStack_OpdVarDataRef(_CE_Instance *ip, Bool32 enaVarRef, _Operand *opd_in, _OperandBase **val_out, LVBoolean *err_f, uInt32 *pVMLOCKFLAGS, uInt32 VMLOCKCODE);
// <<< #CE_3.0.5 2026-07-12

#endif
