/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <extcode.h>

#include <celib\ceMemLib.h>
#include <lvsup\lvsup.h>

#include <VarMan.h>
#include <VHistory.h>
#include <VarInfo_Routines.h>
#include <VMEngine_Routines.h>

#include <VMEngine.h>


// ------------- GLOBAL DATA ----------------------
static	_AliasHashTable		aht[ALIAS_HASH_TABLE_SIZE];	// Global Hash Table for Aliases
static	_CTableHandle		ctbl		= NULL;			// Handle to the Table of Variable Contexts
static	_CTable				*ctblPnt	= NULL;			// Pointer to the Table of Variable Contexts
static	int32				curHandle	= 0x00000000;
		uInt32				_DelVarCnt	= 0;			// used to inform CE that variable(s) deleted and old variable pointers can be invalid [#CE_3.0.2 2026-06-19]
//		CRITICAL_SECTION	ctblLock;					// DEPRECATED: 'Critical Section' lock for VarMan operations
		SRWLOCK				VM_SRWLock;					// 'Slim Reader/Writer Lock' for cooperative access to VarMan operations [#CE_3.0.2 2026-06-08]
// ------------------------------------------------

#include "VM_Subr.c"

#include "history\History.c"
#include "history\History-2.c"
#include "HashTbl_Aliases.c"
#include "HashTbl_Vars.c"
#include "ContextTable.c"
#include "GetVarPointer.c"

// ============================

#include "cmd_Alias_InfoList.c"
#include "cmd_Alias_Opers.c"
// ----
#include "cmd_Ctx_Control.c"
#include "cmd_Ctx_InfoList.c"
#include "cmd_Ctx_Names.c"
// ----
#include "cmd_Misc_Fields.c"
// ----
#include "cmd_Var_Create.c"
#include "cmd_Var_Destroy.c"
#include "cmd_Var_GetValue.c"
#include "cmd_Var_SetValue.c"
#include "cmd_Var_Copy.c"
#include "cmd_Var_InfoList.c"
#include "cmd_Var_Attribute.c"
// -----
#include "history\cmd_History.c"
#include "history\cmd_History-2.c"
// ----
#include "VM_Metrics.c"


// ***********************  Main Function  ****************************

