/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


typedef	MgErr LStrConvertFunc(LStr*, int32*);

// Function bodies are in this file below
LStrConvertFunc	LStr_ToUpper;
LStrConvertFunc	LStr_ToLower;
LStrConvertFunc	LStr_Reverse;
LStrConvertFunc	LStr_Rotate;
LStrConvertFunc	LStr_Shift;
LStrConvertFunc	LStr_PickLine;
LStrConvertFunc	LStr_SubString;
LStrConvertFunc	LStr_RemoveBlanks;

MgErr __str_convert( _gen_operand *in, int32 qty, _gen_operand *out, LStrConvertFunc *_func_);


// ================================
// 	slwr(string)
// ================================
MgErr	_s_slwr(_gen_operand *opdArr, int32 qty, _gen_operand *out )
{
	return __str_convert(opdArr, qty, out, LStr_ToLower);
}
// ================================
// 	supr(string)
// ================================
MgErr	_s_supr(_gen_operand *opdArr, int32 qty, _gen_operand *out )
{
	return __str_convert(opdArr, qty, out, LStr_ToUpper);
}
// ================================
// 	sreverse(string)
// ================================
MgErr	_s_sreverse(_gen_operand *opdArr, int32 qty, _gen_operand *out )
{
	return __str_convert(opdArr, qty, out, LStr_Reverse);
}
// ================================
// 	srotate(string, n)
// ================================
MgErr	_s_srotate(_gen_operand *opdArr, int32 qty, _gen_operand *out )
{
	return __str_convert(opdArr, qty, out, LStr_Rotate);
}
// ================================
// 	sshift(string, n)
// ================================
MgErr	_s_sshift(_gen_operand *opdArr, int32 qty, _gen_operand *out )
{
	return __str_convert(opdArr, qty, out, LStr_Shift);
}
// ================================
// 	sline(string, n)
// ================================
MgErr	_s_sline(_gen_operand *in, int32 qty, _gen_operand *out )
{
	MgErr	err;
	if (!(err = __str_convert(in, qty, out, LStr_PickLine))) {
		out->alloc = in->alloc;
		in->alloc  = FALSE;
	}
	return err;
}
// ================================
// 	substr(string, idx, len)
// ================================
MgErr	_s_substr(_gen_operand *in, int32 qty, _gen_operand *out )
{
	MgErr	err;
	if (!(err = __str_convert(in, qty, out, LStr_SubString))) {
		out->alloc = in->alloc;
		in->alloc  = FALSE;
	}
	return err;
}
// ================================
// 	srefine(string)
// ================================
MgErr	_s_srefine(_gen_operand *in, int32 qty, _gen_operand *out )
{
	MgErr	err;
	if (!(err = __str_convert(in, qty, out, LStr_RemoveBlanks))) {
		out->alloc = in->alloc;
		in->alloc  = FALSE;
	}
	return err;
}

// -------------------------------------------------------------------------------------

// =================================================
// General function. Converts data inside strings
// =================================================
MgErr	__str_convert(	_gen_operand	*in,
			int32		qty,
			_gen_operand	*out,
			LStrConvertFunc	*_func_ )
{
	int32		i;
	int32		pars[2] = {0,0} ;
	LStr		**src;
	MgErr		err = noErr;

	if (_operand_get_BaseType(in->ResType) != OPTYPE_STR) {
		out->erMsg = "Inavalid type of the 1st parameter "
				"(string or an array of strings expected)";
		goto out;
	}
	if (qty >= 2) {
		if ((in+1)->ResType != OPTYPE_DBL) {
			out->erMsg = "Inavalid type of the 2nd parameter (scalar expected)";	goto out;
		}
		pars[0] = _DblToI32((in+1)->scalar);
	}
	if (qty >= 3) {
		if ((in+2)->ResType != OPTYPE_DBL) {
			out->erMsg = "Inavalid type of the 3rd parameter (scalar expected)";	goto out;
		}
		pars[1] = _DblToI32((in+2)->scalar);
	}
	out->nDims	= in->nDims;
	out->size1	= in->size1;
	out->size2	= in->size2;
	out->ResType	= in->ResType;
	out->array	= in->array;
	out->alloc	= FALSE;

	src  = in->array;
	for (i = in->size1 * in->size2; i > 0; i--) {
		if (err = _func_(*src++, pars))
			goto out;
	}
out:	return err;
}


