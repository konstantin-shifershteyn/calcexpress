/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CSE_Parser_H__
#define	__CSE_Parser_H__

#include <CSE.h>


// ============================================================================
//	VI communication parameters
// ============================================================================

// ***	Code Generation parameters
typedef struct {
	LVBoolean	fAutoParsing;		// auto determining of necessity to parse
	LVBoolean	fGroupExpStm;		// group expression statements
#if defined(_M_X64)
	uInt8		pad_2[2];			// >>> #CE_3.0 2024-12-12 : pading bytes
#endif
	int32		ErrSrcMaxLen;		// maximum length of error source
	//--
	LVBoolean	fEnaNamedConst;		// enable Named Constants [#CE_3.1.0-r6 2026-09-28]
#if defined(_M_X64)
	uInt8		pad_3[3];			// >>> #CE_3.0 2024-12-12 : pading bytes
#endif
} _VIParseParams;


// ============================================================================
//	Definition of data used to treat Labels
// ============================================================================

#define	LBL_BLKSIZE	64

// >>> #CE_3.0 2024-12-12 : members rearranged
typedef	struct {
	int32	expIdx;		// index of parsed statement
	int32	nLine;		// line number
	char	*name;		// label name, can be NULL for unnamed label (e.g. if/for/while)
} _LabelInfo;
STATIC_ASSERT_PNT_BYTES_OFFSET(_LabelInfo,name);
STATIC_ASSERT_PNT_BYTES_SIZE(_LabelInfo);


typedef struct {
	int32		dimSize;
#if defined(_M_X64)
	int32		__pad_1__;		// >>> #CE_3.0 2024-12-12 : padding bytes
#endif
	_LabelInfo	data[1];
} _LabelInfoArray, **_LabelInfoArrayHdl;
STATIC_ASSERT_PNT_BYTES_OFFSET(_LabelInfoArray,data);


// ============================================================================
//	Definition of data used to treat Statement Keywords
// ============================================================================

#define	STM_BLKSIZE	64

typedef struct {
	int32	code;
	int32	nLine;
	int32	PScrIdx;		// index in iPScr array
	int32	label1;
	int32	label2;
	int32	__pad_1__;		// >>> #CE_3.0.4 2026-07-09
} _OprInfo;
STATIC_ASSERT_8_BYTES_SIZE(_OprInfo);


typedef struct {
	int32		dimSize;
#if defined(_M_X64)
	int32		__pad_1__;	// >>> #CE_3.0 2024-12-12
#endif
	_OprInfo	data[1];
} _OprInfoArray, **_OprInfoArrayHdl;
STATIC_ASSERT_PNT_BYTES_OFFSET(_OprInfoArray,data);


// ============================================================================
// 	Structure contains temporary data used by CSE Parser
// ============================================================================

#define	PSCR_BLKSIZE	128

typedef struct {
	uChar				*scr;		// pointer to script
	int32				scrLen;		// script length (bytes)
	int32				curLine;	// current line number
	int32				expLine;	// line number where expression statement begins
	int32				expOffset;	// offset where expression statement begins
	int32				expStat;	// состояние текущего подвыражения
	int32				NestingLevel;	// уровень вложенности структур (if,for,while)

	// ---- используются для обработки меток ----
	int32				identOff;	// смещение последнего идентификатора
	int32				identLen;	// длина последнего идентификатора
	Bool32				MayBeLabel;	// признак: идентификатор может быть меткой
	int32				identStat;	// состояние текущего идентификатора

	// ----
	iPScrInstance		*iCSE;		// pointer to instance context
	iPScrArrayHdl		iPScr;		// handle оттранслированного скрипта
	int32				PScrIdx;	// текущий индекс
	// ----
	_OprInfoArrayHdl	OprStk;		// handle стека операторов
	int32				OprNum;		// индекс первого в стеке
	// ----
	_LabelInfoArrayHdl	LblTbl;		// handle таблицы меток
	int32				LblNum;		// кол-во меток в таблице
	// ----
	LStrHandle			ErrMsg;		// сообщение об ошибках

	// ---- code generation ----
	Bool32				fCanBeGrouped;	// current state of permission to group statements
	Bool32				fEnaNamedConst;	// current state of permission for Named Constants	[#CE_3.1.0-r6 2026-09-28]
} _ScrParserInst;
STATIC_ASSERT_PNT_BYTES_OFFSET(_ScrParserInst,iCSE);
STATIC_ASSERT_PNT_BYTES_OFFSET(_ScrParserInst,iPScr);
STATIC_ASSERT_PNT_BYTES_OFFSET(_ScrParserInst,OprStk);
STATIC_ASSERT_PNT_BYTES_OFFSET(_ScrParserInst,LblTbl);
STATIC_ASSERT_PNT_BYTES_OFFSET(_ScrParserInst,ErrMsg);

// ------------------------
//	Operator codes
// ------------------------
#define	OP_UNKNOWN				-111
#define	OP_EndOfScript			-2
#define	OP_COMMENT				-1
// ----
#define	OP_GENERAL				0
#define	OP_SEMICOLON			1
#define	OP_BRACKET_L			2
#define	OP_BRACKET_R			3
#define	OP_SQUARE_BRACKET_L		4
#define	OP_SQUARE_BRACKET_R		5
#define	OP_CURLY_BRACKET_L		6
#define	OP_CURLY_BRACKET_R		7
// ----
#define	OP_IF					8
#define	OP_ELSE					9
#define	OP_WHILE				10
#define	OP_FOR					11
#define	OP_BREAK				12
#define	OP_CONTINUE				13
#define	OP_RETURN				14
#define	OP_GOTO					15
#define	OP_COLON				16	// : can be a sign of a label
#define	OP_DOWHILE				17	// !!! new !!!


// ------------------------------------
// Subexpression processing State Codes
// ------------------------------------
#define	SUBEXP_LABEL		-1		// метка перед подвыражением
#define	SUBEXP_NONE			0		// нет начатого подвыражения
#define	SUBEXP_ACTIVE		1		// имеется активное (незаконченное) подвыражение


// ===========================================================
// 	FUNCTION PROTOTYPES
// ===========================================================

#ifdef	__cplusplus
extern "C" {
#endif

extern	MgErr __cdecl	CSE_ParseScript(
							iPScrInstance	*iCSE,
							LStrHandle		Script,
							LStrHandle		ScriptName,
							_VIParseParams	*params,
							LVBoolean		*fDidParsing,
							int32			*ExecStatus);


#ifdef	__cplusplus
}
#endif

#endif
