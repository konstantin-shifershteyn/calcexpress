/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =====================================================================
// The function assigns values to indexed elements within 2D Arrays
// =====================================================================
void	_set_idxEl_2d_arr(_gen_operand *arr, _indexes_info *idxs, _gen_operand *val)
{
	// this typedef defines a generic pointer to a datatype-specific indexing function
	typedef void (*_idx2dfunc)();
	// this structure contains a stub index: nDims == 1 --> index range for entire dimension size
//	static	_idx_info	idxStub = {-1, 0, 0x7FFFFFFF, 0, &idxStub.scalar, FALSE, NULL };
	static	_idx_info	idxStub = {-1, 0, 0x7FFFFFFF, 0, &idxStub.scalar, FALSE, 0, NULL };	// >>> #CE_3.0.4 2026-07-04

	_idx_info	*ip1,	*ip2;		// pointers to index descriptors
	int32		*i1Arr, *i2Arr;		// pointers to index arrays

	int32		i,j;
	int32		idx1, idx2;
	int32		size1, size2;

	_gen_operand	res;
	_idx_HowToProc	ProcInf;

	_idx2dfunc	_getfunc1, _getfunc2;	// pointers to the datatype-specific indexing functions
	_idx2dfunc	_setfunc;				// a pointer to the datatype-specific indexed assignment function
	char		buf[2*sizeof(float64)];
	//-----------

	if (arr->nDims != 2) {
		arr->erMsg = "Internal Error: incorrect call of the function (not 2D array) in "__FUNCTION__"()";
		return;
	}

	ip1	= &(idxs->idx1);
	if (idxs->qty < 2)	ip2 = &(idxStub);
	else			ip2 = &(idxs->idx2);

	// settings to initialize the result
	// ---------------------------------
	ProcInf.compound	= idxs->compound;
	ProcInf.idxAmount	= idxs->qty;
	ProcInf.allocArray	= FALSE;

	// ***** prepare the result structure
	// ==================================
	_index_a2_init_res(arr, ip1, ip2, &res, &ProcInf);
	if (res.erMsg) {
		arr->erMsg = res.erMsg;
		return;
	}
	// ---
	// the dimensionality of a data source cannot exceed dimensionality of the target array
	// ---
	if (res.nDims < val->nDims) {
		arr->erMsg = "Incompatible operand dimensions in assignment";
		return;
	}

	// dependencies on the array's element datatype
	// ============================================
	switch (_operand_get_BaseType(arr->ResType)) {
		case OPTYPE_DBL :	_setfunc  = set_element_of_A2DBL;
					_getfunc1 = get_element_of_A1DBL;
					_getfunc2 = get_element_of_A2DBL;
					break;
		case OPTYPE_CDB :	_setfunc  = set_element_of_A2CDB;
					_getfunc1 = get_element_of_A1CDB;
					_getfunc2 = get_element_of_A2CDB;
					break;
		case OPTYPE_STR :	_setfunc  = set_element_of_A2STR;
					_getfunc1 = get_element_of_A1STR;
					_getfunc2 = get_element_of_A2STR;
					break;
	}

	// ================================
	// *****    ASSIGN VALUES     *****
	// ================================

	i1Arr	= ip1->values;
	i2Arr	= ip2->values;

	switch(res.nDims) {

	case 0:
		// *** Result is a scalar
		// ======================
		_getfunc1(val, 0, buf);
		_setfunc(arr, *i1Arr, *i2Arr, buf);
		return;
		// ---------------------

	case 1:
		// *** Result is an 1D Array
		// =========================
		size1 = res.size1;	// !!!
		if (val->nDims) {
			if (val->size1 < size1)
				size1 = val->size1;
		}
		else
			_getfunc1(val, 0, buf);		// scalar value
		idx1 = *i1Arr;
		idx2 = *i2Arr;
		for (i=0; i < size1; i++) {
			switch(ProcInf.i1type) {
				case idxArray :			idx1 = i1Arr[i];	break;
				case idxIncremental :	idx1 = *i1Arr + i;	break;
				case idxDecremental :	idx1 = *i1Arr - i;	break;
			}
			switch(ProcInf.i2type) {
				case idxArray :			idx2 = i2Arr[i];	break;
				case idxIncremental :	idx2 = *i2Arr + i;	break;
				case idxDecremental :	idx2 = *i2Arr - i;	break;
			}
			if (val->nDims)
				_getfunc1(val, i, buf);
			_setfunc(arr, idx1, idx2, buf);
		}
		return;
		// ---------------------

	case 2:
		// *** Result is a 2D Array
		// ========================
		size1 = res.size1;	// !!!
		size2 = res.size2;	// !!!
		switch(val->nDims) {
			case 1:	if (val->size1 < size2)	size2 = val->size1;
					break;
			case 2:	if (val->size1 < size1)	size1 = val->size1;
					if (val->size2 < size2)	size2 = val->size2;
					break;
			default:
				_getfunc1(val, 0, buf);		// scalar value
				break;
		}
		idx1 = *i1Arr;
		idx2 = *i2Arr;
		for(i=0; i < size1; i++) {
			switch(ProcInf.i1type) {
				case idxArray :			idx1 = i1Arr[i];	break;
				case idxIncremental :	idx1 = *i1Arr + i;	break;
				case idxDecremental :	idx1 = *i1Arr - i;	break;
			}
			for (j=0; j < size2; j++) {
				switch(ProcInf.i2type) {
				case idxArray :			idx2 = i2Arr[j];	break;
				case idxIncremental :	idx2 = *i2Arr + j;	break;
				case idxDecremental :	idx2 = *i2Arr - j;	break;
				}
				switch(val->nDims) {
					case 1:	_getfunc1(val, j,    buf);	break;
					case 2: _getfunc2(val, i, j, buf);	break;
				}
				_setfunc(arr, idx1, idx2, buf);
			}
		}
	}
}
