/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#define	CEINF_VERSION		0
#define	CEINF_FUNC_LIST		'func'
#define	CEINF_AUTHOR		26011966

// =============================

const	char *	ceinf_EmptyStr	= "";
const	char *	ceinf_ErrMsg	= "Invalid parameter datatype (scalar expected)";

const	char *	ceinf_product	= PRODUCT_FULL_NAME ", ver. " PRODUCT_VERSION " " PRODUCT_RELEASE;
const	char *	ceinf_copyright	= COPYRIGHT;
const	char *	ceinf_maketime	= "Engine DLL built time: " __DATE__ ", " __TIME__;


// ======================================================
//	_ce_inf( [Q] )	, where Q is DBL
// ======================================================
MgErr _ce_inf (
		_CE_Instance	*ip,
		_FastFuncInfo	*fInfo,
		int32			arg_qty,	// number of parameters in call of the FastFunc function
		LVBoolean		*err_f)
{
	_Operand	*arg;
	int32		i, Question;
	const char	*msg = ceinf_EmptyStr;
	MgErr		err = noErr;

	// if the FastFunc is called w/o arguments
	// ==========================
	if (!arg_qty) {
		if (err = ff_PseudoArg(ip, CEINF_VERSION))
			return err;
		arg_qty = 1;
	}

	// get the argument value
	// ==============================
	arg = OpdStack_EPnt(ip, ip->OpdNum);		// pointer to the argument

	if (arg->type == OPTYPE_IDENT) {
		err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
		if (err || *err_f)	goto out;
	}

	if (arg->type != OPTYPE_DBL) {
		err = CE_Error( ip, "%s", (LStrHandle)ceinf_ErrMsg, NULL );
		*err_f = LVTRUE;
		goto out;
	}

	Question = _DblToI32(arg->num);

	switch(Question) {
		default:
		case CEINF_VERSION :
			LStrPrintf(arg->str, "%s\n%s\n%s", ceinf_product, ceinf_copyright, ceinf_maketime);
			goto out2;
		case CEINF_FUNC_LIST	:
			LStrPrintf(arg->str, "Total FastFuncs: %d\n", FuncListLen);
			for (i=0; i<FuncListLen; i++)
				LStrPrintf(arg->str, "%H%.8s\n", arg->str, FuncList[i].name);
			goto out2;
		case CEINF_AUTHOR:
			LStrPrintf(arg->str, "%s: %02d-%02d-%d", AUTHOR_NAME, 26,1,1966);
			goto out2;

	}

out:
	// ### Comments on the Stack of Operands
	// =========================================
	// we don't change size of the stack for this FastFunc function because it has a single argument (max).
	// in event the FastFunc is called w/o arguments, a pseudo-argument with a default value was pushed to
	// the stack in the begining of this FastFunc handler.
	// *** Typical way to change size of Stack of Operands in FastFunc handlers:
	// ***		ip->OpdNum -= arg_qty-1;

	LStrPrintf(arg->str, "%s", msg);
out2:
	arg->type = OPTYPE_STR;	// the result's datatype
	arg->num  = 0;			// the result's numeric value
	return err;
}

#undef	CEINF_VERSION
#undef	CEINF_FUNC_LIST
#undef	CEINF_AUTHOR


