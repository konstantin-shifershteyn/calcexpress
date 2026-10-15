/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// -----------------------------------------------
// The function helps to check and obtain argument
// -----------------------------------------------
Bool32	_matrX_get_par(	_dbl_operand	*opdArr,
			int32		qty,
			int32		idx,
			float64		*val)
{
	if (idx < qty) {
		if (opdArr[idx].ResType != OPTYPE_DBL)
			return TRUE;
		*val = opdArr[idx].scalar;
	}
	return FALSE;
}

// ------------------------------
// The function prepares a result
// ------------------------------
void	_matrX_init_result(	_dbl_operand	*out,
				int32		rows,
				int32		cols )
{
	int32	size, maxSize;

	size	= rows * cols;
	if (size < 0)	size = 0;

	maxSize = (int32)(DSMaxMem()/sizeof(float64));	// у меня возвращает 0
	if (maxSize <= 0) {
		maxSize = 8*1024*1024/sizeof(float64);	// 8MB ( 1M values)
	}

	if (size > maxSize) {
		out->erMsg = "Too large array size is requested";
		size = 0;
	}
	else {
		out->array	= ce_DSNewPtr( size * sizeof(float64) );
		out->alloc	= (out->array != NULL);
		if (!out->alloc) {
			out->erMsg = "No enough memory to create matrix";
			size = 0;
		}
	}
	out->ResType	= OPTYPE_A2DBL;
	out->nDims	= 2;
	out->size1	= rows;
	out->size2	= cols;
}

// ========================================================================
//	 nmtxins( init, rNum, rStep [,cNum [,cStep]] )
// ========================================================================
MgErr	_nmtxins_arr(	_dbl_operand	*opdArr,
			int32		qty,
			_dbl_operand	*out )
{
	float64	init=0, num1=10, num2=10, step1=1, step2=1;
	int32	i, j, rows, cols;
	float64	*p, val;

	// *** ger argument values
	// -----------------------
	if (_matrX_get_par(opdArr, qty, 0, &init))	goto err;
	if (_matrX_get_par(opdArr, qty, 1, &num1))	goto err;
	if (_matrX_get_par(opdArr, qty, 2, &step1))	goto err;
	num2	= num1;
	step2	= step1;
	if (_matrX_get_par(opdArr, qty, 3, &num2))	goto err;
	if (_matrX_get_par(opdArr, qty, 4, &step2))	goto err;

	// *** test conditions
	// -------------------
	rows = _DblToI32(num1);	if (rows < 0)	rows = 0;
	cols = _DblToI32(num2);	if (cols < 0)	cols = 0;

	// *** prepare result
	// ------------------
	_matrX_init_result(out, rows, cols);
	if (out->erMsg)	return noErr;

	// *** fill the array
	// ------------------
	p=out->array;
	for (i=0; i < rows; i++) {
		val = init + step1 * i;
		for (j=0; j < cols; j++) {
			*p++ = val + step2 * j;
		}
	}
	return	noErr;
err:
	out->erMsg = err_InvArgType_needScalar;
	return	noErr;
}

// ========================================================================
//	 nmtxifn( init, rFinal, rNum [,cFinal [,cNnum]] )
// ========================================================================
MgErr	_nmtxifn_arr(	_dbl_operand	*opdArr,
			int32		qty,
			_dbl_operand	*out )
{
	float64	init=0, final1=10, final2=10, num1=10, num2=10;
	float64	step1, step2;
	int32	i, j, rows, cols;
	float64	*p, val;

	// *** get argument values
	// -----------------------
	if (_matrX_get_par(opdArr, qty, 0, &init))	goto err;
	if (_matrX_get_par(opdArr, qty, 1, &final1))	goto err;
	if (_matrX_get_par(opdArr, qty, 2, &num1))	goto err;
	final2	= final1;
	num2	= num1;
	if (_matrX_get_par(opdArr, qty, 3, &final2))	goto err;
	if (_matrX_get_par(opdArr, qty, 4, &num2))	goto err;

	// *** test conditions
	// -------------------
	rows = _DblToI32(num1);
	if (rows < 0)	rows = 0;
	if (rows > 1)	step1 = (final1 - init)/(rows-1);
	else		step1 = 0;
	cols = _DblToI32(num2);
	if (cols < 0)	cols = 0;
	if (cols > 1)	step2 = (final2 - init)/(cols-1);
	else		step2 = 0;

	// *** prepare result
	// ------------------
	_matrX_init_result(out, rows, cols);
	if (out->erMsg)	return noErr;

	// *** fill the array
	// ------------------
	p=out->array;
	for (i=0; i < rows; i++) {
		val = init + step1 * i;
		for (j=0; j < cols; j++) {
			*p++ = val + step2 * j;
		}
	}
	return	noErr;
err:
	out->erMsg = err_InvArgType_needScalar;
	return	noErr;
}

// ========================================================================
//	 nmtxifs( init, rFinal, rStep [,cFinal [,cStep]] )
// ========================================================================
MgErr	_nmtxifs_arr(	_dbl_operand	*opdArr,
			int32		qty,
			_dbl_operand	*out )
{
	float64	init=0, final1=10, final2=10, step1=1, step2=1;
	int32	i, j, rows, cols;
	float64	*p, val;

	// *** get argument values
	// -----------------------
	if (_matrX_get_par(opdArr, qty, 0, &init))	goto err;
	if (_matrX_get_par(opdArr, qty, 1, &final1))	goto err;
//	step1 = (final1 - init)/(10-1);
	if (_matrX_get_par(opdArr, qty, 2, &step1))	goto err;

	final2	= final1;
	step2	= step1;
	if (_matrX_get_par(opdArr, qty, 3, &final2))	goto err;
	if (_matrX_get_par(opdArr, qty, 4, &step2))	goto err;

	// *** test conditions
	// -------------------
	if (step1 != 0)	rows = 1 + _DblToI32(floor((final1 - init)/step1));
	else		rows = 0;
	if (rows <= 0)	rows = 0;
//------
	if (step2 != 0)	cols = 1 + _DblToI32(floor((final2 - init)/step2));
	else		cols = 0;
	if (cols <= 0)	cols = 0;

	// *** prepare result
	// ------------------
	_matrX_init_result(out, rows, cols);
	if (out->erMsg)	return noErr;

	// *** fill the array
	// ------------------
	p=out->array;
	for (i=0; i < rows; i++) {
		val = init + step1 * i;
		for (j=0; j < cols; j++) {
			*p++ = val + step2 * j;
		}
	}
	return	noErr;
err:
	out->erMsg = err_InvArgType_needScalar;
	return	noErr;
}
