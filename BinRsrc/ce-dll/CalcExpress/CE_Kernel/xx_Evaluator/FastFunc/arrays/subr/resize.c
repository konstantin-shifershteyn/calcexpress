/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =========================================
//	resize(vector, newsize)
// =========================================
MgErr	_resize_arr(	_gen_operand	*opdArr,
						int32			qty,
						_gen_operand	*out )
{
	_gen_operand	*arg;
	int32			elSize, oldsize, newsize;
	MgErr			err = noErr;

	// *** 1st parameter
	switch(opdArr->ResType) {
		case OPTYPE_A1DBL :	elSize = sizeof(float64);	break;
		case OPTYPE_A1CDB :	elSize = sizeof(float64) * 2;	break;
		case OPTYPE_A1STR :	elSize = sizeof(LStr*);		break;
		default:	out->erMsg = err_InvArgType1_needArr1D;	goto out;
	}
	oldsize = opdArr->size1;

	// *** 2nd parameter
	arg = opdArr+1;
	if (arg->ResType != OPTYPE_DBL) {
		out->erMsg = err_InvArgType2_needScal;	goto out;
	}
	newsize = _DblToI32(arg->scalar);
	if (newsize < 0)
		newsize = 0;

	// *** Если не надо изменять размер
	if (newsize == oldsize) {
		out->ResType = opdArr->ResType;
		out->scalar  = opdArr->scalar;
		goto out;
	}

	err = __create_empty_vector(_operand_get_BaseType(opdArr->ResType),
					newsize, out, (newsize > oldsize));
	if (err) goto out;

	_MoveBlock(opdArr->array, out->array, elSize * __min(newsize,oldsize) );

out:	return err;
}

// =========================================
//	resize2d(matrix, rows, cols)
// =========================================
MgErr	_resize2d_arr(	_gen_operand	*opdArr,
			int32		qty,
			_gen_operand	*out )
{
	_gen_operand	*arg;
	int32		rows, cols, oldrows, oldcols;
	int32		i, elSize, NRows, NCols;
	char		*src, *dst;
	MgErr		err = noErr;

	// *** 1st parameter
	switch(opdArr->ResType) {
		case OPTYPE_A2DBL :	elSize = sizeof(float64);	break;
		case OPTYPE_A2CDB :	elSize = sizeof(float64) * 2;	break;
		case OPTYPE_A2STR :	elSize = sizeof(LStr*);		break;
		default:	out->erMsg = err_InvArgType1_needArr2D;	goto out;
	}
	oldrows = opdArr->size1;
	oldcols = opdArr->size2;

	// *** 2nd parameter
	arg = opdArr+1;
	if (arg->ResType != OPTYPE_DBL) {
		out->erMsg = err_InvArgType2_needScal;	goto out;
	}
	rows = _DblToI32(arg->scalar);
	if (rows < 0)	rows = 0;

	// *** 3rd parameter
	arg = opdArr+2;
	if (arg->ResType != OPTYPE_DBL) {
		out->erMsg = err_InvArgType3_needScal;	goto out;
	}
	cols = _DblToI32(arg->scalar);
	if (cols < 0)	cols = 0;

	// *** Если не надо изменять размер
	if ((rows == oldrows) && (cols == oldcols)) {
		out->ResType = opdArr->ResType;
		out->scalar  = opdArr->scalar;
		goto out;
	}

	err = __create_empty_matrix(_operand_get_BaseType(opdArr->ResType),
					rows, cols, out,
					(rows > oldrows)||(cols > oldcols));
	if (err) goto out;

	NRows = __min(rows, oldrows);
	NCols = __min(cols, oldcols);
	for(i=0; i < NRows; i++) {
		src = (char*)opdArr->array + (i*oldcols)*elSize;
		dst = (char*)out->array    + (i*cols)*elSize;
		_MoveBlock(src, dst, elSize * NCols );
	}
out:
	return err;
}

