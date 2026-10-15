/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =========================================
//	vectorn(val, size)
// =========================================
MgErr	_vectorn_arr(	_gen_operand	*opdArr,
						int32			qty,
						_gen_operand	*out )
{
	_gen_operand	*arg1, *arg2;
	int32			i, size, elSize;
	char			*pnt;
	MgErr			err = noErr;

	arg1 = opdArr;
	arg2 = opdArr+1;

	if (arg2->ResType != OPTYPE_DBL) {
		out->erMsg = err_InvArgType2_needScal;	goto out;
	}
	size = _DblToI32(arg2->scalar);
	if (size < 0)	size = 0;

	switch(arg1->ResType) {
		case OPTYPE_DBL :	elSize = sizeof(float64);	break;
		case OPTYPE_CDB :	elSize = sizeof(float64) * 2;	break;
		case OPTYPE_STR :	elSize = sizeof(LStr*);		break;
		default:	out->erMsg = err_InvArgType1_cantArr;	goto out;
	}
	out->nDims	= 1;
	out->ResType	= _operand_get_TargetType(arg1->ResType, out->nDims);
	out->size1	= size;
	out->size2	= 1;
	out->array	= ce_DSNewPtr(size * elSize);
	out->alloc	= (out->array != NULL);
	if (!(out->alloc)) { err = mFullErr; goto out; }

	for (i=size, pnt = out->array; i > 0; i--) {
		_MoveBlock(arg1->array, pnt, elSize);
		pnt += elSize;
	}
out:	return err;
}

// =========================================
//	 mtx2vec(matrix)
// =========================================
MgErr	_mtx2vec_arr(	_gen_operand	*opdArr,
			int32		qty,
			_gen_operand	*out )
{
	int32		size, elSize;
	MgErr		err = noErr;

	switch(opdArr->ResType) {
		case OPTYPE_A2DBL :	elSize = sizeof(float64);	break;
		case OPTYPE_A2CDB :	elSize = sizeof(float64) * 2;	break;
		case OPTYPE_A2STR :	elSize = sizeof(LStr*);		break;
		default:	out->erMsg = err_InvArgType_needArr2D;	goto out;
	}
	size = opdArr->size1 * opdArr->size2;

	out->nDims	= 1;
	out->ResType	= _operand_get_TargetType(opdArr->ResType, out->nDims);
	out->size1	= size;
	out->size2	= 1;
	out->array	= ce_DSNewPtr(size * elSize);
	out->alloc	= (out->array != NULL);
	if (!(out->alloc)) { err = mFullErr; goto out; }

	_MoveBlock(opdArr->array, out->array, size * elSize);
out:	return err;
}

// =========================================
//	 subvec(vect, idx, len)
// =========================================
MgErr	_subvec_arr(	_gen_operand	*opdArr,
			int32		qty,
			_gen_operand	*out )
{
	_gen_operand	*arg;
	int32		idx, len, elSize;
	MgErr		err = noErr;

	// *** first argument
	switch(opdArr->ResType) {
		case OPTYPE_A1DBL :	elSize = sizeof(float64);	break;
		case OPTYPE_A1CDB :	elSize = sizeof(float64) * 2;	break;
		case OPTYPE_A1STR :	elSize = sizeof(LStr*);		break;
		default:	out->erMsg = err_InvArgType_needArr1D;	goto out;
	}
	// *** second argument
	arg = opdArr + 1;
	if (arg->ResType != OPTYPE_DBL) {
		out->erMsg = err_InvArgType2_needScal;	goto out;
	}
	idx = _DblToI32(arg->scalar);

	// *** third argument
	arg = opdArr + 2;
	if (arg->ResType != OPTYPE_DBL) {
		out->erMsg = err_InvArgType3_needScal;	goto out;
	}
	len = _DblToI32(arg->scalar);

	// *** test conditions
	// -------------------
	if ((idx < 0) || (idx >= opdArr->size1))
		len = 0;
	else	len = __min(opdArr->size1 - idx, len);
	if ((len < 0) || (len > opdArr->size1))
		len = 0;

	// *** Prepare out array
	out->nDims	= 1;
	out->ResType	= opdArr->ResType;
	out->size1	= len;
	out->size2	= 1;
	out->array	= ce_DSNewPtr(len * elSize);
	out->alloc	= (out->array != NULL);
	if (!(out->alloc)) { err = mFullErr; goto out; }

	// *** Copy data
	_MoveBlock((char*)opdArr->array + idx * elSize, out->array, len * elSize);
out:	return err;
}

