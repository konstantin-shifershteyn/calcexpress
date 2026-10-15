/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef __VM_ENGINE_H__
#define __VM_ENGINE_H__

#include <extcode.h>

#include <VarMan.h>
#include <VHistory.h>

#include <VMEngine_Constants.h>


// declared in VarMan.h :
//MgErr	__cdecl	varman_Initializing(void);
//MgErr	__cdecl	varman_Finalizing(void);


// non complete types (type patterns)
// ----------------------------------
typedef	struct	_var_rec	*___pVarRec;	// pointer to variable record
typedef	struct	_alias_rec	*___pAliasRec;	// pointer to alias record

// ---------------------------------------
// Тип описывает элемент списка алиасов
// Alias Record (an element of Alias List)
// ---------------------------------------
typedef	struct	_alias_rec {
	___pAliasRec	lnkHashList;	// *pointer* to the next alias in a hash list
	LStrHandle		name;			// alias name
	___pVarRec		var;			// *pointer* to variable record
	int32			vcIndex;		// index in VC Table
} _AliasRecord, *_AliasHashTable;
STATIC_ASSERT_PNT_BYTES_SIZE(_AliasRecord);
// -----------------------------

// ---------------------------------------
// Тип описывает элемент списка переменных
// Variable Resord (an element of Variable List)
// ---------------------------------------
typedef struct _var_rec {
	___pVarRec		link;		// *pointer* to the next variable in a list
	LStrHandle		name;		// variable name
	char			*namePnt;	// Used to speed up searching var names.	[#CE_3.1.0 2026-08-18]
	int32			nameLen;	// Used to speed up searching var names.	[#CE_3.1.0 2026-08-18]
	_VarInfo		inf;		// variable info
	_pVHistory		hist;		// *pointer* to variable history descriptor
	_AliasRecord	*alias;		// *pointer* to alias record
	LStrHandle		attrib;		// an attribute (can be NULL)		[CE261 18-10-2008]
	uInt32			writerID;	// an writer ID						[#CE_3.0.7 2026-07-23]
//	uInt32			__pad_1__;
	uInt32			writeCnt;	// incremental writing counter		[#CE_3.1.0-r6 2026-09-17]
	uInt32			writeMS;	// millisecs of the last writing	[#CE_3.1.0-r6 2026-09-17]
	float64			writeTS;	// timestamp of the last writing	[#CE_3.1.0-r6 2026-09-17]
	//--
	uInt32			creatorID;	// a creator ID						[#CE_3.1.0-r6 2026-09-19]
	uInt32			createMS;	// millisecs of the creation		[#CE_3.1.0-r6 2026-09-19]
	float64			createTS;	// timestamp of the creation		[#CE_3.1.0-r6 2026-09-19]
} _VarRecord;
STATIC_ASSERT_PNT_BYTES_OFFSET(_VarRecord,hist);
STATIC_ASSERT_PNT_BYTES_SIZE(_VarRecord);


// >>> #CE_3.1.0 2025-08-18
// ----------------------------------------
// The datatype describes an element of the variables HashTable
//#define	VAR_ARRAY_SIZE	8	// >>> #CE_3.1.0 2026-09-16 : DEPRECATED : no much speed benefits observed, but big impact on memory use
#define	VAR_ARRAY_SIZE	0
typedef struct {
	_VarRecord	*vlist;						// dynamic variable list (pointer to the first variable)
#if (VAR_ARRAY_SIZE > 0)
	_VarRecord	varray[VAR_ARRAY_SIZE];		// static variable array	[#CE_3.1.0 2026-08-18]
#endif
}  _HashTableRec;
// <<< #CE_3.1.0 2025-08-18

// ----------------------------------------
// Тип описывает элемент таблицы контекстов
// Variable Context data
// ----------------------------------------
typedef struct {
	int32			key;
//	_HashTableRec	*hktbl;		// *pointer* to the HashTable (created dynamically)
	_HashTableRec	hktbl[VAR_HASH_TABLE_SIZE];	// HashTable	[#CE_3.1.0 2026-08-18]
	LStrHandle		Creator;	// name of VI created context (not handled by VM)
	LStrHandle		vcName;		// name of context
	int32			LifeTime;	// declared context's lifetime (not handled by VM)
//	int32			reserved;
	int32			misc[CNTX_MISC_FIELDS];	// miscellaneous fields
} _CTableEl;
STATIC_ASSERT_PNT_BYTES_OFFSET(_CTableEl,hktbl);
STATIC_ASSERT_PNT_BYTES_SIZE(_CTableEl);

// --------------------------------
// Тип описывает таблицу контекстов
// Context Table (= Array of Variable Contexts)
// --------------------------------
typedef struct {
	int32		dimSize;
	_CTableEl	data[1];
} _CTable;
STATIC_ASSERT_PNT_BYTES_OFFSET(_CTable,data);
STATIC_ASSERT_PNT_BYTES_SIZE(_CTable);

typedef _CTable **_CTableHandle;
//typedef	_CTable	 *_CTablePtr;		// >>> #CE_3.0.2 2026-06-18

// -----------------------------------------------
// Структура используется при анализе имени/алиаса
// Used for parsing Variable_Name/Alias: [name][=alias])
// -----------------------------------------------
typedef struct {
	Bool32	flgName;	// флаг: имя присутствует
	char	*pntName;	// указатель на имя
	int32	lenName;	// длина имени
	Bool32	flgAlias;	// флаг: алиас присутствует
	char	*pntAlias;	// указатель на алиас
	int32	lenAlias;	// длина алиаса
} _NameAliasInfo;

#endif
