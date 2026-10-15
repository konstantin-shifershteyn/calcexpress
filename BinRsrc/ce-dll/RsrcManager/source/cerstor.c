/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// Resource Store Manager
// -------------------------------------------

#define	WINDOWS_IGNORE_PACKING_MISMATCH
#include <windows.h>

#include <celib\ceMemLib.h>
#include <RsrcManager\RsrcManager.h>


// ========  Private Type  =================
typedef struct	_cersrc_rec {
	struct	_cersrc_rec		*link;
			size_t			tagValue;
			int32			tagOwner;
} _CE_RsrcRec;

// ========  Private Data  =================
static	CRITICAL_SECTION	csTag;
static	Bool32				Initialized	= FALSE;
static _CE_RsrcRec			*rsrcList	= NULL;

// ========  Private Functions  ============
MgErr cersFindOut(int32 tagOwner, size_t *tagValue);
MgErr cersInsert (int32 tagOwner, size_t *tagValue);
MgErr cersExtract(int32 tagOwner, size_t *tagValue, Bool32 anyTag);


// ===========  Initializing Routine  =======
void	cerstor_Initializing(void)
{
	if (!Initialized) {
		InitializeCriticalSection(&csTag);
		rsrcList	= NULL;
		Initialized	= TRUE;
	}
}

// ===========  Finalizing Routine  =========
void	cerstor_Finalizing(void)
{
	_CE_RsrcRec *curr;

	if (Initialized) {

		EnterCriticalSection(&csTag);

		// *** Destroy Resource List if it isn't empty
		while (rsrcList) {
			curr	 = rsrcList;
			rsrcList = curr->link;
			DSDisposePtr(curr);		// уничтожить запись
		}
		LeaveCriticalSection(&csTag);

		DeleteCriticalSection(&csTag);
		rsrcList	= NULL;
		Initialized	= FALSE;
	}
}


// ===========  Main Function  =====================
MgErr cerstor(	int32 cmd, int32 tagOwner, size_t *tagValue)
{
	MgErr	err;

	switch(cmd) {
		case cersCmd_Nothing:
			err = noErr;
			break;

		case cersCmd_TagFind:
			EnterCriticalSection(&csTag);
				err = cersFindOut(tagOwner, tagValue);
			LeaveCriticalSection(&csTag);
			break;

		case cersCmd_TagInsert:
			EnterCriticalSection(&csTag);
				err = cersInsert(tagOwner, tagValue);
			LeaveCriticalSection(&csTag);
			break;

		case cersCmd_TagRemove:
			EnterCriticalSection(&csTag);
				err = cersExtract(tagOwner, tagValue, FALSE);
			LeaveCriticalSection(&csTag);
			break;

		case cersCmd_TagDelAny:
			EnterCriticalSection(&csTag);
				err = cersExtract(tagOwner, tagValue, TRUE);
			LeaveCriticalSection(&csTag);
			break;

		default:
			err = mgArgErr;
			break;
	}
	return err;
}


// ====================================================================

MgErr cersFindOut(int32 tagOwner, size_t *tagValue)
{
	register _CE_RsrcRec *curr;

	curr = rsrcList;

	while(curr) {
		if (curr->tagOwner == tagOwner) {	// нашли tagOwner
			if (curr->tagValue == *tagValue)
				return noErr;
		}
		curr = curr->link;
	}
	return rNotFound;
}

// ====================================================================
MgErr cersExtract(int32 tagOwner, size_t *tagValue, Bool32 anyTag)
{
	register _CE_RsrcRec *prev, *curr;

	prev = NULL;
	curr = rsrcList;

	while(curr) {
		if (curr->tagOwner == tagOwner) {	// нашли tagOwner
			if (anyTag) {
				*tagValue = curr->tagValue;
			}
			if (curr->tagValue == *tagValue) {
				// нашли запись, удалить ее из списка
				if (prev)
					prev->link = curr->link;
				else	rsrcList   = curr->link;
				// уничтожить запись
				DSDisposePtr(curr);
				return noErr;
			}
		}
		prev = curr; curr = curr->link;
	}
	return rNotFound;
}

// ================================================================
MgErr cersInsert( int32 tagOwner, size_t *tagValue)
{
	_CE_RsrcRec 	*curr;
	LVBoolean		errf = LVFALSE;
	MgErr			err  = noErr;

	// создать элемент списка
	curr = (_CE_RsrcRec*)ce_DSNewPtr( sizeof(_CE_RsrcRec));
	if (!curr)
		return mFullErr;	// rAddFailed;
	curr->tagOwner =  tagOwner;
	curr->tagValue = *tagValue;

	// поместить в список
	curr->link = rsrcList;
	rsrcList   = curr;

	return noErr;
}