// ======================================================
//	_fflist()
// ======================================================
int32	_fflist_cmp_name(_FastFuncInfo **v1, _FastFuncInfo **v2)
{
	return StrNCaseCmp((*v1)->name, (*v2)->name, FF_MaxNameLen /*8*/);
}
// ---------------
MgErr	_fflist(
		_CE_Instance	*ip,
		_FastFuncInfo	*fInfo,
		int32			arg_qty,	// number of parameters in call of the FastFunc function
		LVBoolean		*err_f)
{
	MgErr			err = noErr;
	_Operand		*res;
	_FastFuncInfo	**ffi = NULL;
	int32			i, size;

	// ### the FastFunc function has no arguments
	if (!arg_qty) {
		if (err = ff_PseudoArg(ip, 0))
			return err;
		arg_qty = 1;
	}
//	res = &((*(ip->OpdStk))->data[ip->OpdNum]);	// указатель на результат
	res = OpdStack_EPnt(ip, ip->OpdNum);		// pointer to the argument. It is also the result

	// ### create array of indexes
	size = FuncListLen-1;
	ffi  = ce_DSNewPtr(size * sizeof(_FastFuncInfo*));
	if (!ffi) {
		err = mFullErr; goto out;
	}
	for (i=0; i<size; i++) ffi[i] = &FuncList[i];

	// ### sort array
	QSort((void*)ffi, size, sizeof(_FastFuncInfo*), (CompareProcPtr)_fflist_cmp_name);

	// ### format the resulting string
	LStrPrintf(res->str, "%s, FastFunc List\n#\tname    \targuments\n", ceinf_product);
	for(i=0; i<size; i++) {
		if (ffi[i]->minArg == ffi[i]->maxArg) {
			err = LStrPrintf(res->str, "%H%-3d:\t%-8.8s\t[%d]\n",
				res->str, i+1, ffi[i]->name, ffi[i]->minArg);
			if (err) goto out;
		}
		else {
			err = LStrPrintf(res->str, "%H%-3d:\t%-8.8s\t[%d..%d]\n",
				res->str, i+1, ffi[i]->name, ffi[i]->minArg, ffi[i]->maxArg);
			if (err) goto out;
		}
	}

out:
	if (ffi)	DSDisposePtr(ffi);

	res->type = OPTYPE_STR;
	res->num  = 0;
	return err;
}

// ======================================================
//	_nclist()
// ======================================================
#include <stdio.h>
MgErr	_nclist(
		_CE_Instance	*ip,
		_FastFuncInfo	*fInfo,
		int32			arg_qty,	// number of parameters in call of the FastFunc function
		LVBoolean		*err_f)
{
	MgErr			err = noErr;
	_Operand		*res;
	const _td_named_dbl_const *ncp;
	int32			i, size;
	char			buf[256];
	float64			val;
	int32			mode;
	size_t			off;

	// ### функция без аргументов !!!
	if (!arg_qty) {
		if (err = ff_PseudoArg(ip, 0))
			return err;
		arg_qty = 1;
	}
//	res = &((*(ip->OpdStk))->data[ip->OpdNum]);	// указатель на результат
	res = OpdStack_EPnt(ip, ip->OpdNum);		// pointer to the argument. It is also the result
	mode = _DblToI32(res->num);
	LStr_SetSize(res->str, 0);

	// ### Сохранить строку
	size = __namedDblConst_size();
	for(i=0; i<size; i++) {
		if ( !(ncp = __namedDblConst_pnt(i)))
			break;

		val = (ncp->valPnt != NULL ? *(ncp->valPnt) : ncp->val);

		if (mode & 0x1)
				sprintf_s(buf, sizeof(buf), "%-16s", ncp->name);
		else	sprintf_s(buf, sizeof(buf), "%s="  , ncp->name);
		off = strlen(buf);

		if (_finite(val)) {
			sprintf_s(buf+off, sizeof(buf)-off, "%.17G", val);
		}
		else {
			if		(_isnan(val))	strcpy_s(buf+off, sizeof(buf)-off, "NaN");
			else if (val > 0)		strcpy_s(buf+off, sizeof(buf)-off, "Inf");
			else					strcpy_s(buf+off, sizeof(buf)-off, "-Inf");
		}

		if (mode >= 2) {
			off = strlen(buf);
			while(off < 48) *(buf+off++) = ' ';
			*(buf+off) = 0;
			sprintf_s(buf+off, sizeof(buf)-off, "/* %s */", ncp->dscr);
		}

		if (err = LStrPrintf(res->str, "%H%s\n", res->str, buf))
			goto out;
	}
out:
	res->type = OPTYPE_STR;
	res->num  = 0;
	return err;
}