// -------------------------------------------------------------------------------------


// ================================
MgErr	LStr_ToLower(LStr *lsp, int32 *stub)
{
	char	*pnt;
	int32	len;
	for (pnt=LStrBuf(lsp), len=LStrLen(lsp); len; len--, pnt++)
		*pnt = (char)ToLower(*pnt);
	return noErr;
}
// ================================
MgErr	LStr_ToUpper(LStr *lsp, int32 *stub)
{
	char	*pnt;
	int32	len;
	for (pnt=LStrBuf(lsp), len=LStrLen(lsp); len; len--, pnt++)
		*pnt = (char)ToUpper(*pnt);
	return noErr;
}
// ================================
MgErr	LStr_Reverse(LStr *lsp, int32 *stub)
{
	ReverseBlock(LStrBuf(lsp), LStrLen(lsp), 1);
	return noErr;
}
// ================================
MgErr	LStr_Rotate(LStr *lsp, int32 *n)
{
	return RotateBlock(LStrBuf(lsp), LStrLen(lsp), 1, *n);
}
// ================================
MgErr	LStr_Shift(LStr *lsp, int32 *n)
{
	int32	len, nsh = *n;
	char	*pnt;
	MgErr	err;

	pnt = LStrBuf(lsp);
	len = LStrLen(lsp);

	if (err = RotateBlock(pnt, len, 1, nsh))
		return err;

	// clear strings in the tail
	if (abs(nsh) >= len)
		nsh = len;
	if (nsh < 0) {
		nsh = abs(nsh);	pnt += len - nsh;
	}
	while (nsh-- )
		*pnt++ = ' ';

	return noErr;
}
// ================================
MgErr	LStr_SubString(LStr *lsp, int32 *pars)
{
	int32	idx, sLen, aLen;
	char	*pnt;

	idx  = *pars++;
	aLen = *pars;

	pnt  = LStrBuf(lsp);
	sLen = LStrLen(lsp);


	if (idx >= sLen)	goto empty;
	if (idx < 0) {
		aLen += idx; idx = 0;
	}
	if (aLen <= 0)		goto empty;
	if ((idx + aLen) > sLen)
		aLen = sLen - idx;
	if (aLen <= 0)		goto empty;

	_MoveBlock(pnt+idx, pnt, aLen);
	LStrLen(lsp) = aLen;
	return noErr;
empty:
	LStrLen(lsp) = 0;
	return noErr;
}
// ================================
MgErr	LStr_RemoveBlanks(LStr *lsp, int32 *stub)
{
	char	*pnt, *p1, *p2;
	int32	i, sLen, aLen;

	pnt  = LStrBuf(lsp);
	sLen = LStrLen(lsp);
	aLen = sLen;

	for(i=0, p1=pnt; i < sLen; i++)
		if (*p1 > ' ')	break;
		else	{	p1++; aLen--; }

	for(p2=pnt+sLen-1; i < sLen; i++)
		if (*p2 > ' ')	break;
		else	{	p2--; aLen--; }

	if (aLen < 0)	aLen = 0;

	if ((p1 != pnt) && aLen)
		_MoveBlock(p1, pnt, aLen);

	LStrLen(lsp) = aLen;
	return noErr;
}
// ================================
MgErr	LStr_PickLine(LStr *lsp, int32 *line)
{
	char	*pnt, *p1, *p2;
	int32	i, sLen, aLen, cLine;

	pnt 	= LStrBuf(lsp);
	sLen	= LStrLen(lsp);
	aLen	= 0;
	cLine	= 0;
	i	= 0;
	p1	= pnt;
	if (!(*line)) goto pick;

	for( ; i< sLen; i++) {
		// looking for a line
		if (*p1++ == '\n') {
			if (++cLine == *line) {
				i += 1;
pick:				// looking for the end of line
				for(p2=p1; i < sLen; i++) {
					aLen++;
					if (*p2++ == '\n')
						break;
				}
				break;
			}
		}	
	}
	if ((p1 != pnt) && aLen)
		_MoveBlock(p1, pnt, aLen);

	LStrLen(lsp) = aLen;
	return noErr;
}
