/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#define	EXPARRAY_BLOCK_SIZE		240		//ExpArr_BlkSize		240

// =============================================================
// >>> #CE_3.0.4 2026-06-22
__inline void ExAr_RemoveOneRecord(_CE_Instance *ip, int32 *idx)
{
	_Operand	*opd;

	if (*idx) {
//DbgPrintf("ExAr_RemoveOneRecord: EXEC\n");
		*idx -= 1;
		opd = &(ip->expArrDat[*idx].opd);
		if ( opd->str ) {
			DSDisposeHandle( opd->str );
			opd->str = NULL;
		}
		_IndexInfo_Destroy(opd);
	}
//else DbgPrintf("ExAr_RemoveOneRecord: Do nothing\n");
}

// =============================================================
// >>> #CE_3.0.4 2026-06-22
__inline void ExAr_FreeAllocatedMemory(_CE_Instance *ip, int32 idx)
{
	_Operand	*opd;

	while( idx < ip->expArrLen ) {
		opd = &(ip->expArrDat[idx].opd);
		if ( opd->str ) {
			DSDisposeHandle( opd->str );
			opd->str = NULL;
		}
		_IndexInfo_Destroy(opd);
		idx++;
	}
}
// <<< #CE_3.0.4 2026-06-22

// =============================================================
// >>> #CE_3.0.4 2026-06-22
__inline MgErr ExAr_Resize(_CE_Instance *ip, int32 newsize )
{
	int32	i;
	int32	size = ip->expArrLen; 
	MgErr	err = noErr;

	if (newsize == size)
		return noErr;

//DbgPrintf(__FUNCTION__"(%s): %d\n", newsize < size ? "compress" : " expand ", newsize);

	// *** ≈сли массив уменьшаетс€, то освободить не нужные LStrHandle, etc.
	if (newsize < size) {
		ExAr_FreeAllocatedMemory(ip, newsize);		// >>> #CE_3.0.4 2026-06-22
	}
	// >>> #CE_3.0.4 2026-07-05
	if ( err = ce_CreateOrResizeAlignedArray((UHandle*)&(ip->expArr), newsize, sizeof(_ExpresElement), sizeof(float64), offsetof(_ExpresArray,data)))
		goto out;
//	(*(ip->expArr))->dimSize = newsize;					// no need, ce_CreateOrResizeAlignedArray sets DimSize
	ip->expArrDat	= &((*(ip->expArr))->data[0]);		// >>> #CE_3.0.4 2026-06-22
	ip->expArrLen	= newsize;							// >>> #CE_3.0.4 2026-06-22

	// *** ≈сли массив расшир€етс€, то обнулить новые LStrHandles & Pointers
	// (no need in fact because ce_CreateOrResizeAlignedArray sets all new data to zero)
	if (newsize > size) {
//DbgPrintf(__FUNCTION__"(): Extend ExpArray\n");		// >>> #CE_3.1.0 2026-08-02
		for (i = size; i < newsize; i++) {
//if (ip->expArrDat[i].opd.str)	DbgPrintf(__FUNCTION__"(): x->str != NULL\n");		// >>> #CE_3.1.0 2026-08-02
			ip->expArrDat[i].opd.str	= NULL;
			ip->expArrDat[i].opd.iip	= NULL;
			ip->expArrDat[i].opd.hdl	= NULL;		// >>> #CE_3.0 2024-12-16
			ip->expArrDat[i].opd.num	= 0.0;
			ip->expArrDat[i].opd.vpnt	= NULL;		// >>> #CE_3.0.2 2026-06-19
			ip->expArrDat[i].opd.deref	= FALSE;	// >>> #CE_3.0.5 2025-07-14
		}
	}
out:
	return err;
}
// <<< #CE_3.0.4 2026-06-22

// =============================================================
// >>> #CE_3.0.4 2026-06-22
MgErr ExAr_PutOperand(_CE_Instance *ip, _Operand *opd, int32 *idx)
{
	_Operand	*otmp;
	MgErr		err = noErr;

	if (*idx >= ip->expArrLen) {
		if (err = ExAr_Resize(ip, ip->expArrLen + EXPARRAY_BLOCK_SIZE))
			goto out;
	}
	ip->expArrDat[*idx].IsOps = FALSE;
	ip->expArrDat[*idx].ops.category = CAT_OPERAND;	// ! 19:17 22-03-02 !

	otmp  = &(ip->expArrDat[*idx].opd);

	*otmp = *opd;			// скопировать всю структуру!!! INCLUDING STRING HANDLES !!!
	otmp->str = NULL;		// set NULL in oder to create a new string (if necessary)

//	if (err = ce_CopyOrCreate_OperandString(&(otmp->str), opd->str, otmp->type))		// >>> #CE_3.0.4 2026-07-06
//		goto out;
// >>> #CE_3.1.0 2026-08-02 : don't copy string for DBLs
// ALSO review calling functions, maybe we can simplify them ?!
	if ((otmp->type != OPTYPE_DBL) && (otmp->type != OPTYPE_IDXRANGE))	{	// >>> #CE_3.1.0 2026-08-02
		if (err = ce_CopyOrCreate_OperandString(&(otmp->str), opd->str, otmp->type))		// >>> #CE_3.0.4 2026-07-06
			goto out;
	}
// <<< #CE_3.1.0 2026-08-02

	// *****************************************************************
	// ƒл€ идентификаторов в _Operand.hdl заносим handle строки имени
	// Ёто используетс€ при выполнении, дл€ исключени€ копировани€ строк
	// при копировании структуры _Operand (в частности - стек операндов)
	// *****************************************************************
	if (otmp->type == OPTYPE_IDENT) {
// >>> #CE_3.0 2024-12-16
		otmp->num	= 0.0;			// 2025-03-12
		otmp->hdl	= otmp->str;
		otmp->deref	= FALSE;		// >>> #CE_3.0.5 2025-07-14
// <<< #CE_3.0 2024-12-16
	}

	// ¬ыделить пам€ть под информацию об индексах и инициализировать
	err = _IndexInfo_Create(otmp);

	otmp->vpnt = NULL;	// >>> #CE_3.0.2 2026-06-19
	*idx += 1;
out:
	return err;
}
// <<< #CE_3.0.4 2026-06-22

