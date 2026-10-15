/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ======================================================================================
//	General function to copy/clone a variable
// ======================================================================================
MgErr _vfunc_copy_s(_CE_Instance	*ip,
					_FastFuncInfo	*fInfo,
					int32			arg_qty,
					Bool32			enaCreate,
					LVBoolean		*err_f)
{
	_VM_2Var_Info	vars;
	_Operand		*res, *arg;
	int32			i, idx;
	uInt16			errc = 0;
	LVBoolean		errf = LVFALSE;
	MgErr			err = noErr;

#ifdef	NARGS
	#error "NARG is already defined"
#endif
#define	NARGS	4
static  uInt32	argTypes[NARGS] = { OPTYPE_STR, OPTYPE_STR, OPTYPE_DBL, OPTYPE_DBL };
static  char	*errMsgs[NARGS] = { "string", "string", "DBL scalar", "DBL scalar" };

	vars.src_vch  = vars.dst_vch  = 0;

	idx = ip->OpdNum - arg_qty + 1;		// индекс 1-го аргумента (он же результат)
	res = OpdStack_EPnt(ip, idx);		// указатель на результат

	for (i=0; i < __min(NARGS, arg_qty); i++) {
		arg = OpdStack_EPnt(ip, idx++);

		if (arg->type == OPTYPE_IDENT) {
			err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
			if (err || *err_f)	goto out;
		}
		if (arg->type != argTypes[i]) {
			err = CE_Error( ip, "Invalid datatype of parameter %d (%s expected)",
						(LStrHandle)(size_t)(i+1), (LStrHandle)errMsgs[i] );
			*err_f = LVTRUE;
			goto out;
		}
		switch(i) {
			case 0 : vars.src_name = arg->str;		break;
			case 1 : vars.dst_name = arg->str;		break;
			case 2 : vars.src_vch  = _DblToI32(arg->num);	break;
			case 3 : vars.dst_vch  = _DblToI32(arg->num);	break;
		}
	}
	if (!vars.src_vch)	vars.src_vch = ip->VCHandle;
	if (!vars.dst_vch)	vars.dst_vch = ip->VCHandle;
	vars.enaCreate = enaCreate;

	err = vmCmd_Var_Copy(&vars, ip->writerID, &errf, &errc);

out:
	// ===  СТЕК ОПЕРАНДОВ. КОЛ-ВО ЭЛЕМЕНТОВ
	ip->OpdNum -= arg_qty-1;

	res->num  = __vfunc_err_code(errc);
	res->type = OPTYPE_DBL;
	return err;

#undef	NARGS
}

// ===============================================================================
//	vcopy( nameSrc, nameDst [,vchSrc [,vchDst]] )
// ===============================================================================
MgErr	v_vcopy(
			_CE_Instance	*ip,
			_FastFuncInfo	*fInfo,
			int32			arg_qty,
			LVBoolean		*err_f)
{
	return _vfunc_copy_s(ip, fInfo, arg_qty, FALSE, err_f);
}
// ===============================================================================
//	vclone( nameSrc, nameDst [,vchSrc [,vchDst]] )
// ===============================================================================
MgErr	v_vclone(
			_CE_Instance	*ip,
			_FastFuncInfo	*fInfo,
			int32			arg_qty,
			LVBoolean		*err_f)
{
	return _vfunc_copy_s(ip, fInfo, arg_qty, TRUE, err_f);
}
