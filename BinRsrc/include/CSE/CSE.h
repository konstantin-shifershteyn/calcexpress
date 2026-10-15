/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CSE_H__
#define	__CSE_H__

#ifdef	__CESP_H__
		#error !!! Already defined: __CESP_H__ !!!
#endif
#define	__CESP_H__

#ifdef	__CESP_STAT_PARSER_H__
		#error !!! Already defined: __CESP_STAT_PARSER_H__ !!!
#endif
#define	__CESP_STAT_PARSER_H__

#ifdef	__CE_ScrParser_H__
		#error !!! Already defined: __CE_ScrParser_H__ !!!
#endif
#define	__CE_ScrParser_H__

#ifdef	__iCESP_InstanceAccess_H__
		#error !!! Already defined: __iCESP_InstanceAccess_H__ !!!
#endif
#define	__iCESP_InstanceAccess_H__


// =================================

#include <extcode.h>

#include <CalcExpress.h>
#include <CE_ErrorCodes.h>

#include <CSE Modifiers.h>

// ---------------------------
// CSE Execution Status Codes
// ---------------------------
enum {
	estNormalTermination	= -1,
	estNotCompleted			= 0,
	estSuccess				= 0,
	estInvalidRefNum		= CSE_ERR_INVINSTANCE	,
	estSyntaxError			= CSE_ERR_SYNTAX		,
	estRuntimeError			= CSE_ERR_RUNTIME		,
	estCancelled			= CSE_ERR_ABORT
};


// ==========================================================================
//		Definitions of data describing parsed statements
// ==========================================================================

// -------------------------------------------
// codes of jumpAction field in iPScrElement
// -----------------------------------------
#define	JA_JUMP				0	// unconditional jump
#define	JA_JUMP_IFT			1	// jump if expression is true
#define	JA_JUMP_IFF			2	// jump if expression is false
#define	JA_STOP				3	// terminate execution

// -------------------------------------------
// codes of stmType field in iPScrElement
// -----------------------------------------
#define	STMTYPE_EXPRESS		0	// expression
#define	STMTYPE_JUMP		1	// jump
#define	STMTYPE_SRCREF		2	// source reference
#define	STMTYPE_JUMPSTUB	3	// jump stub is used for arrange implicit jumps in 'if/else', 'while', 'for'
#define	STMTYPE_SRCREFSTUB	4	// a stub to arrange a source reference

// -------------------------------------
// Descriptor of parsed statement
// -------------------------------------
// >>> #CE_3.0 2025-05-01 Comments
// jumpIndex in parsed script contains an absolute index of the target statement
// jumpIndex while PARSING:
//		if jumpIndex < 0, it designates an relative offset to a target_statement_index to jump as follow:
//			if jumpIndex > LABEL_DIRECTION_THRESHOLD (=-10000), abs(jumpIndex) is offset to jump forward; i.e. target index to jump equals statement_index + abs(jumpIndex)
//			otherwise, abs(jumpIndex - LABEL_DIRECTION_THRESHOLD) is offset to jump backward; i.e. target index to jump equals statement_index - abs(pe->jumpIndex - LABEL_DIRECTION_THRESHOLD)
//		if jumpIndex >= 0, jumpIndex contains Label_Index; i.e. index in label table (_ScrParserInst._LabelInfoArrayHdl).
// <<< #CE_3.0 2025-05-01 Comments
typedef	struct {
	_CE_Instance	*ip;			// pointer to CE Instance (NULL if expression is non-calculated)
	int32			jumpAction;
	int32			jumpIndex;
	int32			stmType;		// 0:expression, 1: jump; 2:srcReference, 3:jump-stub
	int32			NestingLevel;	// !!!
//	int32			calcIt;			// ???
//	int32			idx_flag;		// ???
	// *** Links to statement in source script
	int32			stmOffset;		// offset of a statement involving the expression
	int32			stmLength;		// length of the statement (in bytes)
	int32			stmLine;		// line number where the statement started
	// *** Links to expression in source script
	int32			expOffset;		// offset of expression in a script (an index)
	int32			expLength;		// length of the expression (in bytes)
	int32			expLine;		// line number where the expression started
	// ---
	const char		*expNote;		// points to static memory !!!
	int32			parentIndex;	// it's used in error messaging and debugging
	int32			__pad_1__;		
} iPScrElement;
STATIC_ASSERT_PNT_BYTES_OFFSET(iPScrElement,ip);
STATIC_ASSERT_PNT_BYTES_OFFSET(iPScrElement,expNote);
STATIC_ASSERT_8_BYTES_SIZE(iPScrElement);


// -------------------------------------
// Array of parsed statements
// -------------------------------------
typedef struct {
	int32			size;
#if defined(_M_X64)
	int32			__pad_1__;	// padding bytes
#endif
	iPScrElement	buff[1];
} iPScrArray, **iPScrArrayHdl;
STATIC_ASSERT_PNT_BYTES_OFFSET(iPScrArray,buff);

