/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include "Indexation.h"

#include "Subr\PrepareResult1D.c"
#include "Subr\PrepareResult2D.c"
#include "GetIndexedValue.c"
#include "SetIndexedValue.c"

// ====================================================================
//		Обработчик Оператора Индексации Массивов
// ====================================================================

MgErr IndexingOperator(
					_CE_Instance	*ip,
					_Operation		*op,
			 		LVBoolean		*err_f)
{
	int32			idxNum;
	_Operand		*arr, *io1, *io2;
	_indexes_info	idxs, *idxPnt;
	const char		*errmsg	= NULL;
	LVBoolean		errf = LVFALSE;
	MgErr			err	= noErr;

	arr = io1 = io2 = NULL;

	idxs.idx1.alloc = idxs.idx2.alloc = FALSE;
	idxs.idx1.erMsg = idxs.idx2.erMsg = NULL;

	idxNum = op->n_ops - 1;

	// указатель на массив (он же - результат)
//	arr = &((*(ip->OpdStk))->data[ip->OpdNum - idxNum]);
	arr = OpdStack_EPnt(ip, ip->OpdNum - idxNum);	// >>> #CE_3.0 2025-04-21

	// ---------------------------------------
	// Определить значения индексных выражений
	// ---------------------------------------
	switch(idxNum) {
		case 2:
			io2 = OpdStack_EPnt(ip, ip->OpdNum-(idxNum-2));	// >>> #CE_3.0 2025-04-21
			if (io2->type == OPTYPE_IDENT) {
				err = GetVariable(ip, io2, (_OperandBase*)io2, &errf);
				if (err || errf) goto out;
			}
		case 1:
			io1 = OpdStack_EPnt(ip, ip->OpdNum-(idxNum-1));	// >>> #CE_3.0 2025-04-21
			if (io1->type == OPTYPE_IDENT) {
				err = GetVariable(ip, io1, (_OperandBase*)io1, &errf);
				if (err || errf) goto out;
			}
			break;
		default:
			if (idxNum < 1)
					errmsg = "Internal Error: idxNum < 1 condition is met during Subscript operator processing in "__FUNCTION__"()";
			else	errmsg = "Internal Error: idxNum > 2 condition is met during Subscript operator processing in "__FUNCTION__"()";
			goto out;
	}

	// --------------------------------
	//	Индексация переменных
	// --------------------------------
	if (arr->type == OPTYPE_IDENT) {

		if (!(arr->iip)) {
			err = CE_InternalError(ip, "Array Indexing: arr->iip == NULL",NULL,NULL);
			errf = LVTRUE;
			goto out;
		}
		idxPnt = arr->iip;

		// ------------------------------------------
		// ЕСЛИ переменная УЖЕ индексировалась, то
		// выполнить предыдущую индексацию и затем текущую индексацию
		// ------------------------------------------
		if(idxPnt->qty) {
			err = GetVariable(ip, arr, (_OperandBase*)arr, &errf);
			//_IndexInfo_FreeValues(arr);			// очистить индексную информацию (??? надо-ли ???)
			// >>> #CE_3.0.5 2025-07-15 : disabled: indexation is only used with OPTYPE_IDENT, but if we keep var name in the Operands Stack, we also must keep its indexation
			//_IndexInfo_ResetIndexation(arr);		// >>> #CE_3.0.4 2026-07-05
			goto subscript_non_var;		// затем выполнить индексацию не-переменной
		}

		// ------------------------------------------
		// ЕСЛИ переменная еще не индексировалась, то
		// сохранить информацию об индексах
		// ------------------------------------------
		idxPnt->compound = op->misc;		// Тип индексации 2D массивов

		switch (idxPnt->qty = idxNum) {		// подготовить и сохранить индексы
			case 2:
				if (err	= _prepare_index(io2, &(idxPnt->idx2)))
					goto out;
				if (errmsg	= idxPnt->idx2.erMsg) {
					idxPnt->idx2.erMsg = NULL;
					goto out;
				}
			case 1:
				if (err	= _prepare_index(io1, &(idxPnt->idx1)))
					goto out;
				if (errmsg	= idxPnt->idx1.erMsg) {
					idxPnt->idx1.erMsg = NULL;
					goto out;
				}
		}
		goto out;
	}

subscript_non_var:
	// ------------------------
	// Индексация не-переменных
	// ------------------------
	idxs.compound = op->misc;	// Тип индексации 2D массивов
	switch (idxs.qty = idxNum) {
		case 2 : 
			if (err		= _prepare_index(io2, &idxs.idx2))	goto out;
			if (errmsg	= idxs.idx2.erMsg)					goto out;
		case 1 :
			if (err		= _prepare_index(io1, &idxs.idx1))	goto out;
			if (errmsg	= idxs.idx1.erMsg)					goto out;
	}
	err = GetIndexedValue(ip, (_OperandBase*)arr, (_OperandBase*)arr, &idxs, &errf);

out:
	// *** Изменить размер стека ***
	ip->OpdNum -= idxNum;

	if (idxs.idx1.alloc) {
		//DbgPrintf("Indexation: Discard 'idxs.idx1'\n");
		_dispose_index_array(&(idxs.idx1));		// >>> #CE_3.0.4 2026-07-05
	}
	if (idxs.idx2.alloc) {
		//DbgPrintf("Indexation: Discard 'idxs.idx2'\n");
		_dispose_index_array(&(idxs.idx2));		// >>> #CE_3.0.4 2026-07-05
	}

	if (err) return err;

	if (errmsg) {
		errf	= LVTRUE;
		if (err = CE_Error(ip, "%s", (LStrHandle)errmsg, NULL))
			return err;		//goto out;
	}
	if (errf) {
		if (arr && (arr->type == OPTYPE_IDENT))
				err = CE_Error(ip, " : %H[..]", getIdentString(arr), NULL);
		else	err = CE_Error(ip, " : <expression>[..]", NULL, NULL);
		*err_f = errf;
	}
	return	err;
}
