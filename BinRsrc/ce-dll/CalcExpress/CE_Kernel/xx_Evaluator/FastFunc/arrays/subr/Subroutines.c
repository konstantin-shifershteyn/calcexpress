/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =========================================
// Создает пустой вектор
// =========================================
MgErr	__create_empty_vector(	int32		valType,
				int32		size,
				_gen_operand	*out,
				Bool32		FillIt )
{
	int32		elSize;
	char		*pnt;
	MgErr		err = noErr;
	void	*stub;
static	LStr	nulStr		= {0, 0};
static	void	*zeroStr	= &nulStr;
static	float64	zeroNum[2]	= {0, 0};

	//---------

	if (size < 0)	size = 0;

	switch(valType) {
		case OPTYPE_DBL :	elSize = sizeof(float64);	stub = &zeroNum;
					break;
		case OPTYPE_CDB :	elSize = sizeof(float64)*2;	stub = &zeroNum;
					break;
		case OPTYPE_STR:	elSize = sizeof(LStr*);		stub = &zeroStr;
					break;
		default:	out->erMsg = err_InvArgType_cantArr;	goto out;
	}
	out->nDims	= 1;
	out->ResType	= _operand_get_TargetType(valType, out->nDims);
	out->size1	= size;
	out->size2	= 1;
	out->array	= ce_DSNewPtr( size * elSize);
	out->alloc	= (out->array != NULL);
	if (!(out->alloc)) { err = mFullErr; goto out; }

	if (FillIt) {
		for (pnt = out->array; size > 0; size--) {
			_MoveBlock(stub, pnt, elSize);
			pnt += elSize;
		}
	}
out:	return err;
}

// =========================================
// Создает пустую матрицу NxM
// =========================================
MgErr	__create_empty_matrix(	int32		valType,
				int32		rows,
				int32		cols,
				_gen_operand	*out,
				Bool32		FillIt )
{
	int32		i, elSize;
	char		*pnt;
	MgErr		err = noErr;
	void	*stub;
static	LStr	nulStr		= {0, 0};
static	void	*zeroStr	= &nulStr;
static	float64	zeroNum[2]	= {0, 0};

	//---------

	if (rows < 0)	rows = 0;
	if (cols < 0)	cols = 0;

	switch(valType) {
		case OPTYPE_DBL :	elSize = sizeof(float64);	stub = &zeroNum;
					break;
		case OPTYPE_CDB :	elSize = sizeof(float64)*2;	stub = &zeroNum;
					break;
		case OPTYPE_STR:	elSize = sizeof(LStr*);		stub = &zeroStr;
					break;
		default:	out->erMsg = err_InvArgType_cantArr;	goto out;
	}
	out->nDims	= 2;
	out->ResType	= _operand_get_TargetType(valType, out->nDims);
	out->size1	= rows;
	out->size2	= cols;
	out->array	= ce_DSNewPtr( rows * cols * elSize);
	out->alloc	= (out->array != NULL);
	if (!(out->alloc)) { err = mFullErr; goto out; }

	if (FillIt) {
		for (i=rows*cols, pnt = out->array; i > 0; i--) {
			_MoveBlock(stub, pnt, elSize);
			pnt += elSize;
		}
	}
out:	return err;
}

