/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// formal parameter 'number' different from declaration
#pragma warning (disable : 4028)

// in LabVIEW 6.1 and greater StrNCmp and StrNCaseCmp have a little different prototypes:
// int32  StrNCmp(ConstCStr, ConstCStr, int32);
// int32  StrNCaseCmp(ConstCStr, ConstCStr, int32);

typedef	int32 LStrCompareFunc(CStr, CStr, int32);

LStrCompareFunc	StrNCmp;
LStrCompareFunc	StrNCaseCmp;

// formal parameter 'number' different from declaration
#pragma warning (default : 4028)

// ----------------

MgErr __str_compare( _gen_operand *in, int32 qty, _gen_operand *out, LStrCompareFunc *_func_);


// ======================================
//	scmp(str1, str2)
// ======================================
MgErr	_s_scmp(_gen_operand *opdArr, int32 qty, _gen_operand *out)
{
	return __str_compare(opdArr, qty, out, StrNCmp);
}
// ======================================
//	sccmp(str1, str2)
// ======================================
MgErr	_s_sccmp(_gen_operand *opdArr, int32 qty, _gen_operand *out)
{
	return __str_compare(opdArr, qty, out, StrNCaseCmp);
}
	
// -------------------------------------------------------------------------------------

// =================================================
// General function. Lexically compares strings
// =================================================
MgErr	__str_compare(
			_gen_operand	*opdArr,
			int32			qty,
			_gen_operand	*out,
			LStrCompareFunc	*_func_)
{
	_gen_operand	*in1, *in2;
	int32			i, size;
	int32			len1, len2, flg;
	float64			*dst;
	LStr			**src1, **src2;
	Bool32			twoArr1D;
	MgErr			err = noErr;

	in1 = opdArr;
	in2 = opdArr + 1;

	if ((_operand_get_BaseType(in1->ResType) != OPTYPE_STR) || 
		(_operand_get_BaseType(in2->ResType) != OPTYPE_STR) ) 
	{
		out->erMsg = "Inavalid parameter type "
						"(string or an array of strings expected)";
		goto out;
	}
	// ---------------------------------------------------------------
	// Проверить совместимость типов (str-str, str-ArrXD, Arr1D-Arr1D)
	// ---------------------------------------------------------------
	twoArr1D = FALSE;
	if (in1->nDims || in2->nDims) {
		if (!(in1->nDims))		goto ok;
		if (!(in2->nDims))		goto ok;
		if ((in1->nDims == 1) && (in2->nDims == 1)) {
			twoArr1D = TRUE;	goto ok;
		}
		out->erMsg = "Incompatible parameter types";
		goto out;
	}
ok:	// --------------------------------
	// подготовить структуру результата
	// --------------------------------
	out->nDims	= __max(in1->nDims, in2->nDims);
	out->ResType	= _operand_get_TargetType(OPTYPE_DBL, out->nDims);
	if (twoArr1D) {
		out->size1 = __min(in1->size1, in2->size1);
		out->size2 = 1;
	}
	else {	out->size1 = __max(in1->size1, in2->size1);
		out->size2 = __max(in1->size2, in2->size2);
	}
	if (out->nDims) {
		size = out->size1 * out->size2;
		out->array	= ce_DSNewPtr(size * sizeof(float64));
		out->alloc	= (out->array != NULL);
		if (!(out->alloc)) { err = mFullErr; goto out; }
	}
	else	{
		size = 1;
		out->array = &(out->scalar);
	}

	// =====================
	// 	ВЫПОЛНЕНИЕ
	// =====================
	src1 = in1->array;
	src2 = in2->array;
	dst  = out->array;
	for(i=0; i<size; i++) {
		len1 = LStrLen(*src1);
		len2 = LStrLen(*src2);
		flg  = _func_( LStrBuf(*src1), LStrBuf(*src2), __min(len1,len2));
		if (!flg) {
			if (len1 < len2) flg = -1;
			if (len1 > len2) flg =  1;
		}
		else {
			if (flg < 0)	flg = -1;
			if (flg > 0)	flg =  1;
		}
		*dst++ = (float64)flg;
		if (in1->nDims)	src1++;
		if (in2->nDims)	src2++;
	}
out:	return err;
}
