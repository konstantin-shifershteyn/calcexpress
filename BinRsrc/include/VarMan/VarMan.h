/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__VarMan_H__
#define	__VarMan_H__

#include <extcode.h>
#include <CE_DataTypes.h>

//#include <VarMan20_Errors.h>
#include <VarMan_Errors.h>

// ##################################################################

// >>> #CE_3.0.2 2026-06-08
#define	WINDOWS_IGNORE_PACKING_MISMATCH
#include <windows.h>
//--
extern	SRWLOCK		VM_SRWLock;			// 'Slim Reader/Writer Lock' for cooperative access to VarMan operations
//-- Macros to operate on SRW Lock
#define	VARMAN_LOCK_CREATE()		InitializeSRWLock(&VM_SRWLock)
#define	VARMAN_LOCK_DELETE()		// no function
#define	VARMAN_LOCK_SHARED()		AcquireSRWLockShared(&VM_SRWLock)
#define	VARMAN_LOCK_EXCLUSIVE()		AcquireSRWLockExclusive(&VM_SRWLock)
#define	VARMAN_UNLOCK_SHARED()		ReleaseSRWLockShared(&VM_SRWLock)
#define	VARMAN_UNLOCK_EXCLUSIVE()	ReleaseSRWLockExclusive(&VM_SRWLock)
/*
extern	CRITICAL_SECTION	ctblLock;	//  'CriticalSection' for cooperative access to VarMan operations
//-- Macros to operate on CriticalSection
#define	VARMAN_LOCK_CREATE()		InitializeCriticalSection(&ctblLock)
#define	VARMAN_LOCK_DELETE()		DeleteCriticalSection(&ctblLock)
#define	VARMAN_LOCK_SHARED()		EnterCriticalSection(&ctblLock)
#define	VARMAN_LOCK_EXCLUSIVE()		EnterCriticalSection(&ctblLock)
#define	VARMAN_UNLOCK_SHARED()		LeaveCriticalSection(&ctblLock)
#define	VARMAN_UNLOCK_EXCLUSIVE()	LeaveCriticalSection(&ctblLock)
*/
// <<< #CE_3.0.2 2026-06-08

// ##################################################################

// ***	символ - префикс алиаса
#define	VM_ALIAS_SIGN	'='

// =============================================
//		VarMan Command Codes
// =============================================

//#define VM_CMD_NOP				0
//#define	VM_CMD_INIT				1
//#define	VM_CMD_CTX_OPEN			2
//#define	VM_CMD_CTX_CLOSE		3
//#define VM_CMD_CTX_TEST			4
//#define	VM_CMD_VAR_CREATE		5
//#define	VM_CMD_VAR_DESTROY		6
//#define	VM_CMD_VAR_SETVAL		7
//#define	VM_CMD_VAR_GETVAL		8
//#define	VM_CMD_LOAD				9	// OBSOLETE (IS NOT USED IN VM2.4 DLL version)
//#define	VM_CMD_UNLOAD			10	// OBSOLETE (IS NOT USED IN VM2.4 DLL version)
//#define	VM_CMD_VAR_INFOLIST		11	// VM_CMD_VARLIST before VarMan 2.2
//#define	VM_CMD_CTX_INFOLIST		12
//#define	VM_CMD_ALIAS_INFOLIST	13	// new in VarMan 2.2
//#define	VM_CMD_ALIAS_OPERS		14	// new in VarMan 2.2

// -----------
// команда VM_CMD_VARTEST выполняется при cmd = VM_CMD_VAR_GETVAL и param->str = NULL
// #define	VM_CMD_VARTEST	NNN
// -----------

// -----------
// >>> #CE_3.0.2 2026-06-08	: deprecated
//#define	VM_CMD_LOCK				20
//#define	VM_CMD_UNLOCK			21
//#define	VM_CMD_VAR_PNT			22
// ------------
//#define	VM_CMD_MISCACC			23
//---
//#define	VM_CMD_VAR_COPY			24

// ------------------------------------------
// команды манипулирования именами контекстов
// ------------------------------------------
//#define	VM_CMD_VCNAME_RESET		0x0101		// reset all names
//#define	VM_CMD_VCNAME_ASSIGN	0x0102		// assign name to VC
//#define	VM_CMD_VCNAME_GETNAME	0x0103		// get VC name
//#define	VM_CMD_VCNAME_FINDNAME	0x0104		// get VC handle

// ------------------------------------------
// Variable History related commands
// ------------------------------------------
//#define	VM_CMD_HISTORY_CREATE	0x0201
//#define	VM_CMD_HISTORY_DESTROY	0x0202
//#define	VM_CMD_HISTORY_SAVE		0x0203		// it forces value be saved to history (is it used?)
//#define	VM_CMD_HISTORY_ACCESS	0x0204
//#define	VM_CMD_HISTORY_RESET	0x0205
//#define	VM_CMD_HISTORY_CONFIG	0x0206


