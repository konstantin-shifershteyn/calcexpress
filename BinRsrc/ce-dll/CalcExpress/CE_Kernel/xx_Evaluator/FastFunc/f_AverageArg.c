/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ================================================================================================
// =====  min( x1 [,x2 .. [,xN]] )  ===============================================================
// ================================================================================================
MgErr _min_dbl( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	register	int32	size;
	register	float64	*nums;

	if (!idx) {	// ***** При первом вызове проинициализировать результат ***
		out->scalar = (in->nDims) ? *(in->array) : in->scalar;	// ***** начальное значение
	}
	if (idx >= arg_qty)	// ***** Последний вызов: ничего не делать, результат уже готов
		return noErr;

	// ***** Выполнение *****
	if ( in->nDims) {
		for ( nums= in->array, size= in->size1 * in->size2; size; size--, nums++)
			if (*nums < out->scalar)
				out->scalar = *nums;
	}
	else		if (in->scalar < out->scalar)
				out->scalar = in->scalar;
	return noErr;
}
// ================================================================================================
// =====  max( x1 [,x2 .. [,xN]] )  ===============================================================
// ================================================================================================
MgErr _max_dbl( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	register	int32	size;
	register	float64	*nums;

	if (!idx) {	// ***** При первом вызове проинициализировать результат ***
		out->scalar = (in->nDims) ? *(in->array) : in->scalar;	// ***** начальное значение
	}
	if (idx >= arg_qty)	// ***** Последний вызов: ничего не делать, результат уже готов
		return noErr;

	// ***** Выполнение *****
	if ( in->nDims) {
		for ( nums= in->array, size= in->size1 * in->size2; size; size--, nums++)
			if (*nums > out->scalar)
				out->scalar = *nums;
	}
	else		if (in->scalar > out->scalar)
				out->scalar = in->scalar;
	return noErr;
}

// ================================================================================================
// =====  sum( x1 [,x2 .. [,xN]] )  ===============================================================
// ================================================================================================
MgErr _sum_dbl( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	register	int32	size;
	register	float64	*nums;

	if (idx >= arg_qty)	// ***** Последний вызов: ничего не делать, результат уже готов
		return noErr;

	// ***** Выполнение *****
	if ( in->nDims)
		for ( nums= in->array, size= in->size1 * in->size2; size; size--, nums++)
			out->scalar += *nums;
	else		out->scalar += in->scalar;
	return noErr;
}
// ====== CDB Версия =======
MgErr _sum_cdb( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	register	int32	size;
	register	_cdb_	*cdb;
			MgErr	err;

	if (idx >= arg_qty)	// ***** Последний вызов: ничего не делать, результат уже готов
		return noErr;

	// ***** Подготовка операндов ******
	if (err = dbl_to_cdb(out))	return err;
	if (err = dbl_to_cdb(in))	return err;

	// ***** Выполнение *****
	cdb = (_cdb_*)in->array;
	for( size=in->size1 * in->size2;  size > 0;  size--, cdb++)
		add_cdb( (_cdb_*)out->array, cdb, (_cdb_*)out->array );
	return noErr;
}

// ================================================================================================
// =====  mean( x1 [,x2 .. [,xN]] )  ==============================================================
// ================================================================================================
MgErr _mean_dbl( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	register	int32	size;
	register	float64	*nums;

	if (idx >= arg_qty) {	// ***** Последний вызов: вычислить значение
		out->scalar /= tmp->scalar;
		return noErr;
	}
	// ***** Выполнение *****
	if ( in->nDims) {
		size = in->size1 * in->size2;
		tmp->scalar += size;
		for ( nums= in->array; size; size--, nums++)
			out->scalar += *nums;
	}
	else {		out->scalar += in->scalar;
			tmp->scalar += 1;
	}
	return noErr;
}
// ====== CDB Версия =======
MgErr _mean_cdb( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	register	int32	size;
	register	_cdb_	*cdb;
			MgErr	err;

	if (idx >= arg_qty) {	// ***** Последний вызов: вычислить значение
		if (err = dbl_to_cdb(tmp))	//преобразовать счетчик из DBL в CDB
			return err;
		div_cdb( (_cdb_*)out->array, (_cdb_*)tmp->array, (_cdb_*)out->array );
		return noErr;
	}
	// ***** Подготовка операндов ******
	if (err = dbl_to_cdb(in))	return err;
	if (err = dbl_to_cdb(out))	return err;

	// ***** Выполнение *****
	size = in->size1 * in->size2;
	tmp->scalar += size;		// счетчик - DBL scalar
	for( cdb = (_cdb_*)in->array;  size > 0;  size--, cdb++)
		add_cdb( (_cdb_*)out->array, cdb, (_cdb_*)out->array );
	return noErr;
}