// ---------------------------------------------------
// Definition to operate on Array of parsed statements
// ---------------------------------------------------
#define	iPScrSize(ar)		(((ar))->size)
#define iPScrBuff(ar)		(&((ar))->buff[0])
#define iPScrEPnt(ar,idx)	(&((ar))->buff[(idx)])


// ==========================================================================
//		Definition of data describing parsed script
// ==========================================================================

#define	CSE_CONTEXT_SIGNATURE	'iCSE'		// signature of CSE Context
#define	CSE_RESSTOR_OWNER		'csei'		// ResourceStore: signature of Owner of CSE Resource


// Instance Context of a parsed CSE script
// ---------------------------------------

typedef	struct	_iPScrInst	*_iPScrInstPnt;
typedef	struct  _iPScrInst	{
	uInt32			signature;			// context signature
	int32			SyncTag;			// synchronization tag (like a semaphore)
	Bool32			CancelTag;			// tag of abort condition
	uInt32			writerID;			// >>> #CE_3.0.7 2026-07-25 : VarMan variable's writer ID
	// --
	LStrHandle		Script;				// source script
	LStrHandle		ScriptName;			// script name (file name)
	iPScrArrayHdl	iPScr;				// parsed script (array of parsed statements)
	LStrHandle		ErrMsg;				// !!!
	// --
	Bool32			fScriptParsed;		// TRUE if script is successfully parsed
	Bool32			fGroupExpStm;		// Grouping of expression statements during parsing
	int32			ErrCode;			// !!!
	int32			ErrIndex;			// index of an element in iPScrArray
	int32			ErrSrcMaxLen;		// maximum length of error source
	// --
	int32			DbgNestingLevel;	// used during execution in debug mode
	int32			DbgStmLine;			// used during execution in debug mode

	// -- The following are used for calling scripts (CSE FuncVI2)
	int32			rtlCallLevel;
	_iPScrInstPnt	rtlCaller;
	_iPScrInstPnt	rtlCallee;
	int32			rtlRunMode;
	LStrHandle		rtlCallbackVIPath;
	//
	LStrHandle		rtlScriptPath;
	LStrHandle		rtlSearchPaths;

} iPScrInstance, *iPScrInstancePnt;
STATIC_ASSERT_PNT_BYTES_OFFSET(iPScrInstance,Script);


// =====================================================================
// 	INTERNAL FUNCTIONS
// =====================================================================

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------
// ***	file:	CSE_Instance.c
// ----------------------------------------------

extern	MgErr	__cdecl	iPScr_MinElement_RTData ( iPScrElement  *el);
extern	MgErr	__cdecl	iPScr_MinElements_RTData( iPScrArrayHdl ipsa);

extern	MgErr	__cdecl	iPScr_FreeElement ( iPScrElement *el);
extern	MgErr	__cdecl	iPScr_FreeElements( iPScrArrayHdl ipsa, int32 idx);
extern	MgErr	__cdecl	iPScr_ResizeArray ( iPScrArrayHdl *ipsa, int32 newSize);

extern	MgErr	__cdecl	iPScr_CreateArray ( iPScrInstance *iCESP);
extern	MgErr	__cdecl	iPScr_DestroyArray( iPScrInstance *iCESP);

extern	MgErr	__cdecl	CSE_InstCreate ( iPScrInstancePnt *iCESP_Hdl);
extern	MgErr	__cdecl	CSE_InstDestroy( iPScrInstancePnt iCESP);
extern	MgErr	__cdecl	CSE_InstFreeMem( iPScrInstancePnt iCESP);
extern	int32	__cdecl	CSE_DestroyAllInstances(void);


// ----------------------------------------------
// ***	file:	CSE_ErrorMessaging.c
// ----------------------------------------------
extern	MgErr	__cdecl	CSE_MakeErrorMessage(
				iPScrInstance	*iCESP,		// context
				int32		stmIndex,	// statement index
				int32		ErrType,	// 0,estSyntaxError,estRuntimeError,estCancelled
				Bool32		fSourceInfo,	// include source info
				CStr		cfmt,		// format string
				void		*par1,
				void		*par2 );


// ----------------------------------------------
// ***	file:	CSE_Diagnostic.c
// ----------------------------------------------
extern	int32	__cdecl	CSE_Diag_Convert_ItemID(int32	extItemID);
extern	MgErr	__cdecl	CSE_List_Of_Item_Names(	_CE_Instance *ip, LStrHandle outString, int32 ItemID );
extern	MgErr	__cdecl	CSE_Array_Of_Lists_Of_Item_Names( iPScrInstance	*iCESP, LStr1DArrayHdl outArray, int32 ItemID );


#ifdef __cplusplus
}
#endif

#endif
