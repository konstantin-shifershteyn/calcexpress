/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__StrConverts_C__
#define	__StrConverts_C__

// ===========================================================
//	prepares string operands (arrays)
// ===========================================================
MgErr _prepare_strings(_OperandBase *arg, _str_operand *data)
{
	int32	*intp, i, len, size;

	data->size1		= data->size2 = 1;
	data->ResType	= arg->type;		// datatype
	intp			= (int32*)LStrBuf(*(arg->str));

	switch (arg->type) {
		case OPTYPE_STR :
					intp = (int32*)&(LStrLen(*(arg->str)));	// !!! pointer to LStr
					data->size1 = data->size2 = 1;
					data->nDims = 0;
					break;
		case OPTYPE_A1STR :	
					data->size1 = *intp++;
					data->nDims = 1;
					break;
		case OPTYPE_A2STR :	
					data->size1 = *intp++;
					data->size2 = *intp++;
					data->nDims = 2;
					break;
		default :	return noErr;
	}

	size		= data->size1 * data->size2;
	data->array = ce_DSNewPtr( size * sizeof(data->array));
	data->alloc = (data->array != NULL);
	if (!(data->alloc))	return mFullErr;

	for (i=0; i<size; i++) {
		data->array[i] = (LStr*)intp;	// pointer to LStr
		len = *intp++;					// save length and skip length field
		(char*)intp += len;				// skip string
	}

	return noErr;
}

// ==================================================
//	saves string arrays
// ==================================================
MgErr _save_strings(_str_operand *data, LStrHandle *sh)
{
	int32		i, len, tlen, size, tsize, nDims;
	int32		*ipo;
	char		*cpo, *cpi;
	LStrHandle	tmp;
	MgErr		err = noErr;

	switch (data->ResType) {
		case OPTYPE_STR:	nDims = 0;	break;
		case OPTYPE_A1STR :	nDims = 1;	break;
		case OPTYPE_A2STR :	nDims = 2;	break;
		default :	return noErr;
	}

	size = data->size1 * data->size2;	// total number of elements in an array

	// подсчитать суммарную длину всех строк
	for (i=tlen=0; i<size; i++)
		tlen += LStrLen(data->array[i]);

	// общая длина результирующей строки
	if (nDims) {
		tsize  = nDims * sizeof(int32);		// память под размерности массива
		tsize += size * sizeof(int32);		// + память под длины строк
		tsize += tlen;						// + сумма длин всех строк
	}
	else	tsize = tlen;

	// Выделить память под результирующую строку
	if (err = LStr_Create( &tmp, tsize) )
		return err;

	ipo = (int32*)LStrBuf(*tmp);		// указатель на данные

	// сохранить размерности
	switch(nDims) {
		case 0 :	_MoveBlock( LStrBuf(data->array[0]), ipo, tsize);
					goto finish;
		case 1 :	*ipo++ = data->size1;
					break;
		case 2 :	*ipo++ = data->size1;
					*ipo++ = data->size2;
					break;
	}

	// сохранить строки
	for (i=0; i<size; i++) {
		len	= LStrLen(data->array[i]);
		*ipo++	= len;
		cpo	= (char*)ipo;
		cpi	= LStrBuf(data->array[i]);
		while(len--)	*cpo++ = *cpi++;
		ipo	= (int32*)cpo;
	}
finish:
	err = LStr_Copy(*sh, tmp);
	LStr_Destroy(tmp);

// ***  Fragment below works too. I've replaced it with fragment above for safety purpose
//	err = DSDisposeHandle(*sh);
//	*sh = tmp;

	return err;
}

#endif
