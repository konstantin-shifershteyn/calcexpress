/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <VMEngine_Constants.h>

int32	__fastcall	ctbl_find_free 	(void);
MgErr	__fastcall	ctbl_free_el	(int32 idx);

// ################################################


// =============================================
// Вычисляет новый уникальный handle по индексу.
// Корректирует шаблон curHandle
// =============================================
__inline int32 ctbl_new_handle(int32 index)
{
	curHandle = (curHandle + CTBL_HANDLE_INC) & CTBL_HANDLE_MASK;
	if (!curHandle)
		curHandle = CTBL_HANDLE_INC;	// *** NEW : It prevents zero handle that can occur if index == 0
	return (curHandle | (index & CTBL_INDEX_MASK) );
}
// =============================
// проверяет свободен-ли элемент
// =============================
__forceinline  Bool32 ctbl_is_el_free(int32 idx)
{
	return (ctblPnt->data[idx].key == -1);		// >>> #CE_3.0.2 2026-06-18
}
// =============================================
// Определяет индекс элемента таблицы контекстов
// по handle. Выполняет проверку и возвращает -1
// при ошибочном handle.
// =============================================
__inline int32 ctbl_find_index(int32 handle)
{
	int32	index;
	if (handle > 0) {
// >>> #CE_3.0.2 2026-06-18
		index = CTBL_INDEX(handle);
		if (index < ctblPnt->dimSize)
			if (ctblPnt->data[index].key == handle)
				return index;
// <<< #CE_3.0.2 2026-06-18
	}
	return -1;
}
// =============================================
// Определяет индекс элемента таблицы контекстов
// по handle. Если handle неверный, то возвращает
// индекс глобльного контекста если разрешено,
// или  -1, в противном случае
// =============================================
__forceinline int32	__fastcall Get_Context_Index( int32 handle, Bool32 enaGlobal, uInt16 *err_c)
{
	int32	idx;
	if ( (idx = ctbl_find_index(handle)) < 0) {
		if (enaGlobal) {
			idx = CTBL_GBLIDX;
		}
		else {
			*err_c = VM_ERR_CTX_INVHANDLE;
			return -1;	//__ER__;
		}
	}
	return idx;
}

// =============================================
// Определяет индекс элемента таблицы контекстов
// по handle. Выполняет проверку и возвращает -1
// при ошибочном handle. Если ЯВНО указано, то
// возвращает индекс глобального контекста.
// =============================================
__inline int32	Get_Context_Index_SG( int32 handle, LVBoolean global, uInt16 *err_c)
{
	int32	idx;
	if (global)
		return	CTBL_GBLIDX;
	if ((idx = ctbl_find_index(handle)) < 0) {
		*err_c = VM_ERR_CTX_INVHANDLE;
		return -1;	//__ER__;
	}
	return idx;
}



// ###########################################################

// --------------------------------------------
// Функция заполняет элемент в таблице контекстов
// --------------------------------------------
MgErr ctbl_fill_el(int32 idx, int32 handle, LStrHandle Creator, int32 lifetime)
{
	_CTableEl	*cxp;
	 MgErr		err = noErr;

	cxp = &((*ctbl)->data[idx]);

	// ***	init
	cxp->vcName		= NULL;
	cxp->Creator	= NULL;

	// ***	store VC Handle
	cxp->key		= handle;

	// ***	store context's life time
	cxp->LifeTime	= lifetime;

	// *** allocate LStr for name of creator
	if (Creator)	err = LStr_CopyOrCreate(&(cxp->Creator), Creator);
	else			err = LStr_Create(&(cxp->Creator), 0);
	if (err)	goto out;

	// *** allocate LStr for context name
	if (idx == CTBL_GBLIDX)	err = LStr_CreateFromMem(&(cxp->vcName), "Global Context", -1);
	else					err = LStr_Create(&(cxp->vcName), 0);
	if (err)	goto out;

	// *** Clear miscellaneous fields
	for (idx=0; idx < CNTX_MISC_FIELDS; idx++)
		cxp->misc[idx] = 0;

	// *** Create HashTable for variables
//	if ( !(cxp->hktbl = HashTable_create()) )
//		err = mFullErr;
// >>> #CE_3.1.0 2026-08-18
	memset(&(cxp->hktbl), 0, sizeof(cxp->hktbl));
// <<< #CE_3.1.0 2026-08-18


out:
	if (err)
		ctbl_free_el(idx);
	return err;
}

// ----------------------------
// Функция освобождает контекст
// ----------------------------
MgErr __fastcall ctbl_free_el(int32 idx)
{
	_CTableEl	*cxp;

	cxp = &((*ctbl)->data[idx]);

	if (cxp->Creator) {	LStr_Destroy(cxp->Creator); 	cxp->Creator	= NULL; }
	if (cxp->vcName)  {	LStr_Destroy(cxp->vcName);		cxp->vcName		= NULL; }
// >>> #CE_3.1.0 2026-08-18 : disabled
//	if (cxp->hktbl)	  {	HashTable_destroy(cxp->hktbl);	cxp->hktbl		= NULL; }
	HashTable_destroy(&(cxp->hktbl[0]));
// <<< #CE_3.1.0 2026-08-18

	cxp->key = -1;

	return noErr;
}

// ###########################################################