// =============================================================
// >>> #CE_3.0.4 2026-06-22
__inline MgErr ExAr_PutOperation(_CE_Instance *ip, _Operation *ops, int32 *idx)
{
	_Operation	*otmp;
	MgErr		err = noErr;

	if (*idx >= ip->expArrLen) {
		if (err = ExAr_Resize(ip, ip->expArrLen + EXPARRAY_BLOCK_SIZE))
			goto out;
	}
	ip->expArrDat[*idx].IsOps = TRUE;
	ip->expArrDat[*idx].opd.str = NULL;		// 2002-01-29: можно присваивать NULL и не выдел€ть пам€ть под строку !!!
	ip->expArrDat[*idx].opd.iip = NULL;		// pointer to indexes info

	otmp = &(ip->expArrDat[*idx].ops);
	*otmp = *ops;
	*idx += 1;
out:
	return err;
}
// <<< #CE_3.0.4 2026-06-22

// =============================================================
// Ќулевой размер - это важно !!!
MgErr ExAr_Create( _CE_Instance *ip)
{
	MgErr	err;

	// ### create array
	// >>> #CE_3.0.4 2026-07-05
	ip->expArr = NULL;
	if ( err = ce_CreateOrResizeAlignedArray((UHandle*)&(ip->expArr), 0, sizeof(_ExpresElement), sizeof(float64), offsetof(_ExpresArray,data)))
		return err;
//	(*(ip->expArr))->dimSize = 0;						// no need, ce_CreateOrResizeAlignedArray sets DimSize
	ip->expArrDat	= &((*(ip->expArr))->data[0]);		// >>> #CE_3.0.4 2026-06-22
	ip->expArrLen	= (*(ip->expArr))->dimSize;			// >>> #CE_3.0.4 2026-06-22
	return  noErr;
}


// =============================================================

MgErr ExAr_Destroy(	_CE_Instance *ip)
{
	ExAr_FreeAllocatedMemory(ip, 0);	// >>> #CE_3.0.4 2026-06-22
	DSDisposeHandle(ip->expArr);
	ip->expArr		= NULL;
	ip->expArrDat	= NULL;			// >>> #CE_3.0.4 2026-06-22
	ip->expArrLen	= 0;			// >>> #CE_3.0.4 2026-06-22

	return noErr;
}


// =============================================================
/*
// >>> #CE_3.0 2025-03-22
int32 ExAr_DebugDisplay( _ExpArrayHandle ea )
{
	int32			i, size;
	_ExpresElement	*ee;
	LStrHandle		str, msg;
	MgErr			err = noErr;

	err = LStr_Create(&str, 0);
	err = LStr_Create(&msg, 0);

	size = (*ea)->dimSize;
	LStrPrintf(msg, "Exp Array size = %d\n", size);

	for( i=0; i < size; i++ ) {
		ee = &((*ea)->data[i]);
		LStrPrintf(str, "%02d: ", i);
		LStr_Cat(msg, str);

		if (ee->IsOps) {
			// operator
			LStrPrintf(str, "operator: cat=%d, code=%d\n", ee->ops.category, ee->ops.code);
			LStr_Cat(msg, str);
		}
		else {
			LStrPrintf(str, "#operand: dtype=%d, strHdl=%u, ", ee->opd.type, (size_t)ee->opd.str);
			LStr_Cat(msg, str);
			// operand
			switch (ee->opd.type) {
				case OPTYPE_DBL:	
						LStrPrintf(str, "num=%.3f\n", ee->opd.num);
						break;
				case OPTYPE_STR:
				case OPTYPE_IDENT:
						LStrPrintf(str, "str=\'%H\'\n", ee->opd.str);
						break;
				default:
						LStrPrintf(str, "---\n");
						break;
			}
			LStr_Cat(msg, str);
		}
	}
	*(LStrBuf(*msg) + LStrLen(*msg) - 1) = 0;
	MessageBox( NULL, (LPCSTR)LStrBuf(*msg) , "Expression Array" , MB_OK|MB_TASKMODAL|MB_ICONEXCLAMATION|MB_SETFOREGROUND|MB_TOPMOST);

	LStr_Destroy(str);
	LStr_Destroy(msg);
	return size;
}
// <<< #CE_3.0 2025-03-22
*/
