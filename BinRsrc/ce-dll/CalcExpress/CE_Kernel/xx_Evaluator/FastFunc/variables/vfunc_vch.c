/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ############################################################################################
// VarMan functions of the type:	function ( vch )
// ############################################################################################

// ==========================================================
//	vchtest(vch)
// ==========================================================
MgErr v_vchtest(_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,
				int32			arg_qty,
				LVBoolean		*err_f)
{
	_Operand	*arg;
	int32		vcHandle;
	uInt16		errc = 0;
	LVBoolean	errf = LVFALSE;
	MgErr		err  = noErr;

	// ------------------------------------------------------------
	// если вызвана без параметров, то аргумент = текущий VC Handle
	// ------------------------------------------------------------
	if (!arg_qty) {
		if (err = ff_PseudoArg(ip, (float64)ip->VCHandle))
			return err;
		arg_qty = 1;
	}

	arg = OpdStack_EPnt(ip, ip->OpdNum);	// указатель на аргумент, он же результат

	// =======================================
	// Определить VC handle
	// =======================================
	err = ff_GetArgumentValue(ip, arg, OPTYPE_DBL, "Invalid parameter type", err_f);
	if (err || *err_f)	goto out;

	vcHandle = _DblToI32(arg->num);
	if (!vcHandle)
		vcHandle = ip->VCHandle;

	// ==================================
	//	Вызвать VarMan
	// ==================================
//// >>> #CE_3.0.2 2026-06-08
//	err = VMEngine(	VM_CMD_CTX_TEST,	// cmd
//			&vcHandle,		// handle
//			NULL,			// global flag
//			NULL,			// name of variable
//			NULL,			// type & value of variable
//			0,				// writerID		// >>> #CE_3.0.7 2026-07-23
//			NULL, NULL,		// extfunc & extdata
//			&errf, &errc
//		);
//// <<< #CE_3.0.2 2026-06-08
	err = vmCmd_Context_Test(&vcHandle, &errf, &errc);
out:
	arg->type = OPTYPE_DBL;
	arg->num  = __vfunc_err_code(errc);

	// Размер стека не меняем, т.к. функция одного аргумента
	return err;
}

// ==========================================================
//	vcname(vch)
// ==========================================================
MgErr v_vcname(	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,
				int32			arg_qty,
				LVBoolean		*err_f)
{
	_Operand	*arg;
	int32		vcHandle;
	uInt16		errc = 0;
	LVBoolean	errf = LVFALSE;
	MgErr		err  = noErr;

	// ------------------------------------------------------------
	// если вызвана без параметров, то аргумент = текущий VC Handle
	// ------------------------------------------------------------
	if (!arg_qty) {
		if (err = ff_PseudoArg(ip, (float64)ip->VCHandle))
			return err;
		arg_qty = 1;
	}

	arg = OpdStack_EPnt(ip, ip->OpdNum);	// указатель на аргумент, он же результат

	// =======================================
	// Определить VC handle
	// =======================================
	err = ff_GetArgumentValue(ip, arg, OPTYPE_DBL, "Invalid parameter type", err_f);
	if (err || *err_f)	goto out;

	vcHandle = _DblToI32(arg->num);
	if (!vcHandle)
		vcHandle = ip->VCHandle;

	// ==================================
	//	Вызвать VarMan
	// ==================================
	err = vmCmd_vcNames_GetName(
			&vcHandle,						// handle
			arg->str,						// name
			&errf, &errc );
/*
// >>> #CE_3.0.2 2026-06-08
	err = VMEngine(	VM_CMD_VCNAME_GETNAME,	// cmd
			&vcHandle,						// handle
			NULL,							// global flag
			arg->str,						// name
			NULL,							// type & value of variable
			0,								// writerID		// >>> #CE_3.0.7 2026-07-23
			NULL, NULL,						// extfunc & extdata
			&errf, &errc
		);
// <<< #CE_3.0.2 2026-06-08
//	err = VarMan(	VM_CMD_VCNAME_GETNAME,	// cmd
//			&vcHandle,		// handle
//			NULL,			// global flag
//			arg->str,		// name
//			NULL,			// type & value of variable
//			&errf, &errc
//		);
*/
out:
	arg->type = OPTYPE_STR;
	arg->num  = __vfunc_err_code(errc);	// просто так

	// Размер стека не меняем, т.к. функция одного аргумента
	return err;
}

