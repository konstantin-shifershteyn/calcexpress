/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <VMEngine_Routines.h>
#include <Identifier_Routines.h>
#include "Indexation\Indexation.h"

#include "Indexation\GetIndexedValue.c"
#include "Indexation\SetIndexedValue.c"

// >>> #CE_3.0.2 2026-06-19
// ==============================
__forceinline	void	__ClearVarPointer(_Operand *opd)
{
	opd->vpnt = NULL;
}
// ==============================
__forceinline	void	__RefreshVarPointer(_Operand *opd, uChar *namePnt, int32 nameLen, _VarInfo *vip)
{
	if ((opd->type == OPTYPE_IDENT)	&& (opd->vpnt == NULL))
		if (LStr_CmpWithMem(getIdentString(opd), namePnt, nameLen))
			opd->vpnt = vip;
}
// ==============================
void	__fastcall	RefreshVarPointers(_CE_Instance *ip, _Operand *var)
{
	int32			i;
	_ExpresElement	*expData	= ip->expArrDat;			// >>> #CE_3.0.4 2026-06-22
	int32			expSize		= ip->expArrLen;			// >>> #CE_3.0.4 2026-06-22
	uChar			*namePnt	= LStrBuf(*getIdentString(var));
	int32			nameLen		= LStrLen(*getIdentString(var));

//DbgPrintf("RefreshVarPointers: '%H'\n", getIdentString(var));
	// refresh Expression Array
	for (i=0; i < expSize; i++) {
		if (!(expData[i].IsOps))
			__RefreshVarPointer(&(expData[i].opd), namePnt, nameLen, var->vpnt);
	}
	// refresh Operands Stack
	if (ip->OpdNum > 0) {
		_Operand	*opd = OpdStack_EPnt(ip, 1) ;				// pointer to the 1-st (lowest) element in the stack
		_Operand	*end = OpdStack_EPnt(ip, ip->OpdNum);		// pointer to the top element in the stack
		while (opd <= end) {
			__RefreshVarPointer(opd++, namePnt, nameLen, var->vpnt);
		}
	}
	__RefreshVarPointer(&(ip->LOpd), namePnt, nameLen, var->vpnt);
	__RefreshVarPointer(&(ip->ROpd), namePnt, nameLen, var->vpnt);
}
// ==============================
void	__fastcall	ClearVarPointers(_CE_Instance *ip)
{
	int32			i;
	_ExpresElement	*expData	= ip->expArrDat;			// >>> #CE_3.0.4 2026-06-22
	int32			expSize		= ip->expArrLen;			// >>> #CE_3.0.4 2026-06-22

//DbgPrintf("ClearVarPointers\n");
	// refresh Exprassion Array
	for (i=0; i < expSize; i++) {
		if (!(expData[i].IsOps))
			__ClearVarPointer(&(expData[i].opd));
	}
	// refresh Operands Stack
	if (ip->OpdNum > 0) {
		_Operand	*opd = OpdStack_EPnt(ip, 1) ;				// pointer to the 1-st (lowest) element in the stack
		_Operand	*end = OpdStack_EPnt(ip, ip->OpdNum);		// pointer to the top element in the stack
		while (opd <= end) {
			__ClearVarPointer(opd++);
		}
	}
	__ClearVarPointer(&(ip->LOpd));
	__ClearVarPointer(&(ip->ROpd));
}
// ==============================
__forceinline	Bool32	__fastcall	ClearVarPointersIfChangedVCH(_CE_Instance *ip)
{
	if (ip->VCHandle == ip->oldVCH)
		return FALSE;
	if (ip->oldVCH != 0) {
//DbgPrintf(__FUNCTION__"(): VCH Changed\n");
		ClearVarPointers(ip);
	}
	ip->oldVCH = ip->VCHandle;
	return TRUE;
}
// ==============================
__forceinline	Bool32	__fastcall	ClearVarPointersIfChangedVarMan(_CE_Instance *ip)
{
	if (CheckForHardVarManChange(ip))	{
//DbgPrintf(__FUNCTION__"(): VM Hard Change\n");
		ClearVarPointers(ip);
		return TRUE;
	}
	return FALSE;
}
// <<< #CE_3.0.2 2026-06-19

