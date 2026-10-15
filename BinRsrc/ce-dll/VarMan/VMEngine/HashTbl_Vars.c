/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <VMEngine_Routines.h>


// #####################################################################################################################

_HashTableRec * HashTable_create(void) { return (_HashTableRec*)ce_DSNewPClr( sizeof(_HashTableRec) * VAR_HASH_TABLE_SIZE ); }

MgErr			Var_Destroy(_HashTableRec	*hktbl,
							LStrHandle name);
MgErr			Var_Create(	_HashTableRec	*hktbl,
							uChar			*pntName,
							int32			lenName,
							_VarInfo		*pars,
							uInt32			writerID,		// >>> #CE_3.0.7 2026-07-23
							_VM_EXT_FUNC	*extfunc,
							void			*extdata,
							_VarRecord		**vpp,
							LVBoolean		*err_f,
							uInt16			*err_c);

// #####################################################################################################################

// -----------------------------
// Functions to calculate hash-key
// -----------------------------

// $$$$$$$$$$$$$$$$$$$$$$$$$$
__forceinline uInt32 __fastcall HashKey_v1(const uChar * __restrict pnt, int32 len)
{
    uInt8 hk = 0xD0;
	const uChar *end = pnt + len;

	while (pnt < end) {
		hk = ((hk << 3) | (hk >> 5)) ^ *pnt++;
	}
#if VAR_HASH_KEY_MASK == 0xFF
	return (uInt32)hk;
#else
    return (uInt32)hk & VAR_HASH_KEY_MASK;
#endif
}
// $$$$$$$$$$$$$$$$$$$$$$$$$$
__forceinline uInt32 __fastcall HashKey_v2(const uChar * __restrict pnt, int32 len)
{
    uInt8 hk = 0xD0;
	uInt8 c;
	const uChar *end = pnt + len;
	while (pnt < end) {
		c  = *pnt++;
		c = (c ^ (c >> 3)) + (c << 2);
		hk = ((hk << 3) | (hk >> 5)) ^ c;
	}
	return (uInt32)hk;
}
// $$$$$$$$$$$$$$$$$$$$$$$$$$
__forceinline uInt32 __fastcall HashKey_v3(const uChar * __restrict pnt, int32 len)
{
    uInt16 hk = 0xD0D0;
	const uChar *end = pnt + len;

	while (pnt < end) {
		hk = ((hk << 3) | (hk >> 13)) ^ *pnt++;
	}
	return (uInt32)(hk ^ (hk >> 8)) & 0xFF;
}
// $$$$$$$$$$$$$$$$$$$$$$$$$$
__forceinline uInt32 __fastcall HashKey_v4(const uChar * __restrict pnt, int32 len)
{
    uInt8 hk = 0xD0;
	const uChar *end = pnt + len;
	const uChar *fin = pnt + (len & ~3);	// Align end to a multiple of 4
	while (pnt < fin) {
		hk = ((hk << 3) | (hk >> 5)) ^ pnt[0];
		hk = ((hk << 3) | (hk >> 5)) ^ pnt[1];
		hk = ((hk << 3) | (hk >> 5)) ^ pnt[2];
		hk = ((hk << 3) | (hk >> 5)) ^ pnt[3];
		pnt += 4;
	}
	while (pnt < end) {
		hk = ((hk << 3) | (hk >> 5)) ^ *pnt++;
	}
	hk = ((hk << 3) | (hk >> 5)) ^ (uInt8)len;

#if VAR_HASH_KEY_MASK == 0xFF
	return (uInt32)hk;
#else
    return (uInt32)hk & VAR_HASH_KEY_MASK;
#endif
}
// $$$$$$$$$$$$$$$$$$$$$$$$$$
// FNV-1a algorithm
#define FNV_PRIME_32  0x01000193U   // Decimal: 16777619
#define FNV_OFFSET_32 0x811C9DC5U   // Decimal: 2166136261

__forceinline uInt32 __fastcall HashKey_fnv_1a(const uChar * __restrict pnt, int32 len)
{
    uInt32		hash = FNV_OFFSET_32;
	const uChar	*end = pnt + len;

    // 1. Process 4 bytes at a time to reduce loop conditions
    while (pnt < end) {
        //hash = (hash ^ *pnt++) * FNV_PRIME_32;
		hash ^= *pnt++;
		hash *= FNV_PRIME_32;
    }

	// 2. Inject length
//	hash ^= (uInt32)len;
//	hash *= FNV_PRIME_32;

	// 3. Avalanche Fold
//	hash ^= (hash >> 16);	hash *= FNV_PRIME_32;
//	hash ^= (hash >> 24);	hash *= FNV_PRIME_32;
//	hash ^= (hash >> 16);	hash *= FNV_PRIME_32;
//	hash ^= (hash >> 8);	hash *= FNV_PRIME_32;

//	return hash & VAR_HASH_KEY_MASK;
	return ( (hash ^ (hash >> 8) ^ (hash >> 16) ^ (hash >> 24)) & VAR_HASH_KEY_MASK );


}

