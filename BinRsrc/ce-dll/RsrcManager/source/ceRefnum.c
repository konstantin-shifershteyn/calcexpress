/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// LabVIEW RefNums manager
// -------------------------------------------

#define	WINDOWS_IGNORE_PACKING_MISMATCH
#include <windows.h>

#include <celib\ceMemLib.h>
#include <RsrcManager\RsrcManager.h>


// ========  Private Type  =================
typedef struct	_RefNum_struct {
	struct	_RefNum_struct		*link;
			LVRefNumData		data;
			LVRefNum			refnum;
} _RefNumRec;

// ========  Private Data  =================
static	CRITICAL_SECTION	csRef;
static	Bool32				Initialized	= FALSE;
static _RefNumRec			*refnumList	= NULL;

// ========  Private Functions  ============

// ---------------------------
void	_ceref_clear_record(_RefNumRec *rec)
{
	rec->link		= NULL;
	rec->data		= __NotRefData;
	rec->refnum		= __NotRefNum;
}
// ---------------------------
_RefNumRec * _ceref_find_refnum(LVRefNum refnum)
{
	_RefNumRec *curr = refnumList;
	if (refnum == __NotRefNum)
		return NULL;
	while (curr) {
		if (curr->refnum == refnum)
			break;
		curr = curr->link;
	}
	return curr;
}
// ---------------------------
_RefNumRec * _ceref_find_refdata(LVRefNumData data)
{
	_RefNumRec *curr = refnumList;
	while (curr) {
		if (curr->data == data)
			break;
		curr = curr->link;
	}
	return curr;
}
// ---------------------------
LVRefNum	_ceref_unique_refnum(uInt8 kind)
{
static	uInt32		cnt = 0;
		LVRefNum	refnum;

	for(;;) {
		cnt += 1;
		cnt &= 0x00FFFFFF;
		if (cnt == 0)
			cnt = 1;
		refnum = (LVRefNum)(cnt | ((uInt32)kind << 24));
		if (_ceref_find_refnum(refnum) == __NotRefNum)
			break;
	}
	return refnum;
}

// ===========  Initializing Routine  =======
void	ceref_Initializing(void)
{
	if (!Initialized) {
		InitializeCriticalSection(&csRef);
		refnumList	= NULL;
		Initialized	= TRUE;
	}
}
// ===========  Finalizing Routine  =========
void	ceref_Finalizing(void)
{
	_RefNumRec *curr;

	if (!Initialized)
		return;

	EnterCriticalSection(&csRef);
	// *** Destroy Resource List if it isn't empty
	while (refnumList) {
		curr			= refnumList;
		refnumList		= curr->link;
		_ceref_clear_record(curr);		// reset record's data
		DSDisposePtr(curr);				// destroy record
	}
	LeaveCriticalSection(&csRef);

	DeleteCriticalSection(&csRef);
	refnumList	= NULL;
	Initialized	= FALSE;
}



// ===========  Create a RefNum  ==================
LVRefNum	ceref_Create(LVRefNumData val,  uInt8 kind,  MgErr *errp)
{
	LVRefNum	refnum = __NotRefNum;
	_RefNumRec	*curr;
	MgErr		err  = noErr;

	EnterCriticalSection(&csRef);		// ######

	// create a Refnum List record
	curr = (_RefNumRec*)DSNewPClr(sizeof(_RefNumRec));
	if (!curr) {
		err = mFullErr;	// rAddFailed;
		goto out;
	}
	refnum			= _ceref_unique_refnum(kind);
	curr->refnum	= refnum;
	curr->data		= val;

	// insert to the list
	curr->link	= refnumList;
	refnumList	= curr;
out:
	LeaveCriticalSection(&csRef);		// ######
	if (errp)
		*errp = err;
	return refnum;
}

// ===========  Destroy a RefNum  ==================
void  ceref_Destroy(LVRefNum ref,  MgErr *errp)
{
	_RefNumRec	*prev = __NotRefNum;
	_RefNumRec	*curr = refnumList;
	MgErr		err   = noErr;

	if (ref == __NotRefNum) {
		err = mZoneErr;
		goto exit;
	}

	EnterCriticalSection(&csRef);		// ######
	while(curr) {
		if (curr->refnum == ref) {
			if (prev)	prev->link = curr->link;
			else		refnumList = curr->link;
			_ceref_clear_record(curr);
			DSDisposePtr(curr);
			goto out;
		}
		prev = curr; curr = curr->link;
	}
	err = rNotFound;
out:
	LeaveCriticalSection(&csRef);		// ######
exit:
	if (errp)
		*errp = err;
	return;
}

// ===========  Find RefNum and return RefData  ==================
LVRefNumData ceref_GetRefData(LVRefNum ref,  MgErr *errp)
{
	LVRefNumData	data = __NotRefData;
	_RefNumRec		*curr;
	MgErr			err   = noErr;

	if (ref == __NotRefNum) {
		err = mZoneErr;
		goto exit;
	}

	EnterCriticalSection(&csRef);		// ######
	curr = _ceref_find_refnum(ref);
	if (curr == NULL) {
		err = rNotFound;
	}
	else {
		data = curr->data;
	}
	LeaveCriticalSection(&csRef);		// ######
exit:
	if (errp)
		*errp = err;
	return data;
}

// ===========  Find RefData and return RefNum  ==================
LVRefNum ceref_GetRefnum(LVRefNumData data,  MgErr *errp)
{
	LVRefNum		refnum = __NotRefNum;
	_RefNumRec		*curr;
	MgErr			err   = noErr;

	EnterCriticalSection(&csRef);		// ######
	curr = _ceref_find_refdata(data);
	if (curr == NULL) {
		err = rNotFound;
	}
	else {
		refnum = curr->refnum;
	}
	LeaveCriticalSection(&csRef);		// ######
	if (errp)	*errp = err;
	return refnum;
}


// ######################################################################
// The function is intended to be called from LV VI
// it returns ALL registered refnums and associated data in two LV arrays
// ######################################################################

#include "lv_prolog.h"

typedef struct {
	int32_t dimSize;
	uint32_t elt[1];
	} TD1;
typedef TD1 **TD1Hdl;

typedef struct {
	int32_t dimSize;
	uint64_t elt[1];
	} TD2;
typedef TD2 **TD2Hdl;

#include "lv_epilog.h"
// -----------------------------------------------------

MgErr viGetRefnums(int32 *pCount, TD1Hdl RefNums, TD2Hdl RefData)
{
	_RefNumRec *curr = refnumList;
	int32		i, counter = 0;
	MgErr		err = noErr;

	EnterCriticalSection(&csRef);		// ######

	// count elements
	while (curr) {
		counter += 1;
		curr = curr->link;
	}
	*pCount = counter;

	// allocate output arrays
	if ((err = NumericArrayResize(uL, 1, (UHandle*)&RefNums, counter)) != noErr)
		goto out;
	(*RefNums)->dimSize = counter;
	

	if ((err = NumericArrayResize(uQ, 1, (UHandle*)&RefData, counter)) != noErr)
		goto out;
	(*RefData)->dimSize = counter;

	// fill the arrays
	curr = refnumList;
	for (i=0; i < counter; i++) {
		(*RefNums)->elt[i] = curr->refnum;
		(*RefData)->elt[i] = curr->data;
		curr = curr->link;
		if (!curr)
			break;
	}
out:
	LeaveCriticalSection(&csRef);		// ######
	return err;
}