// ------------------------------------------------
// Функция создает или расширяет таблицу контекстов
// ------------------------------------------------
int32 ctbl_create(void)
{
	int32	i, newSize, first;

	if (!ctbl) {
		// Создать таблицу
		// ---------------
//DbgPrintf(__FUNCTION__ "(): create table\n");

//		if (!(ctbl = (_CTable**)DSNewHClr(sizeof(_CTable) * CTBL_BLKSIZE) )) {
//			first = -1;
//			goto out;
//		}
//		(*ctbl)->dimSize = CTBL_BLKSIZE;	// размер массива
//		first = i = 0;
// >>> #CE_3.1.0 2026-08-15
		if (ce_CreateOrResizeAlignedArray((UHandle*)&ctbl, CTBL_BLKSIZE, sizeof(_CTableEl), sizeof(void*), offsetof(_CTable,data)) ) {
			first = -1;
			goto out;
		}
//		(*ctbl)->dimSize = CTBL_BLKSIZE;	// размер массива
		first = i = 0;
// <<< #CE_3.1.0 2026-08-15
	}
	else {
		// Расширить таблицу
		// -----------------
		i = (*ctbl)->dimSize;
		newSize = i + CTBL_BLKSIZE;
//DbgPrintf(__FUNCTION__ "(): extend table from %d to %d\n", i, newSize);

		// ***** test ctbl overflow *****
		if (newSize > (CTBL_MAX_INDEX+1)) {
			first = -1;
			goto out;
		}
		// ***** extend ctbl *****
// >>> #CE_3.1.0 2026-08-15
//		if (ce_1DArrayResize((UHandle*)&ctbl, newSize, sizeof(_CTableEl)) ) {
		if (ce_CreateOrResizeAlignedArray((UHandle*)&ctbl, newSize, sizeof(_CTableEl), sizeof(void*), offsetof(_CTable,data)) ) {
// <<< #CE_3.1.0 2026-08-15
			first = -2;
			goto out;
		}
		first = i;
	}
	ctblPnt = *ctbl;	// >>> #CE_3.0.2 2026-06-18

	// отметить новые как свободные
	// ----------------------------
	for ( ; i < ctblPnt->dimSize; i++) {
		ctblPnt->data[i].key		= -1;
// >>> #CE_3.1.0 2026-08-18 : disabled
//		ctblPnt->data[i].hktbl		= NULL;
		memset(&(ctblPnt->data[i].hktbl), 0, sizeof(ctblPnt->data[i].hktbl));
// <<< #CE_3.1.0 2026-08-18
		ctblPnt->data[i].Creator	= NULL;
		ctblPnt->data[i].vcName		= NULL;
	}
out:
	return first;
}

// -------------------------------------------------
// Функция разрушения таблицы и списков переменных
// -------------------------------------------------
MgErr ContextTable_Destroy(void)
{
	int32	i;
	MgErr	err = noErr;

	if (!ctbl)	goto out;

//DbgPrintf(__FUNCTION__"(): ctblSize = %u\n", (*ctbl)->dimSize);

	// Destroy contexts
	// ------------------
	for (i=0; i < (*ctbl)->dimSize; i++) {
		ctbl_free_el(i);
	}
	// Destroy Context Table
	// ------------------
	err = DSDisposeHandle(ctbl);

out:
	ctbl = NULL;
	ctblPnt = NULL;
	return err;
}

// -------------------------------------------
// Функция инициализации таблицы и всех данных
// -------------------------------------------
MgErr ContextTable_Init(void)
{
	MgErr	err = noErr;

	// разрушаем все данные, если они есть
	// -----------------------------------
	ContextTable_Destroy();

	// создаем таблицу
	// ---------------
	if (ctbl_create() >= 0 ) {
		// ***	создаем контекст глобальных переменных
		err = ctbl_fill_el(CTBL_GBLIDX, 0, NULL, 2);
	}
	else {
		err = mFullErr;
	}
	return err;
}

// =============================================
// Возвращает индекс первого свободного элемента
// в таблице контекстов
// =============================================
int32 __fastcall ctbl_find_free(void)
{
	int32	i, size;

	size = (*ctbl)->dimSize;
	for (i=0; i < size; i++) {
		if (ctbl_is_el_free(i))
			return i;
	}
	return -1;
}


// =========================================================
// =========   Routines to operate on VC Names   ===========
// =========================================================

// -----------------------------------------
// returns TRUE if ERROR
Bool32	vcName_test_syntax(LStrHandle name)
{
	return	test_freestyle_name_syntax(LStrBuf(*name),LStrLen(*name));
}

// -----------------------------------------
int32	vcName_find_name(LStrHandle name)
{
	int32	idx, size;

	size = (*ctbl)->dimSize;
	for (idx=0; idx < size; idx++) {
		if ( !ctbl_is_el_free(idx))
			if (LStr_Cmp(name, (*ctbl)->data[idx].vcName))
				return idx;
	}
	return -1;
}

// -----------------------------------------
void	vcName_reset_names(void)
{
	int32	idx, size;

	size = (*ctbl)->dimSize;
	for (idx=0; idx < size; idx++) {
		if ( !ctbl_is_el_free(idx))
			LStr_Clear((*ctbl)->data[idx].vcName);
	}
}