// #####################################################################################################################

#define	CALC_HASH_KEY_SELECTABLE	0	//1

int32	__HashKeyMode = 0;

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// test function is to be called from a test VI
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
int32 __cdecl VMTest_HashKey_SetMode(int32 mode)
{
#if (CALC_HASH_KEY_SELECTABLE > 0)
	int32	oldMode = __HashKeyMode;
	__HashKeyMode = mode;
	return oldMode;
#else
	return -1;
#endif
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// master function to calculate hash key
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
__forceinline uInt32 __fastcall HashKey(const uChar * __restrict pnt, int32 len)
{
#if (CALC_HASH_KEY_SELECTABLE > 0)
	switch (__HashKeyMode) {
		case 0:		break;
		case 1:		return HashKey_v1(pnt, len);
		case 2:		return HashKey_v2(pnt, len);
		case 3:		return HashKey_v3(pnt, len);
		case 4:		return HashKey_v4(pnt, len);
		case 5:		return HashKey_fnv_1a(pnt, len);
		default:	break;
	}
	{
#endif
//	return HashKey_actual(pnt, len);
	// ### ACTUAL HASH-KEY CALCULATION ###
    uInt8 hk = 0xD0;
	const uChar *end = pnt + len;
	const uChar *fin = pnt + (len & ~3);	// Align end to a multiple of 4
	while (pnt < fin) {
		hk = ((hk << 3) | (hk >> 5)) ^ pnt[0];
		hk = ((hk << 3) | (hk >> 5)) ^ pnt[1];
		hk = ((hk << 3) | (hk >> 5)) ^ pnt[2];
		hk = ((hk << 3) | (hk >> 5)) ^ pnt[3];
		pnt += 4;
	}
	while (pnt < end) {
		hk = ((hk << 3) | (hk >> 5)) ^ *pnt++;
	}
#if VAR_HASH_KEY_MASK == 0xFF
	return (uInt32)hk;
#else
    return (uInt32)hk & VAR_HASH_KEY_MASK;
#endif
#if (CALC_HASH_KEY_SELECTABLE > 0)
	}
#endif
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// test function is to be called from a test VI
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
uInt32 __cdecl VMTest_HashKey(uInt32 mode, int32 repeat, uChar *pnt, int32 len)
{
	int32	i, hash;
	if (repeat <= 0)
		repeat = 1;
	for (i=0; i<repeat; i++) {
		switch (mode) {
//			case 0:		hash = HashKey_actual(pnt, len);	break;
			case 1:		hash = HashKey_v1(pnt, len);		break;
			case 2:		hash = HashKey_v2(pnt, len);		break;
			case 3:		hash = HashKey_v3(pnt, len);		break;
			case 4:		hash = HashKey_v4(pnt, len);		break;
			case 5:		hash = HashKey_fnv_1a(pnt, len);	break;
			default:	hash = HashKey(pnt, len);			break;
		}
	}
	return hash;
}


// #####################################################################################################################

// >>> #CE_3.1.0 2026-08-18
__forceinline _VarRecord * __fastcall Var_FindName( _HashTableRec *htrec, uChar *namePnt, int32 nameLen)
{
	_VarRecord *vp;

#if (VAR_ARRAY_SIZE > 0)
	int32		i;
	// ### search in the var array
	vp = &(htrec->varray[0]);
	for (i=0; i < VAR_ARRAY_SIZE; i++, vp++) {
		if (vp->name == NULL)		// if not used
			continue;
		if (nameLen == vp->nameLen) {
			if (!memcmp(namePnt, vp->namePnt, nameLen))
				return vp;
		}
	}
#endif

//DbgPrintf(__FUNCTION__"(): search in var list\n");
	// ### search in the var list
	for (vp = htrec->vlist; vp != NULL; vp = vp->link) {
		if (nameLen == vp->nameLen) {
			if (!memcmp(namePnt, vp->namePnt, nameLen))
				return vp;
		}
	}
	return NULL;
}
////////////////////////////////////////////
#if (VAR_ARRAY_SIZE > 0)
// >>> #CE_3.1.0 2026-08-18
__forceinline _VarRecord * __fastcall Var_FindFreeArrayElement(_HashTableRec *htrec)
{
	_VarRecord	*vp;
	int32		i;

	// ### search in the var array
	for (i=0; i < VAR_ARRAY_SIZE; i++) {
		vp = &(htrec->varray[i]);
		if (vp->name == NULL)		// if not used
			return vp;
	}
	return NULL;
}
// <<< #CE_3.1.0 2026-08-18
#endif

// ######################################################################

MgErr Var_Create(
				_HashTableRec	*hktbl,
				uChar			*pntName,
				int32			lenName,
				_VarInfo		*pars,
				uInt32			writerID,		// >>> #CE_3.0.7 2026-07-23
				_VM_EXT_FUNC	*extfunc,		//extension function
				void			*extdata,		//extension data
				_VarRecord		**vpp,
				LVBoolean		*err_f,
				uInt16			*err_c)
{
	int32		hkey;
	_VarRecord	*vp;
	MgErr		err = noErr;
	Bool32		dynamic = FALSE;

	if (pars->type == OPTYPE_ANY_FLATTENED) {	// This type is not allowed !!!
		*err_f	= LVTRUE;
		*err_c	= VM_ERR_VAR_INVTYPE;
		return noErr;
	}

	// Поиск переменной для предотвращения переопределения
	// ---------------------------------------------------
	hkey = HashKey(pntName, lenName);	// *** Calculate hash-key

	if ( Var_FindName( &(hktbl[hkey]), pntName, lenName) ) {
		*err_f = LVTRUE;
		*err_c = VM_ERR_VAR_NONUNIQUE;
		return noErr;
	}

	// Выделить память под запись о переменной
	// ---------------------------------------
#if (VAR_ARRAY_SIZE > 0)
// >>> #CE_3.1.0 2026-08-18
	vp = Var_FindFreeArrayElement(&(hktbl[hkey]));
	if (!vp) {
		dynamic = TRUE;
//		DbgPrintf(__FUNCTION__"(): add to var list\n");
		if (!(vp = (_VarRecord *)ce_DSNewPtr(sizeof(_VarRecord)) ))
			return  mFullErr;
	}
// <<< #CE_3.1.0 2026-08-18
#else
	dynamic = TRUE;
	if (!(vp = (_VarRecord *)ce_DSNewPtr(sizeof(_VarRecord)) ))
		return  mFullErr;
#endif

	// ***	save the variable name
	if (err = LStr_CreateFromMem( &(vp->name), pntName, lenName) )
		goto ErrOut;

	// ***	init private parameters
	vp->namePnt	= (char*)LStrBuf(*(vp->name));		// >>> #CE_3.0.2 2026-06-18
	vp->nameLen	= LStrLen(*(vp->name));				// >>> #CE_3.0.2 2026-06-18
	vp->hist		= NULL;		// no history assigned
	vp->alias		= NULL;		// no alias assigned
	vp->attrib		= NULL;		// no attribute assigned

// >>> #CE_3.1.0-r6 2026-09-19	// >>> #CE_3.0.7 2026-07-23
	vp->writerID	= vp->creatorID	= writerID;					
	vp->writeMS		= vp->createMS	= MillisecsTimer();				
	vp->writeTS		= vp->createTS	= __GetTime_Labview_DBL();
	vp->writeCnt	= 0;
	//__UpdateVarWritingTimeNow(vp);
// <<< #CE_3.1.0-r6 2026-09-19


	// ***	save type & initial value
	vp->inf.type	= pars->type;
	vp->inf.num	= pars->num;
	vp->inf.str	= NULL;

	if (extfunc) {
		// ***	call an extension function if any is specified
		err = extfunc(vp->name, &(vp->inf), pars, extdata, err_f, err_c);
	}
	else {
		// ***	Копируем параметры переменной если нет внешней функции
		switch (vp->inf.type) {
			case VM_TYPE_DBL :
			case VM_TYPE_IDXRANGE :
				// don't allocate string: vp->inf.str = NULL !!!
				break;
			default :
// >>> #CE_3.0.4 2026-07-06
				//err = LStr_CopyOrCreate(&(vp->inf.str), pars->str);
				err = ce_CopyOrCreate_OperandString(&(vp->inf.str), pars->str, vp->inf.type);
				break;
// <<< #CE_3.0.4 2026-07-06
		}
	}
	if (err) {
		DSDisposeHandle(vp->name);
		goto ErrOut;
	}

	if (dynamic) {
		// ### add the variable to the var list
		vp->link			= hktbl[hkey].vlist;
		hktbl[hkey].vlist	= vp;
	}
	*vpp = vp;
	return err;

ErrOut:
	*vpp	= NULL;
	DSDisposePtr(vp);
	return err;
}

// ######################################################################

MgErr __fastcall Var_free_mem(_VarRecord *vp, Bool32 dynamic)
{
	MgErr	err, errtmp;

	err = noErr;
	if (vp) {

		if (++_DelVarCnt == 0) _DelVarCnt++;	// >>> #CE_3.0.2 2026-06-19

		if (errtmp = Alias_Destroy(vp->alias))			err = errtmp;
		if (errtmp = _VHistory_Destroy(vp))				err = errtmp;
		// ---
		if (vp->attrib) {
			if (errtmp = DSDisposeHandle(vp->attrib) )	err = errtmp;
			vp->attrib = NULL;					// >>> #CE_3.0.2 2026-06-17
		}
		// ---
		if (errtmp = DSDisposeHandle(vp->name) )		err = errtmp;
		vp->name = NULL;						// >>> #CE_3.0.2 2026-06-17
		vp->namePnt = NULL;						// >>> #CE_3.0.2 2026-06-18
		vp->nameLen = 0;

		if (vp->inf.str) {
			if (errtmp = DSDisposeHandle(vp->inf.str))	err = errtmp;
			vp->inf.str = NULL;					// >>> #CE_3.0.2 2026-06-17
		}
		if (dynamic) {
			if (errtmp = DSDisposePtr(vp) )			err = errtmp;
		}
	}
	return err;
}

// ######################################################################

MgErr Var_Destroy( _HashTableRec *hktbl, LStrHandle name)
{
	_VarRecord	*vp, *prev;
	int32		i=0, hkey;

	// -------------------------------
	// вычислить индекс в Hash-таблице
	// -------------------------------
	hkey = HashKey( LStrBuf(*name), LStrLen(*name) );

#if (VAR_ARRAY_SIZE > 0)
	// ### search in var array
	vp = &(hktbl[hkey].varray[0]);
	for (i=0; i < VAR_ARRAY_SIZE; i++, vp++) {
		if (vp->name == NULL)		// if not used
			continue;
		if ( LStr_Cmp(name, vp->name) ) {
			// the variable is found
			return Var_free_mem(vp, FALSE);
		}
	}
#endif

	// ### search in var list
	vp  = hktbl[hkey].vlist;		// pointer to the first variable in the Var List
	prev = NULL;					// pointer to the previous variable in the list
	while(vp) {
		if ( LStr_Cmp(name, vp->name) ) {
			// the variable is found
			if (prev)	prev->link			= vp->link;
			else		hktbl[hkey].vlist	= vp->link;
			return	Var_free_mem(vp, TRUE);
		}
		prev = vp;
		vp  = vp->link;
	}
	return noErr;
}


// ######################################################################
// >>> #CE_3.0.2 2026-06-14 : moved to \include\VarMan\VMEngine_Routines.h
/*
__inline MgErr __fastcall Var_Copy( _VarInfo *dst, _VarInfo *src)
{
	dst->num	= src->num;
	switch( dst->type = src->type) {
		case VM_TYPE_DBL :
		case VM_TYPE_IDXRANGE :	return noErr;
	}
	if ((dst->str) && (src->str))	// do not copy a string if either dst->str == NULL or src->str == NULL
		return  LStr_Copy(dst->str, src->str);
	return noErr;
}
*/

// ######################################################################

MgErr HashTable_destroy(_HashTableRec *hktbl)
{
	int32		i_ht=0, i_va=0;
	_VarRecord	*vp, *vlist;
	MgErr		err = noErr, errtmp;

	if (!hktbl) goto out;

#if (VAR_ARRAY_SIZE > 0)
	// ### release data in the var array
	for (i_ht=0; i_ht < VAR_HASH_TABLE_SIZE; i_ht++) {
		vp = &(hktbl[i_ht].varray[0]);
		for (i_va=0; i_va < VAR_ARRAY_SIZE; i_va++, vp++) {
			if (vp->name == NULL)		// if not used
				continue;
			if (errtmp = Var_free_mem(vp, FALSE))
				err = errtmp;
		}
	}
#endif
	// ### release the variable list
	for (i_ht=0; i_ht < VAR_HASH_TABLE_SIZE; i_ht++) {
		vlist = hktbl[i_ht].vlist;		// pointer to the first variable in the Var List
		while (vlist) {
			vp		= vlist;
			vlist	= vlist->link;
			if (errtmp = Var_free_mem(vp, TRUE))
				err = errtmp;
		}
		hktbl[i_ht].vlist = NULL;		// set the Var List empty!	[#CE_3.1.0 2026-09-16]
	}
out:
	return err;
}

