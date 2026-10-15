/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ==========================================================
// General handler for array functions with a single argument
// ==========================================================
MgErr f_arr_1a(		_CE_Instance	*ip,
			_FastFuncInfo	*fInfo,
			int32		arg_qty,	// number of arguments
			LVBoolean	*err_f )
{
	MgErr		err = noErr;
	_Operand	*arg, *res;

	_dbl_operand	opnum;
	_str_operand	opstr;
	Bool32		itComplex = FALSE;

	opnum.alloc	= opstr.alloc	= FALSE;
	opnum.erMsg	= opstr.erMsg	= NULL;

	arg = &((*(ip->OpdStk))->data[ip->OpdNum]);	// pointer to argument
	res = arg;

	if (arg->type == OPTYPE_IDENT) {
		err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
		if (err || *err_f)	goto out;
	}

	switch(arg->type) {
		case OPTYPE_A2DBL :
		case OPTYPE_A2CDB :
		case OPTYPE_A1DBL :
		case OPTYPE_A1CDB :
			_prepare_operand((_OperandBase*)arg, &opnum, &itComplex);

			fInfo->exec_dbl(&opnum, NULL);	// execute

			_save_dimensions_info(&opnum, &(res->str));
			res->type = opnum.ResType;
			break;

		case OPTYPE_A2STR :
		case OPTYPE_A1STR :
			if (err = _prepare_strings((_OperandBase*)arg, &opstr))
				goto out;
			fInfo->exec_dbl(NULL, &opstr);	// execute
			if (err = _save_strings(&opstr, &(res->str)))
				goto out;
			res->num  = 0;
			res->type = opstr.ResType;
			break;
		default:
			opnum.erMsg = err_InvArgType_needArr;
			break;
	}
	if (opnum.erMsg) {
		err = CE_Error( ip, "%s", (LStrHandle)opnum.erMsg, NULL );
		*err_f = LVTRUE;
		goto	out;
	}
	if (opstr.erMsg) {
		err = CE_Error( ip, "%s", (LStrHandle)opstr.erMsg, NULL );
		*err_f = LVTRUE;
		goto	out;
	}
out:
	// *** decrease the stack size ***
	ip->OpdNum -= arg_qty - 1;

	if (opstr.alloc)	DSDisposePtr(opstr.array);
	if (opnum.alloc)	DSDisposePtr(opnum.array);
	return noErr;
}


// =======================================================================
// ======  Частные исполнители функций массивов с одним аргументом  ======
// =======================================================================


// ============================
//	reverse(array)
// ============================
MgErr _arr_reverse(_dbl_operand *nop, _str_operand *sop)
{
	int32	elNums, elSize;
	void	*pnt;

	if (!sop && !nop)	return noErr;

	if (sop) {
		pnt	= sop->array;
		elNums	= sop->size1 * sop->size2;
		elSize	= sizeof(LStr*);
	}
	else {
		pnt	= nop->array;
		elNums	= nop->size1 * nop->size2;
		switch (nop->ResType) {
			case OPTYPE_A2DBL :
			case OPTYPE_A1DBL : elSize = sizeof(float64);	break;
			case OPTYPE_A2CDB :
			case OPTYPE_A1CDB : elSize = 2*sizeof(float64);	break;
			default	:	return noErr;
		}
	}
	ReverseBlock(pnt, elNums, elSize);
	return noErr;
}

