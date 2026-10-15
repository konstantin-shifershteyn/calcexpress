/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// -----------------------------------------------
// The function helps to check and obtain argument
// -----------------------------------------------
Bool32	_vectX_get_par(	_dbl_operand	*in,
						float64			*val )
{
	switch (in->ResType) {
		case OPTYPE_DBL:		*val = in->scalar;
		case OPTYPE_SKIPPED_ARG:	break;
		default:			return TRUE;
	}
	return FALSE;
}

// ------------------------------
// The function prepares a result
// ------------------------------
void	_vectX_init_result(	_dbl_operand	*out,
							int32			size )
{
	int32	maxSize;

	maxSize = (int32)(DSMaxMem()/sizeof(float64));	// у меня возвращает 0
	if (maxSize <= 0) {
		maxSize = 16*1024*1024/sizeof(float64);		// 16MB
	}

	if (size < 0)	size = 0;

	if (size > maxSize) {
		out->erMsg = "Too large array size is requested";
		size = 0;
	}
	else {
		out->array	= ce_DSNewPtr( size * sizeof(float64) );
		out->alloc	= (out->array != NULL);
		if (!out->alloc) {
			out->erMsg = "No enough memory to create vector";
			size = 0;
		}
	}

	out->ResType	= OPTYPE_A1DBL;
	out->nDims	= 1;
	out->size1	= size;
	out->size2	= 1;
}

// =========================================
//	 nvecins(init, num [,step] )
// =========================================
MgErr	_nvecins_arr(	_dbl_operand	*in1,
			_dbl_operand	*in2,
			_dbl_operand	*in3,
			_dbl_operand	*out )
{
	int32	i, size;
	float64	*p;
	float64	init=0.0, num=100.0, step=1.0;

	// *** ger argument values
	// -----------------------
	if (_vectX_get_par(in1, &init)) {	out->erMsg = err_InvArgType1_needScal; goto out;	}
	if (_vectX_get_par(in2, &num)) {	out->erMsg = err_InvArgType2_needScal; goto out;	}
	if (_vectX_get_par(in3, &step)) {	out->erMsg = err_InvArgType3_needScal; goto out;	}

	// *** test conditions
	// -------------------
	if (num < 0)	size = 0;
	else			size = _DblToI32(num);

	// *** prepare result
	// ------------------
	_vectX_init_result(out, size);
	if (out->erMsg)	return noErr;

	// *** fill vector
	// ---------------
	for (i=0, p=out->array; i < size; i++)
		*p++ = init + step * i;
out:
	return	noErr;
}

// =========================================
//	 nvecifn(init, final [,num] )
// =========================================
MgErr	_nvecifn_arr(	_dbl_operand	*in1,
			_dbl_operand	*in2,
			_dbl_operand	*in3,
			_dbl_operand	*out )
{
	int32	i, size;
	float64	*p;
	float64	init=0.0, final, num=100.0, step;

	// *** ger argument values
	// -----------------------
	if (_vectX_get_par(in1, &init)) {	out->erMsg = err_InvArgType1_needScal; goto out;	}

	final = init + 1.0;			// default value
	if (_vectX_get_par(in2, &final)) {	out->erMsg = err_InvArgType2_needScal; goto out;	}
	if (_vectX_get_par(in3, &num)) {	out->erMsg = err_InvArgType3_needScal; goto out;	}

	// *** test conditions
	// -------------------
	size = _DblToI32(num);
	if (size < 0)	size = 0;
	if (size > 1)	step = (final - init)/(size-1);
	else		step = 0;

	// *** prepare result
	// ------------------
	_vectX_init_result(out, size);
	if (out->erMsg)	return noErr;

	// *** fill vector
	// ---------------
	for (i=0, p=out->array; i < size; i++)
		*p++ = init + step * i;
out:
	return	noErr;
}

// =========================================
//	 nvecifs(init, final [,step] )
// =========================================
MgErr	_nvecifs_arr(	_dbl_operand	*in1,
			_dbl_operand	*in2,
			_dbl_operand	*in3,
			_dbl_operand	*out )
{
	int32	i, size;
	float64	*p;
	float64	init=0.0, final, step;

	// *** ger argument values
	// -----------------------
	if (_vectX_get_par(in1, &init)) {	out->erMsg = err_InvArgType1_needScal; goto out;	}
	final = init + 1.0;			// default value
	if (_vectX_get_par(in2, &final)) {	out->erMsg = err_InvArgType2_needScal; goto out;	}
	step  = (final - init)/100.0;		// default value
	if (_vectX_get_par(in3, &step)) {	out->erMsg = err_InvArgType3_needScal; goto out;	}

	// *** test conditions
	// -------------------
	if (step != 0.0)	size = 1 + _DblToI32(floor((final - init)/step));
	else				size = 0;
	if (size <= 0)		size = 0;

	// *** prepare result
	// ------------------
	_vectX_init_result(out, size);
	if (out->erMsg)	return noErr;

	// *** fill vector
	// ---------------
	for (i=0, p=out->array; i < size; i++)
		*p++ = init + step * i;
out:
	return	noErr;
}