// ==============================

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// >>> #CE_3.0.2 2026-06-19
// ===  New in CalcExpress 3.0.2  =================
// Определяет указатель на переменную внутри VarMan
// Returns TRUE if success 
// Returns FALSE and makes an error message if error
// ------------
// IMPORTANT!!!
//	the function returns the variable pointer in the '**vip',
//	it uses and updates 'var->vpnt' as the storage of the variable pointers for local variables only!!!
//	for global variables, 'var->vpnt' is NULL to avoid double referencing to local and global variables with the same name.
// ================================================
Bool32 __fastcall	GetPointerToVariable2( _CE_Instance *ip, _VarInfo **vipp, _Operand *var, LVBoolean *err_f, uInt16 *err_c)
{
	LVBoolean	glob = LVTRUE;
	uInt16		errc = 0;

	if (ClearVarPointersIfChangedVarMan(ip))
		var->vpnt = NULL;

	if (var->vpnt) {
		*vipp = var->vpnt;
		return TRUE;
	}
//DbgPrintf(__FUNCTION__"(): find '%H'\n", getIdentString(var));

	if ( *vipp = __vmCmd_VarPointer( ip->VCHandle, &glob, getIdentString(var), &errc)) {
		*err_f	= LVFALSE;
		if (!glob) {
			var->vpnt = *vipp;
			RefreshVarPointers(ip, var);
		}
		return TRUE;	// !!! OK !!!
	}
	else *err_f	= LVTRUE;
	// ERROR: variable is not found. *err_f is already set
	if (err_c) {
		// do not generate CE error, error is handled by the caller
		*err_c = errc;
	}
	else {
		if (errc == VM_ERR_ALIAS_NOTFOUND)
				CE_Error(ip, "Undefined alias \'%H\' ", getIdentString(var), NULL);
		else	CE_Error(ip, "Undefined variable \'%H\' ", getIdentString(var), NULL);
	}
	return FALSE;
}
// <<< #CE_3.0.2 2026-06-19

// >>> #CE_3.0.2 2026-06-19
// ===  New in CalcExpress 3.0.2  =================
// Определяет указатель на переменную внутри VarMan
// Returns TRUE if success 
// Returns FALSE and makes an error message if error
// ------------
// IMPORTANT!!!
//	the function returns the variable pointer in the '**vip',
//	it uses and updates 'var-vpnt' as the storage of the variable pointers for local variables only!!!
//	for global variables, 'var-vpnt' is NULL to avoid double referencing to local and global variables with the same name.
// ================================================
Bool32 __fastcall	GetPointerToIndexedVar2( _CE_Instance *ip, _VarInfo **vipp, _Operand *var, LVBoolean *err_f)
{
	LVBoolean	glob = LVTRUE;
	uInt16		errc = 0;

	if (ClearVarPointersIfChangedVarMan(ip))
		var->vpnt = NULL;

	if (var->vpnt) {
		*vipp = var->vpnt;
		return TRUE;
	}
//DbgPrintf(__FUNCTION__"(): find '%H'\n", getIdentString(var));

	if ( *vipp = __vmCmd_VarPointer( ip->VCHandle, &glob, getIdentString(var), &errc)) {
		*err_f	= LVFALSE;
		if (!glob) {
			var->vpnt = *vipp;
			RefreshVarPointers(ip, var);
		}
//DbgPrintf(__FUNCTION__"(): found, glob=%d\n", glob);
		return TRUE;	// !!! OK !!!
	}
	else {
//DbgPrintf(__FUNCTION__"(): not found\n");
		*err_f	= LVTRUE;
	}
	// ERROR: variable is not found. *err_f is already set
	if (errc == VM_ERR_ALIAS_NOTFOUND)
			CE_Error(ip, "Undefined alias \'%H\' in subscript operator: \'%H[..]\' ", getIdentString(var), getIdentString(var));
	else	CE_Error(ip, "Undefined variable \'%H\' in subscript operator: \'%H[..]\' ", getIdentString(var), getIdentString(var));

	return FALSE;
}
// << #CE_3.0.2 2026-06-19

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


