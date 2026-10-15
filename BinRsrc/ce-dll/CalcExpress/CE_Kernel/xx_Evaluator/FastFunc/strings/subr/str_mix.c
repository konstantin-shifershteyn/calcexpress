/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

MgErr	_f_mix_strings(	_str_operand*, _str_operand*, _LStr_operand*, int32);


// ============================
MgErr	f_smix(	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,		//указатель на описание ф-ии
				int32			arg_qty,	//кол-во аргументов
				LVBoolean		*err_f)
{
	MgErr		err = noErr;
	_Operand	*arg, *res;		// Указатель на обрабытываемую структуру
	int32		i;			// кол-во аргументов в вызове
	_str_operand	*itm, in1, in2;
	_LStr_operand	out;
	int32		AsColumn = FALSE;	// 1D-2D : 1D is column
	// -------------

	in1.alloc = in2.alloc = out.alloc = FALSE;
	in1.array = in2.array = NULL;
	out.array = NULL;
	out.erMsg = NULL;

	res = &((*(ip->OpdStk))->data[ip->OpdNum-arg_qty+1]);	// указатель на результат

	// *****  Определить значения аргументов
	// -------------------------------------
	for(i=0, itm=&in1; i<=1; i++, itm=&in2) {
		arg = res + i;
		if (arg->type == OPTYPE_IDENT) {
			err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
			if (err || *err_f) goto out;
		}
		if (_operand_get_BaseType(arg->type) != OPTYPE_STR) {
			err = CE_Error(ip, "Invalid parameter type "
					"(string or an array of strings expected)",NULL,NULL);
			*err_f = LVTRUE;
			goto out;
		}
		if (err = _prepare_strings((_OperandBase*)arg, (_str_operand*)itm))	goto out;
	}
	if (arg_qty == 3) {
		arg = res+2;
		if (arg->type == OPTYPE_IDENT) {
			err = GetVariable( ip, arg, (_OperandBase*)arg, err_f); if (err || *err_f) goto out;
		}
		if (arg->type != OPTYPE_DBL) {
			err = CE_Error(ip, "Invalid type of the 3rd parameter",NULL,NULL);
			*err_f = LVTRUE;
			goto out;
		}
		AsColumn = _DblToI32(arg->num);
	}

	// *****  Подготовить структуру результата
	// ---------------------------------------
	if (AsColumn)	err = _prepare_LStr_result_1Dc	(&in1, &in2, &out);
	else		err = _prepare_LStr_result	(&in1, &in2, &out);
	if (err) goto out;

	// *****  Выполнить слияние строк
	// ------------------------------
	if (err = _f_mix_strings(&in1, &in2, &out, AsColumn))
		goto out;

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
	if (in1.alloc)	DSDisposePtr(in1.array);
	if (in2.alloc)	DSDisposePtr(in2.array);

	return err;
}

// ==================     SUBROUTINES     ========================


// =============================================
// собирает две строки в одну результирующую
// =============================================
//#line 40000
MgErr	_LStr_Cat2StrToStr(LStrHandle *out, LStr *s1, LStr *s2)
{
	int32	len, len1, len2;
	char	*p;
	MgErr	err = noErr;

	len1 = LStrLen(s1);	len2 = LStrLen(s2);	len = len1+len2;

	if (err = LStr_ResizeOrCreate(out, len))
		return err;

	p = LStrBuf(**out);
	_MoveBlock(LStrBuf(s1), p,	len1);
	_MoveBlock(LStrBuf(s2), p+len1,	len2);

	return err;
}

// =======================================================
// выполняет поэлементную конкатенацию
// =======================================================
//#line 50000
MgErr _f_mix_strings(	_str_operand	*in1,
			_str_operand	*in2,
			_LStr_operand	*out,
			int32		AsColumn )
{
	int32		i, row, col;
	LStr		**p1, **p2;
	LStrHandle	*pnt;
	MgErr		err = noErr;

	pnt	= out->array;
	p1	= in1->array;
	p2	= in2->array;

	// определить комбинацию операндов
	// -------------------------------
	switch(in1->nDims + in2->nDims) {
		case 4:			goto L_2D2D;
		case 3:	if (AsColumn)	goto L_2D1Dc;
			else		goto L_2D1D;
	}

	// ==========================================
	// |	String   - String
	// |	String   - 1D or 2D Array
	// |	1D Array - 1D Array
	// ==========================================
	for (i = out->size1 * out->size2; i; i--) {
		if (err = _LStr_Cat2StrToStr(pnt, *p1, *p2))	return err;
		if (in1->nDims)	p1++;
		if (in2->nDims)	p2++;
		pnt++;
	}
	return err;


	// ==========================================
L_2D1D:	// |	1D Array - 2D Array (1D is row)
L_2D2D:	// |	2D Array - 2D Array
	// ==========================================
	for (row=0; row < out->size1; row++) {
		switch(in1->nDims) {
			case 1:	p1 = in1->array;			break;
			case 2: p1 = in1->array + row * in1->size2;	break;
		}
		switch(in2->nDims) {
			case 1:	p2 = in2->array;			break;
			case 2: p2 = in2->array + row * in2->size2;	break;
		}
		for (col=0; col < out->size2; col++) {
			if (err = _LStr_Cat2StrToStr(pnt, *p1, *p2))	return err;
			if (in1->nDims)	p1++;
			if (in2->nDims)	p2++;
			pnt++;
		}
	}
	return err;

	// ==========================================
	// |	1D Array - 2D Array (1D is column)
	// ==========================================
L_2D1Dc:
	for (row=0; row < out->size1; row++) {
		switch(in1->nDims) {
			case 1:	p1 = in1->array + row;			break;
			case 2: p1 = in1->array + row * in1->size2;	break;
		}
		switch(in2->nDims) {
			case 1:	p2 = in2->array + row;			break;
			case 2: p2 = in2->array + row * in2->size2;	break;
		}
		for (col=0; col < out->size2; col++) {
			if (err = _LStr_Cat2StrToStr(pnt, *p1, *p2))	return err;
			if (in1->nDims == 2)	p1++;
			if (in2->nDims == 2)	p2++;
			pnt++;
		}
	}
	return err;
}
