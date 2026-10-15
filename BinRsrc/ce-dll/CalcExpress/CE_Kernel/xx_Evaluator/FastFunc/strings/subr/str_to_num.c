/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


typedef	float64 LStrToDblFunc(LStr*, int32);

// Function bodies are in this file below
LStrToDblFunc	LStr_SLenToDbl;		// StrLen
LStrToDblFunc	LStr_fStrToDbl;		// get number from float representation
LStrToDblFunc	LStr_iStrToDbl;		// get number from integer representation
MgErr __str_to_num( _gen_operand *in, int32 qty, _gen_operand *out, LStrToDblFunc *_func_);


// ================================
// 	slen(string)
// ================================
MgErr	_s_slen(_gen_operand *opdArr, int32 qty, _gen_operand *out )
{
	return __str_to_num(opdArr, qty, out, LStr_SLenToDbl);
}
// ================================
// 	sfton(string)
// ================================
MgErr	_s_sfton(_gen_operand *opdArr, int32 qty, _gen_operand *out )
{
	return __str_to_num(opdArr, qty, out, LStr_fStrToDbl);
}
// ================================
// 	siton(string [,radix])
// ================================
MgErr	_s_siton(_gen_operand *opdArr, int32 qty, _gen_operand *out )
{
	return __str_to_num(opdArr, qty, out, LStr_iStrToDbl);
}

// -------------------------------------------------------------------------------------

// =================================================
// General function. Converts strings to dbl numbers
// =================================================
MgErr	__str_to_num(
			_gen_operand	*in,
			int32			qty,
			_gen_operand	*out,
			LStrToDblFunc	*_func_ )
{
	int32		i, size, radix=10;
	float64		*dst;
	LStr		**src;
	MgErr		err = noErr;

	if (qty == 2) {
		if ((in+1)->ResType != OPTYPE_DBL) {
			out->erMsg = "Inavalid type of 2nd parameter (scalar expected)";
			goto out;
		}
		radix = _DblToI32((in+1)->scalar);
	}

	if (_operand_get_BaseType(in->ResType) != OPTYPE_STR) {
		out->erMsg = "Inavalid parameter type "
				"(string or an array of strings expected)";
		goto out;
	}

	out->size1	= in->size1;
	out->size2	= in->size2;
	out->nDims	= in->nDims;
	out->ResType	= _operand_get_TargetType(OPTYPE_DBL, out->nDims);
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

	src = in->array;
	dst = out->array;
	for (i=size; i > 0; i--) {
		*dst++ = _func_(*src++, radix);
	}
out:	return err;
}


// -------------------------------------------------------------------------------------

float64	LStr_SLenToDbl(LStr *lsp, int32 stub)
{
	return (float64)LStrLen(lsp);
}

// ================================
float64	LStr_fStrToDbl(LStr *lsp, int32 stub)
{
	char	*pnt, buf[32];
	int32	len;
	double	num;

	pnt = buf;
	*pnt = 0;
	len = LStrLen(lsp);
	if (len >= sizeof(buf)) {
		if (!(pnt = ce_DSNewPtr(len + 1)))
			return 0.0;
	}
	_MoveBlock( LStrBuf(lsp), pnt, len );
	*(pnt + len) = 0;
	num = strtod(pnt, NULL);
	if (pnt != buf)
		DSDisposePtr(pnt);
	return	num;
}
// ================================
float64	LStr_iStrToDbl(LStr *lsp, int32 radix)
{
	char	*pnt, buf[32];
	int32	len;
	double	num;

	if ((radix < 2)||(radix > 36))
		radix = 0;

	pnt = buf;
	*pnt = 0;
	len = LStrLen(lsp);
	if (len >= sizeof(buf)) {
		if (!(pnt = ce_DSNewPtr(len + 1)))
			return 0.0;
	}
	_MoveBlock( LStrBuf(lsp), pnt, len );
	*(pnt + len) = 0;
	num = (float64)(int32)strtoul(pnt, NULL, radix);	// >>> #CE_3.0  2025-07-01 : consider it as "signed 32-bit integer"

	if (pnt != buf)
		DSDisposePtr(pnt);
	return	num;
}
