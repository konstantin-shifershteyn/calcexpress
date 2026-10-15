/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__LStrConverts_C__
#define	__LStrConverts_C__

// ==================================================================================
// 	initializes the result structure ('out' parameter)
// ==================================================================================
MgErr	_prepare_LStr_result( _str_operand *in1, _str_operand *in2, _LStr_operand *out)
{
	// a stub for functions with a single argument
	// ===========================================
	if (!in1) {
		out->ResType	= in2->ResType;
		out->nDims		= in2->nDims;
		out->size1		= in2->size1;
		out->size2		= in2->size2;
		goto L1;
	}
	if (!in2) {
		out->ResType	= in1->ResType;
		out->nDims		= in1->nDims;
		out->size1		= in1->size1;
		out->size2		= in1->size2;
		goto L1;
	}

	// –азмерность и тип - по большему операнду
	// ========================================
	out->ResType	= __max( in1->ResType, in2->ResType );
	out->nDims		= __max( in1->nDims,   in2->nDims );

	// ========================
	// Both operands are scalar
	// ========================
	if (!out->nDims) {
		out->size1 = out->size2 = 1;
		goto L1;
	}
	// ==========================
	// No one operand is 2D Array
	// ==========================
	if (out->nDims == 1) {
		out->size2 = 1;
		out->size1 = 0x7FFFFFFF;
		if (in1->nDims)	out->size1 = __min( out->size1, in1->size1 );
		if (in2->nDims)	out->size1 = __min( out->size1, in2->size1 );
		goto L1;
	}
	// ================================
	// At least one operand is 2D Array
	// ================================
	// !!! 1D Array is row !!!
	// ================================
	// ***  Number of rows  ***
	out->size1 = 0x7FFFFFFF;
	if (in1->nDims == 2)	out->size1 = __min( out->size1, in1->size1 );
	if (in2->nDims == 2)	out->size1 = __min( out->size1, in2->size1 );

	// ***  Number of columns ***
	out->size2 = 0x7FFFFFFF;
	switch(in1->nDims) {
		case 2:	out->size2 = __min( out->size2, in1->size2 ); break;
		case 1: out->size2 = __min( out->size2, in1->size1 ); break;
	}
	switch(in2->nDims) {
		case 2:	out->size2 = __min( out->size2, in2->size2 ); break;
		case 1: out->size2 = __min( out->size2, in2->size1 ); break;
	}
L1:
	// allocate buffer (array of LStrHandle)
	// -------------------------------------
	out->array = (LStrHandle*)ce_DSNewPClr( sizeof(LStrHandle) * (out->size1 * out->size2));
	out->alloc = (out->array != NULL);
	if (!out->alloc)
		return mFullErr;
	return noErr;
}

// ==================================================================================
//	prepares 1D and 2D result when 1D should be a column !!!
// ==================================================================================
MgErr	_prepare_LStr_result_1Dc( _str_operand *in1, _str_operand *in2, _LStr_operand *out)
{
	if (in1->nDims + in2->nDims != 3)
		return _prepare_LStr_result(in1, in2, out);

	out->ResType	= OPTYPE_A2STR;
	out->nDims		= 2;
	out->size1		= __min(in1->size1, in2->size1);	// rows
	out->size2		= __max(in1->size2, in2->size2);	// columns

	// allocate buffer (array of LStrHandle)
	// -------------------------------------
	out->array = (LStrHandle*)ce_DSNewPClr( sizeof(LStrHandle) * (out->size1 * out->size2));
	out->alloc = (out->array != NULL);
	if (!out->alloc)
		return mFullErr;
	return noErr;
}

// ==================================================================================
// 	disposes the result structure
// ==================================================================================
MgErr	_dispose_LStr_result( _LStr_operand *out)
{
	int32		i;
	LStrHandle	*lshp;
	MgErr		err = noErr;

	if (!out->alloc) goto out;

	lshp = out->array;
	for (i = out->size1 * out->size2; i > 0; i--) {
		if (*lshp)
			DSDisposeHandle(*lshp);
		lshp++;
	}
out:
	if (out->alloc)	DSDisposePtr(out->array);
	out->alloc = FALSE;
	out->array = NULL;
	return err;
}

// ==================================================================================
// 	—охран€ет результат (array of LStrHandle)
// ==================================================================================
MgErr _save_LStrings(_LStr_operand *data, LStrHandle *sh)
{
	int32		i, len, tlen, size, tsize, nDims;
	int32		*ipo;
	char		*cpo, *cpi;
	MgErr		err = noErr;

	switch (data->ResType) {
		case OPTYPE_STR :	nDims = 0;	break;
		case OPTYPE_A1STR :	nDims = 1;	break;
		case OPTYPE_A2STR :	nDims = 2;	break;
		default :	return noErr;
	}

	size = data->size1 * data->size2;	// кол-во элементов (строк) в массиве

	// подсчитать суммарную длину всех строк
	for (i = tlen = 0; i < size; i++) {
		if (data->array[i])		// защита
			tlen += LStrLen(*(data->array[i]));
	}

	// обща€ длина результирующей строки
	if (nDims) {
		tsize  = nDims * sizeof(int32);		// пам€ть под размерности массива
		tsize += size  * sizeof(int32);		// + пам€ть под длины строк
		tsize += tlen;				// + сумма длин всех строк
	}
	else	tsize = tlen;

	// »зменить размер результирующей строки
	if (err = LStr_ResizeOrCreate(sh, tsize))
		return err;

	ipo = (int32*)LStrBuf(**sh);		// указатель на данные

	// сохранить размерности
	switch(nDims) {
		case 0 :	if (data->array[0])
						_MoveBlock( LStrBuf(*(data->array[0])), ipo, tsize);
					goto finish;
		case 1 :	*ipo++ = data->size1;
					break;
		case 2 :	*ipo++ = data->size1;
					*ipo++ = data->size2;
					break;
	}

	// сохранить строки
	for (i=0; i<size; i++) {
		if (data->array[i]) {		// защита
			len	= LStrLen(*(data->array[i]));
			*ipo++	= len;
			cpo	= (char*)ipo;
			cpi	= LStrBuf(*(data->array[i]));
			while(len--)	*cpo++ = *cpi++;
			ipo	= (int32*)cpo;
		}
		else {				// пуста€ строка
			*ipo++ = 0;
		}
	}
finish:
	return err;
}


#endif
