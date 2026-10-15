/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ============================================================================================
// ===============================  Заготовки для Функций   ===================================
// ============================================================================================

// initialize the result ('out' parameter)
// ----------------------------------
MgErr res2cdb_init( _dbl_operand *in1, _dbl_operand *in2, _dbl_operand *out)
{
	// Размерность и тип - по большему операнду
	out->nDims	= __max( in1->nDims,   in2->nDims );
	out->ResType	= __max( in1->ResType, in2->ResType );

	// ========================
	// Both operands are scalar
	// ========================
	if (!out->nDims) {
		out->size1 = out->size2 = 1;
		goto L1;
	}
	// ==========================
	// No one operand is 2D Array
	// ==========================
	if (out->nDims == 1) {
		out->size2 = 1;
		out->size1 = 0x7FFFFFFF;
		if (in1->nDims)	out->size1 = __min( out->size1, in1->size1 );
		if (in2->nDims)	out->size1 = __min( out->size1, in2->size1 );
		goto L1;
	}
	// ================================
	// At least one operand is 2D Array
	// ================================
	// !!! 1D Array is row !!!
	// ================================
	// ***  Number of rows  ***
	out->size1 = 0x7FFFFFFF;
	if (in1->nDims == 2)	out->size1 = __min( out->size1, in1->size1 );
	if (in2->nDims == 2)	out->size1 = __min( out->size1, in2->size1 );

	// ***  Number of columns ***
	out->size2 = 0x7FFFFFFF;
	switch(in1->nDims) {
		case 2:	out->size2 = __min( out->size2, in1->size2 ); break;
		case 1: out->size2 = __min( out->size2, in1->size1 ); break;
	}
	switch(in2->nDims) {
		case 2:	out->size2 = __min( out->size2, in2->size2 ); break;
		case 1: out->size2 = __min( out->size2, in2->size1 ); break;
	}
L1:
	// Подготовить массив результата
	out->array = ce_DSNewPtr( 2*(out->size1 * out->size2) * sizeof(float64) );
	if (!out->array)
		return mFullErr;
	out->alloc = TRUE;

	return noErr;
}

// ===============================================================

MgErr dbl_to_cdb( _dbl_operand	*opd)
{
	float64	*p1,*p;
	int32	i, size;

	p1 = opd->array;

	switch (opd->ResType) {
		case OPTYPE_DBL :
			opd->ResType = OPTYPE_CDB;
			opd->array = ce_DSNewPtr( 2 * sizeof(float64));
			if (!opd->array)
				goto nomem;
			opd->array[0] = opd->scalar;
			opd->array[1] = 0;
			break;

		case OPTYPE_A1DBL :
		case OPTYPE_A2DBL :
			opd->ResType += OPTYPE_A1CDB - OPTYPE_A1DBL;
			size = 2 * (opd->size1 * opd->size2);
			opd->array = ce_DSNewPtr( size * sizeof(float64));
			if (!opd->array)
				goto nomem;
			for (i=0, p=p1; i<size; ) {
				opd->array[i++] = *p++;
				opd->array[i++] = 0;
			}
//DSDisposePtr(p1); //не освобождаем, т.к. указывает не на выделенный массив
			break;

		default:
			return noErr;
	}
	opd->alloc = TRUE;
	return noErr;
nomem:
	opd->array = p1;
	return mFullErr;
}

// ===============================================================

// ---------------------------------
typedef MgErr	(*_f_arg2_cdb_low) ( _cdb_ *op1, _cdb_ *op2, _cdb_ *res);
// ---------------------------------
MgErr math_arg2cdb_template(		_dbl_operand	*in1,
					_dbl_operand	*in2,
					_dbl_operand	*out,
			register	_f_arg2_cdb_low	operation)
{
	int	size, row, col;
	_cdb_	*pnt, *p1, *p2;
	MgErr	err;

	if (err = dbl_to_cdb(in1))		goto out;
	if (err = dbl_to_cdb(in2))		goto out;
	if (err = res2cdb_init(in1,in2,out))	goto out;

	p1  = (_cdb_*)in1->array;
	p2  = (_cdb_*)in2->array;
	pnt = (_cdb_*)out->array;

	// ---  Оба операнда - Скаляры
	if (!out->nDims) {
		operation( p1, p2, pnt );
		goto out;
	}
	// ---  Только Первый Скаляр
	if ( !in1->nDims ) {
		for (size = out->size1 * out->size2; size; size--)
			operation( p1, p2++, pnt++ );
		goto out;
	}
	// ---  Только Второй Скаляр
	if ( !in2->nDims ) {
		for (size = out->size1 * out->size2; size; size--)
			operation( p1++, p2, pnt++ );
		goto out;
	}

	// --- Массивы одной размерности
	if ( in1->nDims == in2->nDims ) {

		if (in1->nDims == 1) {
			// ***  1D Arrays  ***
			for (size = out->size1; size; size--) {
				operation( p1++, p2++, pnt++);
			}
		}
		else {	// ***  2D Arrays  ***
			for (row=0; row < out->size1; row++) {
				p1 = (_cdb_*)in1->array + row * in1->size2;
				p2 = (_cdb_*)in2->array + row * in2->size2;
				for (col=0; col < out->size2; col++) {
					operation( p1++, p2++, pnt++);
				}
			}
		}
		goto out;
	}

	// --- Массивы разных размерностей
	for (row=0; row < out->size1; row++) {
		p1 = (_cdb_*)in1->array;
		if (in1->nDims == 2)	p1 += row * in1->size2;
		p2 = (_cdb_*)in2->array;
		if (in2->nDims == 2)	p2 += row * in2->size2;
		for (col=0; col < out->size2; col++)
			operation( p1++, p2++, pnt++);
	}
out:
	return err;
}
