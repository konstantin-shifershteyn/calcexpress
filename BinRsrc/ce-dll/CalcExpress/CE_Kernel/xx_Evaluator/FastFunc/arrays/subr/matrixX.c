/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =========================================
//	matrixn(val, rows [,cols])
// =========================================
MgErr	_matrixn_arr(	_gen_operand	*opdArr,
						int32			qty,
						_gen_operand	*out )
{
	_gen_operand	*arg;
	int32			i, elSize, rows, cols;
	char			*pnt;
	MgErr			err = noErr;

	arg = opdArr+1;

	if (arg->ResType != OPTYPE_DBL) {
		out->erMsg = err_InvArgType2_needScal;	goto out;
	}
	rows = _DblToI32(arg->scalar);
	if (qty == 3) {
		arg += 1;
		if (arg->ResType != OPTYPE_DBL) {
			out->erMsg = err_InvArgType3_needScal;	goto out;
		}
	}
	cols = _DblToI32(arg->scalar);

	switch(opdArr->ResType) {
		case OPTYPE_DBL :	elSize = sizeof(float64);	break;
		case OPTYPE_CDB :	elSize = sizeof(float64) * 2;	break;
		case OPTYPE_STR :	elSize = sizeof(LStr*);		break;
		default:	out->erMsg = err_InvArgType1_cantArr;	goto out;
	}
	err = __create_empty_matrix(opdArr->ResType, rows, cols, out, FALSE);

	for (i = out->size1 * out->size2, pnt = out->array; i > 0; i--) {
		_MoveBlock(opdArr->array, pnt, elSize);
		pnt += elSize;
	}
out:	return err;
}

// =========================================
//	matrixnv(vector, rows)
// =========================================
MgErr	_matrixnv_arr(	_gen_operand	*opdArr,
						int32			qty,
						_gen_operand	*out )
{
	_gen_operand	*arg;
	int32			i, elSize, rows, cols;
	char			*pnt;
	MgErr			err = noErr;

	if (qty == 2) {
		arg = opdArr+1;
		if (arg->ResType != OPTYPE_DBL) {
			out->erMsg = err_InvArgType2_needScal;	goto out;
		}
		rows = _DblToI32(arg->scalar);
	}
	else	rows = 1;

	switch(opdArr->ResType) {
		case OPTYPE_A1DBL :	elSize = sizeof(float64);	break;
		case OPTYPE_A1CDB :	elSize = sizeof(float64) * 2;	break;
		case OPTYPE_A1STR :	elSize = sizeof(LStr*);		break;
		default:	out->erMsg = err_InvArgType_needArr1D;	goto out;
	}
	cols = opdArr->size1;

	err = __create_empty_matrix(_operand_get_BaseType(opdArr->ResType),
					rows, cols, out, FALSE);

	for (i = out->size1, pnt = out->array; i > 0; i--) {
		_MoveBlock(opdArr->array, pnt, out->size2 * elSize);
		pnt += out->size2 * elSize;
	}
out:	return err;
}

// =========================================
//	matrixd(val, size [,diag])
// =========================================
MgErr	_matrixd_arr(	_gen_operand	*opdArr,
			int32		qty,
			_gen_operand	*out )
{
	_gen_operand	*arg;
	int32		i, elSize, rows;
	int32		diag = 0;
	char		*pnt;
	MgErr		err = noErr;

	arg = opdArr+1;

	if (arg->ResType != OPTYPE_DBL) {
		out->erMsg = err_InvArgType2_needScal;	goto out;
	}
	rows = _DblToI32(arg->scalar);

	if (qty == 3) {
		arg += 1;
		if (arg->ResType != OPTYPE_DBL) {
			out->erMsg = err_InvArgType3_needScal;	goto out;
		}
		diag = _DblToI32(arg->scalar);
	}

	switch(opdArr->ResType) {
		case OPTYPE_DBL :	elSize = sizeof(float64);	break;
		case OPTYPE_CDB :	elSize = sizeof(float64) * 2;	break;
		case OPTYPE_STR :	elSize = sizeof(LStr*);		break;
		default:	out->erMsg = err_InvArgType1_cantArr;	goto out;
	}
	err = __create_empty_matrix(opdArr->ResType, rows, rows, out, TRUE);

	for (i=0; i < out->size1; i++) {
		if (diag <= 0)
			pnt = (char*)out->array + elSize * (i*out->size2 + i);
		else
			pnt = (char*)out->array + elSize * (i*out->size2 + out->size2 - i - 1);
		_MoveBlock(opdArr->array, pnt, elSize);
	}
out:	return err;
}

