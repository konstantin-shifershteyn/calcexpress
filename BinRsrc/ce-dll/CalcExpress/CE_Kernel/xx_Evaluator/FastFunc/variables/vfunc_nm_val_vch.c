/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ############################################################################################
// VarMan functions of the type:	function ( varname, value, vch )
// ############################################################################################

// ### helper function to get arguments
MgErr	_vfunc_nam_val_vch(
				_CE_Instance	*ip,
				 int32			arg_qty,	//кол-во аргументов
				 _Operand		**var,
				 _Operand		**val,
				 _Operand		**result,
				 int32			*VCHandle,
				 LVBoolean		*err_f)
{
	MgErr		err = noErr;
	_Operand	*arg;
	int32		idx;

	*val = NULL;
	*VCHandle = 0;

	idx = ip->OpdNum - arg_qty + 1;	// индекс 1-го аргумента
	arg = OpdStack_EPnt(ip, idx);	// указатель на 1-й аргумент, он же результат

	*var = *result = arg;

	// =======================================
	// Определить имя переменной (1-й операнд)
	// =======================================
	err = ff_GetArgumentValue(ip, arg, OPTYPE_STR, ErrMsg_bad_Var_Name, err_f);
	if (err || *err_f)	goto out;

	// ==================================
	// Определить значение 2-го операнда
	// ==================================
	if (arg_qty < 2)
		goto out;
	arg = OpdStack_EPnt(ip, ++idx);		// указатель на 2-й аргумент
	if (arg->type == OPTYPE_IDENT) {
		err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
		if (err || *err_f)	goto out;
	}
	*val = arg;

	// ==================================
	// Определить VC handle (3-й операнд)
	// ==================================
	if (arg_qty < 3)
		goto out;
	arg = OpdStack_EPnt(ip, ++idx);		// указатель на 3-й аргумент
	err = ff_GetArgumentValue(ip, arg, OPTYPE_DBL, ErrMsg_bad_VCHandle3, err_f);
	if (err || *err_f)	goto out;
	*VCHandle = _DblToI32(arg->num);

out:
	// ===  СТЕК ОПЕРАНДОВ. КОЛ-ВО ЭЛЕМЕНТОВ
	ip->OpdNum -= arg_qty-1;

	if (!(*VCHandle))
		*VCHandle = ip->VCHandle;
	return err;
}

// ==============================================================
//   Функция создает переменную
// ==============================================================
MgErr __var_define(	_CE_Instance	*ip,
					LStrHandle		name,
					int32			*VCHandle,
					_Operand		*value,
					_Operand		*result)
{
	LVBoolean	glob;
	uInt16		errc;
	LVBoolean	errf = LVFALSE;
	MgErr		err;

	glob	= (*VCHandle == -1);

	err = vmCmd_Var_CreateEx(
			*VCHandle,						// handle
			&glob,							// global flag
			name,							// name of variable
			(_VarInfo*)value,				// type & value of variable
			ip->writerID,					// writerID							// >>> #CE_3.0.7 2026-07-23
			NULL,							// wrStats (_VM_VI_WriteStats*)		// >>> #CE_3.1.0-r6 2026-09-20
			NULL, NULL,						// extfunc & extdata
			&errf, &errc					// err_f & err_c
		);
/*	
// >>> #CE_3.0.2 2026-06-08
	err =	VMEngine( VM_CMD_VAR_CREATE,	// cmd
			VCHandle,						// handle
			&glob,							// global flag
			name,							// name of variable
			(_VarInfo*)value,				// type & value of variable
			ip->writerID,					// writerID		// >>> #CE_3.0.7 2026-07-23
			NULL, NULL,						// extfunc & extdata
			&errf, &errc					// err_f & err_c
		);
// <<< #CE_3.0.2 2026-06-08
// ------
//	err =	VarMan(	VM_CMD_VAR_CREATE,		// cmd
//			VCHandle,						// handle
//			&glob,							// global flag
//			name,							// name of variable
//			(_VarInfo*)value,				// type & value of variable
//			&errf, &errc					// err_f & err_c
//		);
*/
	result->num		= __vfunc_err_code(errc);
	result->type	= OPTYPE_DBL;
	return err;
}

// ========================================================================================