// ===================================================================
//		Функция определения значения переменной
// ===================================================================
// >>> #CE_3.0 2025-04-22 : if 'opd->str' is NULL, string data are not copied
MgErr __fastcall	GetVariable(
			_CE_Instance	*ip,
			_Operand		*var,
			_OperandBase	*value,
			LVBoolean		*err_f	)
{
	_OperandBase	*vip;
	MgErr			err = noErr;

	if (*err_f)	goto out;

	// *****  Проверка  *****
	if (!(var->iip)) {
		err = CE_InternalError(ip, "var->iip == NULL",NULL,NULL);
		*err_f = LVTRUE;
		goto out;
	}

	if (var->iip->qty) {		//	if (IsOperandIndexed(var)) {
		// ----------------------------------------------
		// get the value of an INDEXED variable
		// ----------------------------------------------
		VARMAN_LOCK_SHARED();
		if ( GetPointerToIndexedVar2(ip, &vip, var, err_f)) {		// >>> #CE_3.0.2 2026-06-19
			err = GetIndexedValue(ip, vip, value, var->iip, err_f);
		}
		VARMAN_UNLOCK_SHARED();
	}
	else {
		// ------------------------------------------------
		// get the value of a NOT INDEXED variable
		// ------------------------------------------------
		VARMAN_LOCK_SHARED();
		if (GetPointerToVariable2(ip, &vip, var, err_f, NULL)) {		// >>> #CE_3.0.2 2026-06-19
			err = Var_Copy(value, vip);
		}
		VARMAN_UNLOCK_SHARED();
	}
out:
	return err;
}