MgErr VMEngine(
			uInt16			cmd,
			int32			*VCH,
			LVBoolean		*glob,
			LStrHandle		name,
			_VarInfo		*par,
			uInt32			writerID,	// >>> #CE_3.0.7 2026-07-23
			_VM_EXT_FUNC	*extfunc,	//extension function
			void			*extdata,	//extension data
			LVBoolean		*err_f,
			uInt16			*err_c)
{
	// ***  ¬ыполнение команды  ***
	switch (cmd) {

// >>> #CE_3.0.2 2026-06-08 : deprecated
//		case VM_CMD_LOCK :
//			EnterCriticalSection( &ctblLock );	return noErr;
//		case VM_CMD_UNLOCK :
//			LeaveCriticalSection( &ctblLock );	return noErr;
//		case VM_CMD_VAR_PNT :
//			*((_VarInfo**)extdata) = __vmCmd_VarPointer(handle, glob, name, err_f, err_c);
//			return noErr;
// <<< #CE_3.0.2 2026-06-08

		//----
//		case VM_CMD_INIT :
//			return vmCmd_VarMan_Init(err_f, err_c);
//		case VM_CMD_CTX_OPEN :
//			return vmCmd_Context_Open(VCH, NULL /*!!!*/, 0 /*!!!*/, err_f, err_c);
//		case VM_CMD_CTX_CLOSE :
//			return vmCmd_Context_Close(VCH, err_f, err_c);
//		case VM_CMD_CTX_TEST :
//			return vmCmd_Context_Test(VCH, err_f, err_c);
//		case VM_CMD_CTX_INFOLIST :
//			return	vmCmd_Context_InfoList(name, par, err_f, err_c);

		//----
//		case VM_CMD_VAR_CREATE :
//			return	vmCmd_Var_CreateEx(*VCH, glob, name, par, writerID, NULL, extfunc, extdata, err_f, err_c);
//		case VM_CMD_VAR_DESTROY :
//			return	vmCmd_Var_Destroy(*VCH, glob, name, err_f, err_c);
//		case VM_CMD_VAR_GETVAL :
//			return	vmCmd_Var_GetValueEx(*VCH, glob, name, par, extfunc, extdata, err_f, err_c);
//		case VM_CMD_VAR_SETVAL :
//			return	vmCmd_Var_SetValueEx(*VCH, glob, name, par, writerID, extfunc, extdata, err_f, err_c);
//		case VM_CMD_VAR_COPY :
//			return	vmCmd_Var_Copy((_VM_2Var_Info*)extdata, writerID, err_f, err_c);
//		case VM_CMD_VAR_INFOLIST :
//			return	vmCmd_Var_InfoList(*VCH, glob, name, par, err_f, err_c);

		//----
//		case VM_CMD_ALIAS_INFOLIST :
//			return	vmCmd_Alias_InfoList(par, err_f, err_c);
//		case VM_CMD_ALIAS_OPERS :
//			return	vmCmd_Alias_Operation(VCH, glob, name, par, err_f, err_c);

		//----
//		case VM_CMD_VCNAME_RESET :
//			return	vmCmd_vcNames_Reset(err_f, err_c);
//		case VM_CMD_VCNAME_ASSIGN :
//			return	vmCmd_vcNames_Assign(VCH, name, err_f, err_c);
//		case VM_CMD_VCNAME_GETNAME :
//			return	vmCmd_vcNames_GetName(VCH, name, err_f, err_c);
//		case VM_CMD_VCNAME_FINDNAME :
//			return	vmCmd_vcNames_FindName(VCH, name, err_f, err_c);

		//----
//		case VM_CMD_HISTORY_CREATE :
//			return	vmCmd_History_Create(*VCH, name, (_VHistParams*)extdata, err_f, err_c);
//		case VM_CMD_HISTORY_DESTROY :
//			return	vmCmd_History_Destroy(*VCH, name, err_f, err_c);
//		case VM_CMD_HISTORY_SAVE :
//			return	vmCmd_History_Save(*VCH, name, err_f, err_c);
//		case VM_CMD_HISTORY_ACCESS :
//			return	vmCmd_History_Access(*VCH, name, (VHistAcc_func)extfunc, extdata, err_f, err_c);
//		case VM_CMD_HISTORY_RESET :
//			return	vmCmd_History_Reset(*VCH, name, err_f, err_c);
//		case VM_CMD_HISTORY_CONFIG :
//			return	vmCmd_History_Config(*VCH, name, (_VHistParams*)extdata, err_f, err_c);

		//----
//		case VM_CMD_MISCACC :
//			return	vmCmd_MiscFieldAccess(VCH, (VM_MiscAccessPars*)extdata, err_f, err_c);

		//----
//		case VM_CMD_NOP :
//					if (*err_f)	*err_c = VM_ERR_INPUTERROR;
//					else		*err_c = VM_ERR_NOERROR;
//					break;
		//----
//		case VM_CMD_LOAD :
//		case VM_CMD_UNLOAD :
//					*err_c	= VM_ERR_NOERROR;
//					break;
		default :
					if (*err_f)	*err_c = VM_ERR_INPUTERROR;
					else		*err_c = VM_ERR_INVALID_CMD;
					break;
	}
	return noErr;
}

// *********************** Initializing  ****************************

MgErr	varman_Initializing(void)
{
	MgErr	err = noErr;

	VARMAN_LOCK_CREATE();
	VARMAN_LOCK_EXCLUSIVE();

	curHandle = 0x00000000;

	AliasHashTable_Clear();		// первым
	err = ContextTable_Init();	// вторым

	VARMAN_UNLOCK_EXCLUSIVE();

	return err;
}

// *********************** Finalizing  ****************************

MgErr	varman_Finalizing(void)
{
	MgErr	err = noErr;

	VARMAN_LOCK_EXCLUSIVE();

	if (ctbl) {
		err = ContextTable_Destroy();
		err = AliasHashTable_DestroyAll();	// на вс€кий случай
		ctbl = NULL;
		curHandle = 0x00000000;
	}

	VARMAN_UNLOCK_EXCLUSIVE();
	VARMAN_LOCK_DELETE();

	return err;
}

