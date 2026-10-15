/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr	AliasHashTable_Clear(void);
MgErr	AliasHashTable_DestroyAll(void);

uInt32	AliasHashKey(char *pnt, int32 len);

_AliasRecord *	Alias_Find	(	_AliasRecord *list, char *namePnt, int32 nameLen);
MgErr			Alias_FreeMem(	_AliasRecord *vep);
MgErr			Alias_Destroy(	_AliasRecord *ap);


MgErr			Alias_Create(	char		*pntName,
								int32		lenName,
								_VarRecord	*pVar,
								int32		vcHandle,
								LVBoolean	*err_f,
								uInt16		*err_c);

//__inline _VarRecord *	Alias_FindVar(	LStrHandle	name,
//					LVBoolean	*err_f,
//					uInt16		*err_c);

// -----------------------------
// Функция вычисления Hash-ключа
// -----------------------------
// >>> #CE_3.0 2025-02-14
uInt32 AliasHashKey(char *pnt, int32 len)
{
	uInt8 hk = 0xD0;
	while (len-- > 0) {
		hk = (hk << 3) | (hk >> 5);		// rotate left by 3 bits
		hk ^= *pnt++;
	}
	return (uInt32)hk & ALIAS_HASH_TABLE_MASK;
}
// <<< #CE_3.0 2025-02-14

// ######################################################################

_AliasRecord * Alias_Find(
						_AliasRecord	*list,
						char			*namePnt,
						int32			nameLen)
{
	if (!list) {
		list = aht[ AliasHashKey(namePnt,nameLen) ];
	}
	// найти алиас в списке
	// ----------------
	while(list) {
		if (nameLen == LStrLen(*(list->name)) ) {
			//if (!BlockCmp(namePnt, LStrBuf(*(list->name)), nameLen))
			if (!memcmp(namePnt, LStrBuf(*(list->name)), nameLen))		// >>> #CE_3.0.2 2026-06-06
				break;
		}
		list = list->lnkHashList;
	}
	return list;
}

// ######################################################################

MgErr Alias_Create(	char		*pntName,
					int32		lenName,
					_VarRecord	*vp,
					int32		vcIndex,
					LVBoolean	*err_f,
					uInt16		*err_c)
{
	int32			hkey;
	_AliasRecord	*ap;

	// Does the variable already have an alias?
	// ----------------------------------------
	if (vp->alias) {
		*err_f	= LVTRUE;
		*err_c	= VM_ERR_ALIAS_ATTACHED;
		return noErr;
	}

	hkey = AliasHashKey(pntName, lenName);

	// Поиск алиаса для предотвращения дублирования
	// --------------------------------------------
	if ( Alias_Find( aht[hkey], pntName, lenName) ) {
		*err_f	= LVTRUE;
		*err_c	= VM_ERR_ALIAS_NONUNIQUE;
		return noErr;
	}

	// Выделить память под запись об алиасе
	// ------------------------------------
	if (!(ap = (_AliasRecord *)ce_DSNewPtr(sizeof(_AliasRecord)) ))
		return  mFullErr;

	// ***	сохраняем имя алиаса
	ap->name = (LStrHandle)DSNewHandle( sizeof(LStr) + lenName );
	if (!(ap->name)) {
		DSDisposePtr(ap);
		return mFullErr;
	}
	LStrLen(*(ap->name)) = lenName;
	_MoveBlock(pntName, LStrBuf(*(ap->name)), lenName);

	// ***	сохраняем информацию о переменной
	ap->vcIndex	= vcIndex;	// index in VC Table
	ap->var		= vp;		// pointer to variable record

	// ***	сохранить информацию об алиасе в записе о переменной
	vp->alias	= ap;

	// ***	занести в нужный список таблицы алиасов
	ap->lnkHashList	= aht[hkey];
	aht[hkey]	= ap;

	return noErr;
}

// ######################################################################

MgErr Alias_FreeMem( _AliasRecord *ap)
{
	MgErr	err, errtmp;

	err = noErr;
	if (ap) {
		// *** удалить запись об алиасе в записи переменной
		ap->var->alias	= NULL;

		if (errtmp = DSDisposeHandle(ap->name))	err = errtmp;	// имя алиаса
		if (errtmp = DSDisposePtr(ap))		err = errtmp;	// вся структура
	}
	return err;
}


// ######################################################################

MgErr Alias_Destroy( _AliasRecord *ap)

{
	_AliasRecord	*cur, *prev;
	 int32			idx;

	if (!ap)
		return noErr;

	// -------------------------------
	// вычислить индекс в Hash-таблице
	// -------------------------------
	idx = AliasHashKey( LStrBuf(*(ap->name)), LStrLen(*(ap->name)) );

	// ----------------
	// найти алиас чтобы удалить из списка
	// ----------------
	cur  = aht[idx];
	prev = NULL;
	while(cur) {
//		if ( LStr_Cmp(ap->name, cur->name) ) {
		if (cur == ap) {
			// нашли алиас
			// *** удалить из общего списка
			if (prev)	prev->lnkHashList = cur->lnkHashList;
			else		aht[idx]   	  = cur->lnkHashList;

			// *** удалить запись об алиасе в записи переменной
			ap->var->alias	= NULL;

			return	Alias_FreeMem(ap);
		}
		prev = cur; cur = cur->lnkHashList;
	}
	return noErr;
}

// ######################################################################

MgErr AliasHashTable_DestroyAll(void)
{
	int32			i;
	_AliasRecord	*ap, *alist;
	MgErr			err = noErr, errtmp;

	if (!aht) return noErr;

	// --------------------------
	// Очистить списки переменных
	// --------------------------
	for (i=0; i < ALIAS_HASH_TABLE_SIZE; i++) {
		alist = aht[i];
		while (alist) {
			ap	= alist;
			alist	= alist->lnkHashList;
			if (errtmp = Alias_FreeMem(ap))	err = errtmp;
		}
		aht[i] = NULL;
	}
	return err;
}

// ######################################################################

MgErr AliasHashTable_Clear(void)
{
	int	i;

	for (i=0; i < ALIAS_HASH_TABLE_SIZE; i++) {
		aht[i] = NULL;
	}
	return noErr;
}

