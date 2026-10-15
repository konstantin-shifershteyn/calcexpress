/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// Resource Access Manager
// -----------------------------------

#define	WINDOWS_IGNORE_PACKING_MISMATCH
#include <windows.h>

#include <RsrcManager\RsrcManager.h>


#define	__CERSAMG_ATTEMPT_MAX		100
#define	__cersamg_WaitProcedure(x)	Sleep(__min((x) * 2, 50))

// ===========  Private Data =======================

static	CRITICAL_SECTION	csTag;
static	int32				Initialized = FALSE;


// ===========  Private Function Prototypes  =======

__inline void * __cersamg_lock		(__cersamg_RsrcHeader *ihp, uInt32 sign);
__inline void * __cersamg_unlock	(__cersamg_RsrcHeader *ihp, uInt32 sign);
__inline void * __cersamg_unlock_quick	(__cersamg_RsrcHeader *ihp);
__inline void * __cersamg_check		(__cersamg_RsrcHeader *ihp, uInt32 sign);
__inline void * __cersamg_async_msg	(__cersamg_RsrcHeader *ihp, uInt32 sign, int32 msg);


// ===========  Initializing Routine  =======
void	cersamg_Initializing(void)
{
	if (!Initialized) {
		InitializeCriticalSection(&csTag);
		Initialized = TRUE;
	}
}

// ===========  Finalizing Routine  =========
void	cersamg_Finalizing(void)
{
	if (Initialized) {
		DeleteCriticalSection(&csTag);
		Initialized = FALSE;
	}
}

// ===========  Main Function  =====================

void * __cdecl cersamg(	int32					cmd,
						__cersamg_RsrcHeader	*ihp,
						uInt32					signature)
{
	switch(cmd) {

		case cermCmd_LockRsrc :
			return __cersamg_lock(ihp, signature);

		case cermCmd_FreeRsrc :
			return __cersamg_unlock(ihp, signature);

		case cermCmd_FreeReliable :
			return __cersamg_unlock_quick(ihp);

		case cermCmd_CheckRsrc :
			return __cersamg_check(ihp, signature);

		case cermCmd_SetAsyncMsg :
			return __cersamg_async_msg(ihp, signature, TRUE);

	}
	return NULL;
}

// ================  SUBROUTINES  ===========================

__inline void * Priv_cersamg_check( __cersamg_RsrcHeader *ihp, uInt32 sign)
{
	if (ihp == NULL)			return NULL;
	if ( DSCheckPtr(ihp))		return NULL;
	if (ihp->signature != sign)	return NULL;
	return ihp;
}

// ===========================================
__inline void * __cersamg_check( __cersamg_RsrcHeader *ihp, uInt32 sign)
{
	EnterCriticalSection(&csTag);

	ihp = Priv_cersamg_check(ihp, sign);

	LeaveCriticalSection(&csTag);
	return ihp;
}

// ===========================================
__inline void * __cersamg_lock( __cersamg_RsrcHeader *ihp, uInt32 sign)
{
	int32	counter = 0;

	for (;;) {
		EnterCriticalSection(&csTag);

		// *****  Проверить указатель на контекст
		if ( !(ihp = Priv_cersamg_check(ihp, sign)) )
			break;

		// *****  Если ресурс свободен, то захватить и выход
		if ( !(ihp->LockTag) ) {
			ihp->LockTag = TRUE;
			break;
		}
		// *****  Если ресурс занят

		LeaveCriticalSection(&csTag);

		// *****  Выход если количество попыток исчерпано
		if (++counter >= __CERSAMG_ATTEMPT_MAX)
			return NULL;

		// *****  Ждать
		__cersamg_WaitProcedure(counter);
	}
	LeaveCriticalSection(&csTag);
	return ihp;
}

// Освободить может только тот Caller (CIN), который захватил ресурс

// ===========================================
__inline void * __cersamg_unlock_quick( __cersamg_RsrcHeader *ihp)
{
//	EnterCriticalSection(&csTag);
	ihp->LockTag = FALSE;		// освободить
//	LeaveCriticalSection(&csTag);
	return ihp;
}

// ===========================================
__inline void * __cersamg_unlock( __cersamg_RsrcHeader *ihp, uInt32 sign)
{
//	EnterCriticalSection(&csTag);

	if (ihp = Priv_cersamg_check(ihp, sign)) {	// Проверить указатель на ресурс
		__cersamg_unlock_quick(ihp);		// освободить если OK
//		ihp->LockTag = FALSE;			// освободить если OK
	}
//	LeaveCriticalSection(&csTag);
	return ihp;
}
// ===========================================
__inline void * __cersamg_async_msg( __cersamg_RsrcHeader *ihp, uInt32 sign, int32 msg)
{
	EnterCriticalSection(&csTag);

	// *****  Проверить указатель на контекст
	if (ihp = Priv_cersamg_check(ihp, sign) ) {
		ihp->AsyncMsg = msg;
	}

	LeaveCriticalSection(&csTag);
	return ihp;
}
