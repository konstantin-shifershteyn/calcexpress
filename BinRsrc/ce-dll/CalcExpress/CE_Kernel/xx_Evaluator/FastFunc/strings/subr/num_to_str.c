/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ============================
MgErr	f_num_to_str(
			_CE_Instance	*ip,
			_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
			int32			arg_qty,	//кол-во аргументов
			LVBoolean		*err_f)
{
	MgErr		err = noErr;
	_Operand	*arg, *res;		// Указатель на обрабытываемую структуру
	int32		i;				// кол-во аргументов в вызове
	_dbl_operand	in;
	_LStr_operand	out;
	Bool32		itComplex = FALSE;
	int32		pars[2] = {-1,-1};
	// -------------

	in.alloc  = out.alloc = FALSE;
	in.array  = NULL;
	out.array = NULL;
	out.erMsg = NULL;

	res = &((*(ip->OpdStk))->data[ip->OpdNum-arg_qty+1]);	// указатель на результат

	// *****  Определить значения аргументов
	// -------------------------------------
	for(i=0; i < arg_qty; i++) {
		arg = res + i;
		if (arg->type == OPTYPE_IDENT) {
			err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
			if (err || *err_f) goto out;
		}
		switch(i) {
		case 0:	if (_prepare_operand((_OperandBase*)arg, &in, &itComplex)) {	*err_f = LVTRUE; break;	}
			if (itComplex) {				*err_f = LVTRUE; break;	}
			break;
		case 1:	if (arg->type != OPTYPE_DBL) {	*err_f = LVTRUE; break;	}
			pars[0] = _DblToI32(arg->num);
			break;
		case 2:	if (arg->type != OPTYPE_DBL) {	*err_f = LVTRUE; break;	}
			pars[1] = _DblToI32(arg->num);
			break;
		}
		if (*err_f) {
			err = CE_Error(ip, "Invalid type of parameter %d", (LStrHandle)(size_t)(i+1),NULL);
			goto out;
		}
	}

	// *****  Подготовить структуру результата
	// ---------------------------------------
	err = _prepare_LStr_result((_str_operand*)&in, NULL, &out);
	out.ResType = _operand_get_TargetType(OPTYPE_STR, out.nDims);
	if (err) goto out;

	// *****  Выполнить
	// ----------------
	err = fInfo->exec_dbl( &in, &out, pars);
	if (err) goto out;


	// *****  Сохранить результат
	// --------------------------
	res->type = out.ResType;
	err = _save_LStrings(&out, &(res->str));

out:
	// *****  Изменить размер стека операндов
	// -----------------------------------
	ip->OpdNum -= arg_qty - 1;

	// *****  Освободить выделенные ресурсы
	// ------------------------------------
	err = _dispose_LStr_result(&out);

	if (out.alloc)	DSDisposePtr(out.array);
	if (in.alloc)	DSDisposePtr(in.array);

	return err;
}

// ==================     Executors     ========================


// =========================================
//	ntosi( num [,radix] )
// =========================================
MgErr	_s_ntosi(	_dbl_operand	*in,
					_LStr_operand	*out,
					int32			*pars )
{
	int32		i, len, radix;
	float64		*nump;
	LStrHandle	*lshp;
	MgErr		err = noErr;
	char		buf[40];

	radix = *pars;
	if ((radix < 2) || (radix > 36)) radix = 10;

	nump = in->array;
	lshp = out->array;

	for (i = out->size1 * out->size2; i > 0; i--) {
		_itoa_s( _DblToI32(*nump), buf, sizeof(buf), radix);
		len = (int32)strlen(buf);
		if (err = LStr_ResizeOrCreate(lshp, len))
			goto out;
		_MoveBlock(buf, LStrBuf(**lshp), len);
		nump++;
		lshp++;
	}
out:
	return err;
}

// =========================================
//	ntosf( num [,precission [,width]] )
// =========================================
MgErr	_s_ntosf(	_dbl_operand	*in,
			_LStr_operand	*out,
			int32		*pars )
{
	int32		i, prec, width;
	float64		*nump;
	LStrHandle	*lshp;
	MgErr		err = noErr;
	char		cfmt[16];

	prec  = *pars++;
	width = *pars;

	if (prec < 0) prec = 6; prec = __min(prec, 24);
	if (width< 0) width= 0; width= __min(width,0xffff);

	if (width)	SPrintf(cfmt, "%%%d.%df", width, prec);
	else		SPrintf(cfmt, "%%.%df", prec);

	nump = in->array;
	lshp = out->array;

	for (i = out->size1 * out->size2; i > 0; i--) {
		// create new handle if *lshp == NULL
		if (err = LStr_ResizeOrCreate(lshp, 0))		goto out;
		if (err = LStrPrintf(*lshp, cfmt, *nump))	goto out;
		nump++;
		lshp++;
	}
out:
	return err;
}

// =========================================
//	ntose( num [,precission [,width]] )
// =========================================
MgErr	_s_ntose(	_dbl_operand	*in,
			_LStr_operand	*out,
			int32		*pars )
{
	int32		i, prec, width;
	float64		*nump;
	LStrHandle	*lshp;
	MgErr		err = noErr;
	char		cfmt[16];

	prec  = *pars++;
	width = *pars;

	if (prec < 0) prec = 6; prec = __min(prec, 24);
	if (width< 0) width= 0; width= __min(width,0xffff);

	if (width)	SPrintf(cfmt, "%%%d.%de", width, prec);
	else		SPrintf(cfmt, "%%.%de", prec);

	nump = in->array;
	lshp = out->array;

	for (i = out->size1 * out->size2; i > 0; i--) {
		// create new handle if *lshp == NULL
		if (err = LStr_ResizeOrCreate(lshp, 0))		goto out;
		if (err = LStrPrintf(*lshp, cfmt, *nump))	goto out;
		nump++;
		lshp++;
	}
out:
	return err;
}