// Subcommands of VM_CMD_ALIASOPERS
// --------------------------------
#define	VM_SUBCMD_ALIAS_New			1
#define	VM_SUBCMD_ALIAS_NewOrRename	2
#define	VM_SUBCMD_ALIAS_Dispose		3


// -------------------------------------
// Тип описывает информацию о переменной
// Varaible datatype and value
// -------------------------------------
typedef struct {
	uInt32		type;
#if defined(_M_X64)
	int32		pad_1;		// >>> #CE_3.0 2024-12-11
#endif
	LStrHandle	str;
	float64		num;
} _VarInfo;
STATIC_ASSERT_8_BYTES_OFFSET(_VarInfo,num);
STATIC_ASSERT_8_BYTES_SIZE(_VarInfo);


#define	VM_TYPE_STR			OPTYPE_STR
#define	VM_TYPE_DBL			OPTYPE_DBL
#define	VM_TYPE_STRING		OPTYPE_STR
#define	VM_TYPE_NUMBER		OPTYPE_DBL

#define	VM_TYPE_A1DBL		OPTYPE_A1DBL
#define	VM_TYPE_A2DBL		OPTYPE_A2DBL
#define	VM_TYPE_CDB			OPTYPE_CDB
#define	VM_TYPE_A1CDB		OPTYPE_A1CDB
#define	VM_TYPE_A2CDB		OPTYPE_A2CDB
#define	VM_TYPE_A1STR		OPTYPE_A1STR
#define	VM_TYPE_A2STR		OPTYPE_A2STR

#define	VM_TYPE_IDXRANGE	OPTYPE_IDXRANGE

// ==================================================
//	VarMan Extension Function Prototype
// ==================================================
typedef MgErr _VM_EXT_FUNC(
	LStrHandle	name,		// name of variable
	_VarInfo	*vp_int,	//_VarInfo inside VarMan
	_VarInfo	*vp_ext,	//_VarInfo outside VarMan
	void		*extdata,	// external data
	LVBoolean	*err_f,
	uInt16		*err_c);

// ### VarMan Extension functions to pass data to/from VIs
MgErr vmExtFunc_getval( LStrHandle name, _VarInfo *vp_int, _VarInfo *vp_ext, void *extdata, LVBoolean *err_f, uInt16 *err_c);
MgErr vmExtFunc_setval( LStrHandle name, _VarInfo *vp_int, _VarInfo *vp_ext, void *extdata, LVBoolean *err_f, uInt16 *err_c);

// ### Структура, через которую передаются параметры в функции расширения VarMan: vmExtFunc_getval() & vmExtFunc_setval()
typedef struct {
	uInt32		type;
#if defined(_M_X64)
	int32		pad_1;	// >>> #CE_3.0 2024-12-13 : padding bytes
#endif
	UHandle		val;
} vmcin_extdata;

// ==================================================
//		 VarMan Engine 2.0 Prototype
// ==================================================

#ifdef	__cplusplus
extern "C" {
#endif

extern	MgErr	__cdecl	VMEngine(
			uInt16			cmd,
			int32			*handle,
			LVBoolean		*glob,
			LStrHandle		name,
			_VarInfo		*par,
			uInt32			writerID,	// >>> #CE_3.0.7 2026-07-23
			_VM_EXT_FUNC	*extfunc,	//extension function
			void			*extdata,	//extension data
			LVBoolean		*err_f,
			uInt16			*err_c);

extern	MgErr	__cdecl	varman_Initializing(void);
extern	MgErr	__cdecl	varman_Finalizing(void);

#ifdef	__cplusplus
}
#endif


// ===============================================================
//	To access miscellaneous fields in a variable context
// ===============================================================

#define	VM_CMD_MISCACC_GET	0
#define	VM_CMD_MISCACC_SET	1

// Pointer to such structure must be passed to VMEngine through 'extdata' argument
typedef struct {
	int32	cmd;		// action: 0-get, 1-set
	int32	idx;		// index of miscellaneous field
	int32	val;		// value
	int32	old;		// old value (if cmd == 'set')
} VM_MiscAccessPars;


// ====================================
//	To copy variables
// ====================================
// Pointer to such structure must be passed to VMEngine through 'extdata' argument
typedef struct	{
	int32		src_vch;
	LStrHandle	src_name;
	int32		dst_vch;
	LStrHandle	dst_name;
	Bool32		enaCreate;
} _VM_2Var_Info;

// ====================================
//	VI interface data types
// ====================================
// >>> #CE_3.1.0-r6 2026-09-18
// Structure to return variable write statistics to VarMan VIs
typedef struct {
	uInt32		writerID;
	uInt32		writeMS;
	float64		writeTS;
	uInt32		writeCnt;
} _VM_VI_WriteStats;
// <<< #CE_3.1.0-r6 2026-09-18

// >>> #CE_3.1.0-r6 2026-09-19
// Structure to return variable creation statistics to VarMan VIs
typedef struct {
	uInt32		creatorID;
	uInt32		createMS;
	float64		createTS;
} _VM_VI_CreateStats;
// <<< #CE_3.1.0-r6 2026-09-18



#endif
