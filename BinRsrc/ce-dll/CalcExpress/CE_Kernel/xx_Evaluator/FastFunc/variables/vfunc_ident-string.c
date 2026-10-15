/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =================================================================
//	vnidn( str )
// =================================================================
MgErr v_vnidn(	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,
				int32			arg_qty,	// кол-во аргументов
				LVBoolean		*err_f)
{
	_Operand	*arg;		// Указатель на операнд, он-же будет результатом
	MgErr		err  = noErr;

	arg = OpdStack_EPnt(ip, ip->OpdNum);	// указатель на аргумент, он же результат

	// *** Определить имя идентификатора
	err = ff_GetArgumentValue(ip, arg, OPTYPE_STR, "Invalid parameter datatype (string expected)", err_f);
	if (err || *err_f)	goto out;

	// *** Преобразовать тип: строка в идентификатор
	arg->type	= OPTYPE_IDENT;
	arg->num	= 0.0;		// !!! ОБЯЗАТЕЛЬНО !!!
	arg->hdl	= NULL;
	arg->vpnt	= NULL;		// >>> #CE_3.0.2 2026-06-19
	arg->deref	= FALSE;	// >>> #CE_3.1.0 2026-08-21

	// ***	Добавил 13:08 19-03-03
	// Выполняем на случай если параметр - результат индексации, например
	//	funcs = vector("abs","sqrt","pow2");
	//	return vnidn(funcs[1])(9);
	// ***
	//_IndexInfo_FreeValues(arg);			// 13:04 19-03-03
	_IndexInfo_ResetIndexation(arg);		// >>> #CE_3.0.4 2026-07-05

out:
	// Размер стека не меняем, т.к. функция одного аргумента
	return err;
}

// =================================================================
//	vnstr( ident )
// =================================================================
MgErr v_vnstr(	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,
				int32			arg_qty,	// кол-во аргументов
				LVBoolean		*err_f)
{
	_Operand	*arg;			// Указатель на операнд, он-же будет результатом
	MgErr		err  = noErr;

	arg = OpdStack_EPnt(ip, ip->OpdNum);	// указатель на аргумент, он же результат

	// *** Определить имя идентификатора
	err = ff_GetArgumentValue(ip, arg, OPTYPE_IDENT, "Invalid parameter datatype (identifier expected)", err_f);
	if (err || *err_f)	goto out;

	// *** Преобразовать тип: идентификатор в строку
	if (IdentInNum(arg)) {
		// если LStrHandle идентификатора находится в поле num
		// то надо скопировать реальную строку
		err = LStr_Copy(arg->str, getIdentString(arg));
	}
	arg->type	= OPTYPE_STR;
	arg->num	= 0.0;
	arg->hdl	= NULL;
	arg->vpnt	= NULL;		// >>> #CE_3.0.2 2026-08-21
	arg->deref	= TRUE;		// >>> #CE_3.1.0 2026-08-21
out:
	// Размер стека не меняем, т.к. функция одного аргумента
	return err;
}