// =========================================
//	matrixdv(vector [,diag])
// =========================================
MgErr	_matrixdv_arr(	_gen_operand	*opdArr,
			int32		qty,
			_gen_operand	*out )
{
	_gen_operand	*arg;
	int32		i, elSize, rows;
	int32		diag = 0;
	char		*src, *dst;
	MgErr		err = noErr;

	if (qty == 2) {
		arg = opdArr+1;
		if (arg->ResType != OPTYPE_DBL) {
			out->erMsg = err_InvArgType2_needScal;	goto out;
		}
		diag = _DblToI32(arg->scalar);
	}

	switch(opdArr->ResType) {
		case OPTYPE_A1DBL :	elSize = sizeof(float64);	break;
		case OPTYPE_A1CDB :	elSize = sizeof(float64) * 2;	break;
		case OPTYPE_A1STR :	elSize = sizeof(LStr*);		break;
		default:	out->erMsg = err_InvArgType_needArr1D;	goto out;
	}
	rows = opdArr->size1;

	err = __create_empty_matrix(_operand_get_BaseType(opdArr->ResType),
					rows, rows, out, TRUE);

	for (i=0, src=opdArr->array; i < out->size1; i++, src += elSize) {
		if (diag <= 0)
			dst = (char*)out->array + elSize * (i*out->size2 + i);
		else
			dst = (char*)out->array + elSize * (i*out->size2 + out->size2 - i - 1);
		_MoveBlock(src, dst, elSize);
	}
out:	return err;
}

// =========================================
//	vec2mtx(vector, rows, cols)
// =========================================
MgErr	_vec2mtx_arr(	_gen_operand	*opdArr,
			int32		qty,
			_gen_operand	*out )
{
	_gen_operand	*arg;
	int32		elSize, rows, cols, size;
	MgErr		err = noErr;

	arg = opdArr+1;
	if (arg->ResType != OPTYPE_DBL) {
		out->erMsg = err_InvArgType2_needScal;	goto out;
	}
	rows = _DblToI32(arg->scalar);

	arg += 1;
	if (arg->ResType != OPTYPE_DBL) {
		out->erMsg = err_InvArgType3_needScal;	goto out;
	}
	cols = _DblToI32(arg->scalar);

	switch(opdArr->ResType) {
		case OPTYPE_A1DBL :	elSize = sizeof(float64);	break;
		case OPTYPE_A1CDB :	elSize = sizeof(float64) * 2;	break;
		case OPTYPE_A1STR :	elSize = sizeof(LStr*);		break;
		default:	out->erMsg = err_InvArgType_needArr1D;	goto out;
	}

	err = __create_empty_matrix(_operand_get_BaseType(opdArr->ResType),
					rows, cols, out, TRUE);
	if (err) goto out;

	size = __min(opdArr->size1, out->size1 * out->size2);

	_MoveBlock(opdArr->array, out->array, size * elSize);

out:	return err;
}

// ===================================================
//	submtx(matrix, idx1, len1, idx2, len2)
// ===================================================
MgErr	_submtx_arr(	_gen_operand	*opdArr,
			int32		qty,
			_gen_operand	*out )
{
	_gen_operand	*arg;
	int32		idx1, idx2, len1, len2;
	int32		i, elSize;
	char		*src, *dst;
	MgErr		err = noErr;

	// *** first argument
	switch(opdArr->ResType) {
		case OPTYPE_A2DBL :	elSize = sizeof(float64);	break;
		case OPTYPE_A2CDB :	elSize = sizeof(float64) * 2;	break;
		case OPTYPE_A2STR :	elSize = sizeof(LStr*);		break;
		default:	out->erMsg = err_InvArgType_needArr2D;	goto out;
	}
	// *** 2nd argument
	arg = opdArr + 1;
	if (arg->ResType != OPTYPE_DBL)
	{	out->erMsg = err_InvArgType2_needScal; goto out;	}
	idx1 = _DblToI32(arg->scalar);

	// *** 3rd argument
	arg = opdArr + 2;
	if (arg->ResType != OPTYPE_DBL)
	{	out->erMsg = err_InvArgType3_needScal; goto out;	}
	len1 = _DblToI32(arg->scalar);

	idx2 = idx1;	//by default idx2 = idx1
	len2 = len1;	//by default len2 = len1

	if (qty >= 4) {
		// *** 4th argument
		arg = opdArr + 3;
		if (arg->ResType != OPTYPE_DBL)
		{	out->erMsg = err_InvArgType4_needScal; goto out;	}
		idx2 = _DblToI32(arg->scalar);

		if (qty >= 5) {
			// *** 5th argument
			arg = opdArr + 4;
			if (arg->ResType != OPTYPE_DBL)
			{	out->erMsg = err_InvArgType_needScalar; goto out;	}
			len2 = _DblToI32(arg->scalar);
		}
	}

	// *** test conditions
	// -------------------
	if ((idx1 < 0) || (idx1 >= opdArr->size1))
		len1 = 0;
	else	len1 = __min(opdArr->size1 - idx1, len1);
	if ((len1 < 0) || (len1 > opdArr->size1))
		len1 = 0;
	// ---
	if ((idx2 < 0) || (idx2 >= opdArr->size2))
		len2 = 0;
	else	len2 = __min(opdArr->size2 - idx2, len2);
	if ((len2 < 0) || (len2 > opdArr->size2))
		len2 = 0;

	// *** Prepare out array
	// ---------------------
	err = __create_empty_matrix(_operand_get_BaseType(opdArr->ResType),
					len1, len2, out, FALSE);

	// *** Copy data
	// -------------
	dst = out->array;
	for ( i=0; i < len1; i++, idx1++) {
		src = (char*)opdArr->array + (idx1 * opdArr->size2 + idx2) * elSize;
		_MoveBlock(src, dst, len2 * elSize);
		dst += len2 * elSize;
	}
out:	return err;
}

