/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ==================================
//	abort()
// ==================================
MgErr f_abort(
		_CE_Instance	*ip,
		_FastFuncInfo	*fInfo,
		int32			arg_qty,	// кол-во аргументов
		LVBoolean		*err_f)
{
	ip->CancelTag = TRUE;		// ABORT !!!

	// ***  функция без аргумента
	if (arg_qty) {			// просто заглушка
		ip->OpdNum -= arg_qty;	// убрать все(!!!) из стека
	}
	// *** результат
	return	ff_PseudoArg(ip, 0);
}

// ==================================
//	errno([code])
// ==================================
MgErr f_errno(
		_CE_Instance	*ip,
		_FastFuncInfo	*fInfo,
		int32			arg_qty,	// кол-во аргументов
		LVBoolean		*err_f)
{
	_Operand	*arg;		// Указатель на операнд, он-же будет результатом
	MgErr		err = noErr;

	// если функция без аргумента, то возвращаем errno
	// ==========================
	if (!arg_qty) {
		return	ff_PseudoArg(ip, (float64)rtErrCode_Get(ip->VCHandle));
	}
//	arg = &((*(ip->OpdStk))->data[ip->OpdNum]);	// указатель на аргумент
	arg = OpdStack_EPnt(ip, ip->OpdNum);		// >>> #CE_3.0.7 2026-07-25 : указатель на аргумент

	// если функция с аргументом, то устанавливаем errno
	// и возвращаем предыдущее значение
	// =================================================
	err = ff_GetArgumentValue(ip, arg, OPTYPE_DBL, "Invalid parameter datatype", err_f);
	if (err || *err_f)
		goto out;
	arg->num  = (float64)rtErrCode_Set(ip->VCHandle, _DblToI32(arg->num));

out:
	// КОММЕНТАРИИ ОТНОСИТЕЛЬНО СТЕКА ОПЕРАНДОВ.
	// =========================================
	// Размер стека (количество элементов) не изменяем, т.к. это функция одного
	// аргумента (если вызывается без аргумента, то принудительно помещаем аргумент
	// со значением по умолчанию в стек)
	// *** обычно размер стека изменяется следующим образом:
	// ***		ip->OpdNum -= arg_qty-1;

	arg->type = OPTYPE_DBL;
	return err;
}

// ==================================
//	vwrid([writerID])
// ==================================
MgErr f_vwrid(
		_CE_Instance	*ip,
		_FastFuncInfo	*fInfo,
		int32			arg_qty,	// кол-во аргументов
		LVBoolean		*err_f)
{
	uInt32		oldWriterID = ip->writerID;
	_Operand	*arg;				// Указатель на операнд, он-же будет результатом
	MgErr		err = noErr;

	// если функция без аргумента, то возвращаем writerID
	// ==========================
	if (!arg_qty) {
		return	ff_PseudoArg(ip, (float64)oldWriterID);
	}
//	arg = &((*(ip->OpdStk))->data[ip->OpdNum]);	// указатель на аргумент
	arg = OpdStack_EPnt(ip, ip->OpdNum);		// >>> #CE_3.0.7 2026-07-25 : указатель на аргумент

	// если функция с аргументом, то устанавливаем writerID
	// и возвращаем предыдущее значение
	// =================================================
	err = ff_GetArgumentValue(ip, arg, OPTYPE_DBL, "Invalid parameter datatype", err_f);
	if (err || *err_f)
		goto out;
	ip->writerID = (uInt32)(int64)arg->num;
	arg->num  = (float64)oldWriterID;

out:
	// КОММЕНТАРИИ ОТНОСИТЕЛЬНО СТЕКА ОПЕРАНДОВ.
	// =========================================
	// Размер стека (количество элементов) не изменяем, т.к. это функция одного
	// аргумента (если вызывается без аргумента, то принудительно помещаем аргумент
	// со значением по умолчанию в стек)
	// *** обычно размер стека изменяется следующим образом:
	// ***		ip->OpdNum -= arg_qty-1;

	arg->type = OPTYPE_DBL;
	return err;
}

// ==================================
//	lvmem([units])
// ==================================
MgErr f_lvmem(
		_CE_Instance	*ip,
		_FastFuncInfo	*fInfo,
		int32			arg_qty,	// кол-во аргументов
		LVBoolean		*err_f)
{
	_Operand	*arg;				// Указатель на операнд, он-же будет результатом
	MgErr		err = noErr;

	if (!arg_qty) {
		if (err = ff_PseudoArg(ip, 1.0))
			return err;
	}
	arg = OpdStack_EPnt(ip, ip->OpdNum);		// указатель на аргумент

	err = ff_GetArgumentValue(ip, arg, OPTYPE_DBL, "Invalid parameter datatype", err_f);
	if (err || *err_f)
		goto out;
	arg->num  = ce_getLabviewMemoryUse((uInt32)arg->num);
out:
	// Размер стека (количество элементов) не изменяем, т.к. это функция одного аргумента
	arg->type = OPTYPE_DBL;
	return err;
}