MgErr var_template(	_CE_Instance	*ip,
					int32			arg_qty,	//кол-во аргументов
					uInt32			type,
					LVBoolean		*err_f)
{
	_Operand	*vname, *value;
	_Operand	*res;		// Указатель на результат
	_Operand	defval;
	int32		VCHandle;
	int32		size;
	MgErr		err;

	// Определить параметры аргументов
	// ===============================
	err = _vfunc_nam_val_vch( ip, arg_qty, &vname, &value, &res, &VCHandle, err_f );
	if (err || *err_f)
		goto out;

	res->type = OPTYPE_DBL;

	// ==============================
	// Создание значения по умолчанию
	// ==============================
	if (!value) {
		value = &defval;

		value->type	= type;
		value->num	= 0.0;
		value->str	= NULL;
		switch(type) {
			case OPTYPE_CDB :	size = 2 * sizeof(float64);
								break;
			case OPTYPE_A1DBL :
			case OPTYPE_A1CDB :	// empty 1D array
			case OPTYPE_A1STR :	size = sizeof(int32);
								break;
			case OPTYPE_A2DBL :
			case OPTYPE_A2CDB :	// empty 2D array
			case OPTYPE_A2STR :	size = 2 * sizeof(int32);
								break;
			case OPTYPE_DBL :
			case OPTYPE_STR :	// empty string
			default :			size = 0;
								break;
		}
		value->str = (LStrHandle)DSNewHClr(size + sizeof(LStr));
		if (!(value->str)) {
			err = mFullErr;
			goto out;
		}
		LStrLen( *(value->str) ) = size;
	}
	// Проверка типа
	// =============
	if (value->type != type) {
		err = CE_Error( ip, "%s", (LStrHandle)ErrMsg_bad_init_value, NULL );
		*err_f = LVTRUE;
		goto out;
	}
	// Создание переменной
	// ===================
	err = __var_define( ip, vname->str, &VCHandle, value, res );

out:
	if (value == &defval)
		if (value->str)
			DSDisposeHandle( value->str );
	return err;
}
// ==============================================================
#define VarDef( _FName, _VarType )		\
MgErr	_FName(	_CE_Instance	*ip,	\
		_FastFuncInfo	*fInfo,			\
		int32		arg_qty,			\
		LVBoolean	*err_f)				\
{										\
return   var_template( ip, arg_qty, _VarType, err_f);		\
}										\
// ==============================================================

	VarDef(	v_dbl	, OPTYPE_DBL	);
	VarDef(	v_cdb	, OPTYPE_CDB	);
	VarDef(	v_str	, OPTYPE_STR	);

	VarDef(	v_a1dbl	, OPTYPE_A1DBL	);
	VarDef(	v_a1cdb	, OPTYPE_A1CDB	);
	VarDef(	v_a1str	, OPTYPE_A1STR	);

	VarDef(	v_a2dbl	, OPTYPE_A2DBL	);
	VarDef(	v_a2cdb	, OPTYPE_A2CDB	);
	VarDef(	v_a2str	, OPTYPE_A2STR	);


// =================================================================================
//	vset(name, val [,vch] )
// =================================================================================

MgErr v_vset(	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,				
				int32			arg_qty,	//кол-во аргументов
				LVBoolean		*err_f)
{
	_Operand	*vname, *value;
	_Operand	*res;		// Указатель на результат
	int32		VCHandle;
	LVBoolean	glob;
	LVBoolean	errf;
	uInt16		errc = 0;
	MgErr		err;

	// Определить параметры аргументов
	// ===============================
	err = _vfunc_nam_val_vch( ip, arg_qty, &vname, &value, &res, &VCHandle, err_f );
	if (err || *err_f)
		goto out;

	// Установить значение переменной
	// ==============================
	errf	= LVFALSE;
	glob	= (VCHandle == -1);

	err = vmCmd_Var_SetValueEx(
			VCHandle,					// handle
			&glob,						// global flag
			vname->str,					// name of variable
			(_VarInfo*)value,			// type & value of variable
			ip->writerID,				// writerID		// >>> #CE_3.0.7 2026-07-23
			NULL,						// wrStats		// >>> #CE_3.1.0-r6 2026-09-18
			NULL, NULL,					// extfunc & extdata
			&errf, &errc				// err_f & err_c
		);
out:
	res->num	= __vfunc_err_code(errc);
	res->type	= OPTYPE_DBL;
	return err;
}

#undef VarDef