// ===================================================================
// 		Функция установки значения переменной
// ===================================================================
MgErr __fastcall	SetVarValue(
			_CE_Instance	*ip,
			_Operand		*var,
			_OperandBase	*value,
			LVBoolean		*err_f,
			Bool32			LockVarMan )
{
	uInt16			errc;
	LVBoolean		glob = LVTRUE;
	enum { k_VAR_CREATE, k_VAR_SETVAL }	vm_cmd;
	MgErr			err = noErr;
	LStrHandle		name;
	_OperandBase	*vip;
	_VarRecord		*vp;

//	if (*err_f)	goto out;

	// --------------------------------
	// error if not identifier
	// --------------------------------
	if (var->type != OPTYPE_IDENT) {
		*err_f = LVTRUE;
		err = CE_Error(ip, "Left operand in assignment must be a variable",NULL,NULL);
		goto out;
	}

	// *****  Проверка  *****
	if (!(var->iip)) {
		err = CE_InternalError(ip, "var->iip == NULL",NULL,NULL);
		*err_f = LVTRUE;
		goto out;
	}

	// ----------------------------------------------
	// set value to INDEXED variable
	// ----------------------------------------------
	if (var->iip->qty) {		//	if (IsOperandIndexed(var)) {
		if (LockVarMan)		VARMAN_LOCK_EXCLUSIVE();
		if (GetPointerToIndexedVar2(ip, &vip, var, err_f)) {		// >>> #CE_3.0.2 2026-06-19
			err = SetIndexedValue(ip, vip, value, var->iip, err_f);
			//vp = __VarInfoPnt_To_VarRecordPnt(var->vpnt);			// BUG!!! var->pnt is NULL for global variables. we must use 'vip'
			vp = __VarInfoPnt_To_VarRecordPnt(vip);					// >>> #CE_3.1.0-r6 2026-09-19	: BUG FIX : we must use 'vip' but not 'vp->vpnt', which is NULL for global variables
			vp->writerID = ip->writerID;							// >>> #CE_3.0.7 2026-07-23
			__UpdateVarWritingTimeNow(vp);							// >>> #CE_3.1.0-r6 2026-09-17
		}
		if (LockVarMan)		VARMAN_UNLOCK_EXCLUSIVE();
		goto out;
	}

	// ------------------------------------------------
	// set value to NOT INDEXED variable
	// ------------------------------------------------
	name	= getIdentString(var);
	vm_cmd	= k_VAR_SETVAL;
vm:
	switch (vm_cmd) {

		case k_VAR_SETVAL:
// >>> #CE_3.0.2 2026-06-13
			if (LockVarMan)		VARMAN_LOCK_EXCLUSIVE();
			if (GetPointerToVariable2(ip, &vip, var, err_f, &errc)) {	// >>> #CE_3.0.2 2026-06-19
				err = __vmCmd_Var_SetValue_Pnt(vip, value, ip->writerID, err_f, &errc);
			}
			if (LockVarMan)		VARMAN_UNLOCK_EXCLUSIVE();
// <<< #CE_3.0.2 2026-06-13
			break;

		case k_VAR_CREATE:
// >>> #CE_3.0.2 2026-06-13
			glob = LVFALSE;		// >>> #CE_3.0.2 2026-06-18
			if (LockVarMan)		VARMAN_LOCK_EXCLUSIVE();
			if (ClearVarPointersIfChangedVarMan(ip))	// >>> #CE_3.0.2 2026-06-19	
				var->vpnt = NULL;
			err = __varCreate_CmdX (ip->VCHandle, &glob, name, (_VarInfo*)value, ip->writerID, NULL, NULL, NULL, &vip, err_f, &errc);
// >>> #CE_3.0.2 2026-06-19
			if (!*err_f) {
				var->vpnt = vip;
				RefreshVarPointers(ip, var);
				//__VarInfoPnt_To_VarRecordPnt(var->vpnt)->writerID = ip->writerID;	// >>> #CE_3.0.7 2026-07-23
			}
// <<< #CE_3.0.2 2026-06-19
			if (LockVarMan)		VARMAN_UNLOCK_EXCLUSIVE();
// <<< #CE_3.0.2 2026-06-13
			break;
	}
	if (err)
		goto out;

	switch(errc) {
		case VM_ERR_NOERROR :
		case VM_ERR_INPUTERROR :
			break;

		// ***	Create a variable if it is enabled
		case VM_ERR_VAR_NOTFOUND :
		case VM_ERR_ALIAS_NOTFOUND:
			if ( (vm_cmd == k_VAR_SETVAL) && ip->procPars.autoVar) {
				// If enabled auto creation of variales
				vm_cmd	= k_VAR_CREATE;
				//glob	= LVFALSE;		// >>> #CE_3.0.2 2026-06-18 : moved up to new position in this file
				*err_f	= LVFALSE;
				errc	= 0;
				goto vm;
			}
			if (errc == VM_ERR_ALIAS_NOTFOUND)
					err = CE_Error(ip, "Undefined alias: \'%H\'", name,NULL);
			else	err = CE_Error(ip, "Undefined variable: \'%H\'", name,NULL);
			break;


		case VM_ERR_CTX_INVHANDLE :	// *** Invalid VC handle
			err = CE_Error(ip, "Undefined variable: \'%H\' (invalid VC Handle)", name,NULL);
			break;
		case VM_ERR_VAR_INVTYPE :	// *** Invalid type
			err = CE_Error(ip, "Datatype mismatch in assignment: %H", name,NULL);
			break;
		case VM_ERR_VAR_INVNAME :	// *** Invalid variable name
			err = CE_Error(ip, "Invalid variable name: \'%H\'", name,NULL);
			break;
		case VM_ERR_VAR_NONUNIQUE :	// *** Non-unique variable name
			err = CE_Error(ip, "Non-unique variable name: \'%H\'", name,NULL);
			break;

		case VM_ERR_ALIAS_INVNAME :	// *** Invalid alias name
			err = CE_Error(ip, "Invalid alias name: \'%H\'", name,NULL);
			break;
		case VM_ERR_ALIAS_NONUNIQUE :	// *** Non-unique alias name
			err = CE_Error(ip, "Non-unique alias name: \'%H\'", name,NULL);
			break;

		default :
			err = CE_Error(ip, "VarMan returned error %d on variable name specifier: \'%H\'", (LStrHandle)errc, name);
			break;
	}
out:
	return err;
}