// ================================================================================================
// =====  prod( x1 [,x2 .. [,xN]] )  ==============================================================
// ================================================================================================
MgErr _prod_dbl( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	register	int32	size;
	register	float64	*nums;

	if (!idx) {			// ***** При первом вызове проинициализировать результат ***
		out->scalar = 1;
	}
	if (idx >= arg_qty)		// ***** Последний вызов: ничего не делать, результат уже готов
		return noErr;

	// ***** Выполнение *****
	if ( in->nDims)
		for ( nums= in->array, size= in->size1 * in->size2; size; size--, nums++)
			out->scalar *= *nums;
	else		out->scalar *= in->scalar;
	return noErr;
}
// ====== CDB Версия =======
MgErr _prod_cdb( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	register	int32	size;
	register	_cdb_	*cdb;
			MgErr	err;

	if (!idx) {			// ***** При первом вызове проинициализировать результат ***
		out->scalar = 1;
	}
	if (idx >= arg_qty)	// ***** Последний вызов: ничего не делать, результат уже готов
		return noErr;

	// ***** Подготовка операндов ******
	if (err = dbl_to_cdb(out))	return err;
	if (err = dbl_to_cdb(in))	return err;

	// ***** Выполнение *****
	cdb = (_cdb_*)in->array;
	for( size=in->size1 * in->size2;  size > 0;  size--, cdb++)
		mul_cdb( (_cdb_*)out->array, cdb, (_cdb_*)out->array );
	return noErr;
}

// ================================================================================================
// =====  land( x1 [,x2 .. [,xN]] )  ===============================================================
// ================================================================================================
MgErr _landFunc_dbl( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	register	int32	size;
	register	float64	*nums;
	
	if (idx >= arg_qty)		// Последний вызов: ничего не делать, результат уже готов
		return noErr;

	if (!idx) {				// При первом вызове проинициализировать результат ***
		out->scalar = 1;	// начинаем с TRUE
	}

	// ***** Выполнение *****
	if (_DblToI32Bits(out->scalar)) {

		// ***** еще не обнаружили FALSE (ноль)
		if ( in->nDims) {
			for ( nums= in->array, size= in->size1 * in->size2; size; size--, nums++)
				if ( !(_DblToI32Bits(*nums)) ) {
					out->scalar = 0; break;
				}
		}
		else {
			if (!_DblToI32Bits(in->scalar))
				out->scalar = 0;
		}
	}
	return noErr;
}

// ================================================================================================
// =====  lor( x1 [,x2 .. [,xN]] )  ===============================================================
// ================================================================================================
MgErr _lorFunc_dbl( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	register	int32	size;
	register	float64	*nums;
	
	if (idx >= arg_qty)		// Последний вызов: ничего не делать, результат уже готов
		return noErr;

	if (!idx) {			// При первом вызове проинициализировать результат ***
		out->scalar = 0;	// начинаем с FALSE
	}

	// ***** Выполнение *****
	if ( !(_DblToI32Bits(out->scalar)) ) {

		// ***** еще не обнаружили TRUE (не ноль)
		if ( in->nDims) {
			for ( nums= in->array, size= in->size1 * in->size2; size; size--, nums++)
				if (_DblToI32Bits(*nums)) {
					out->scalar = 1; break;
				}
		}
		else {
			if (_DblToI32Bits(in->scalar))
				out->scalar = 1;
		}
	}
	return noErr;
}

// ================================================================================================
// =====  lxor( x1 [,x2 .. [,xN]] )  ===============================================================
// ================================================================================================
MgErr _lxorFunc_dbl( _dbl_operand *in, _dbl_operand *out, _dbl_operand *tmp, int32 idx, int32 arg_qty )
{
	register	int32	size;
	register	float64	*nums;
	
	if (idx >= arg_qty)		// Последний вызов: ничего не делать, результат уже готов
		return noErr;

	if (!idx) {			// При первом вызове проинициализировать результат ***
		out->scalar = 0;	// начинаем с FALSE
	}

	// ***** Выполнение *****
	if ( in->nDims)
		for ( nums= in->array, size= in->size1 * in->size2; size; size--, nums++)
			out->scalar = LogXOR( _DblToI32Bits(out->scalar), _DblToI32Bits(*nums) );
	else	out->scalar = LogXOR( _DblToI32Bits(out->scalar), _DblToI32Bits(in->scalar) );

	return noErr;
}
