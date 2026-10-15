/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__RSRC_MGR_H__
#define	__RSRC_MGR_H__

#include <extcode.h>

#ifdef __cplusplus
extern "C" {
#endif


// ================================================
//		LabVIEW RefNum Manager
// ================================================

#define __NotRefNum		((LVRefNum)0)
#define __NotRefData	((LVRefNumData)0)

//typedef	uInt32	LVRefNum;
typedef	size_t	LVRefNumData;

// Codes of a RefNum types
enum {
	cerefKind_CEInstance	= 0x01,	// CE Instance
	cerefKind_CSEInstance	= 0x02	// CSE Instance Creator
};
// -----------------------------------

extern	void	ceref_Initializing(void);
extern	void	ceref_Finalizing(void);

// ***** RefNum Functions
extern	LVRefNum		ceref_Create    (LVRefNumData dat,  uInt8 kind,  MgErr *errp);
extern	void			ceref_Destroy   (LVRefNum     ref,  MgErr *errp);
extern	LVRefNumData	ceref_GetRefData(LVRefNum     ref,  MgErr *errp);
extern	LVRefNum		ceref_GetRefnum	(LVRefNumData dat,  MgErr *errp);



// ================================================
//		Resource Access Manager
// ================================================

enum {
	cermCmd_Nothing	= 0,
	cermCmd_LockRsrc,
	cermCmd_FreeRsrc,
	cermCmd_FreeReliable,		// without check of pointer and signature
	cermCmd_CheckRsrc,
	cermCmd_SetAsyncMsg
};

typedef struct {
	uInt32	signature;
	int32	LockTag;
	int32	AsyncMsg;
} __cersamg_RsrcHeader;
// -----------------------------------

// ***  Resource Access Manager: core function
extern	void *	__cdecl	cersamg( int32 cmd, __cersamg_RsrcHeader *ihp, uInt32 signature);

// ***  May only be called in DllMain()  ***
extern	void	__cdecl	cersamg_Initializing(void);
extern	void	__cdecl	cersamg_Finalizing(void);



// ================================================
//		Resource Store Manager
// ================================================

// Action codes supported by CE Basic Resource Manager
enum {
	cersCmd_Nothing = 0,
	cersCmd_TagFind,
	cersCmd_TagInsert,
	cersCmd_TagRemove,
	cersCmd_TagDelAny
};

// Codes of a few predefined owners
enum {
	cersOwner_CEInstance	= 'ceic',	// CE Instance Creator
	cersOwner_CESPInstance	= 'spic'	// CSE Instance Creator
};

// ================================================

// ***  Resource Store Manager: core function and pointer to it: type definitions
// ***
typedef	MgErr	__cdecl	__ceRStore(int32 cmd, int32 tagOwner, size_t *tagValue);
typedef __ceRStore  *	__ceRStore_Pnt;


// ***  Resource Store Manager: core function
extern	__ceRStore	cerstor;
//	extern  MgErr	__cdecl	cerstor(int32 cmd, int32 tagOwner, size_t *tagValue);

// ***  May only be called in DllMain()  ***
extern	void	__cdecl	cerstor_Initializing(void);
extern	void	__cdecl	cerstor_Finalizing(void);


#ifdef __cplusplus
}
#endif

#endif
