/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ================================================================
// The function assigns values to indexed elements within 1D Arrays
// ================================================================
void	_set_idxEl_1d_arr(_gen_operand *arr, _indexes_info *idxs, _gen_operand *val)
{
	typedef void (*_idx1dfunc)();	// this typedef defines a generic pointer to a datatype-specific indexing function

	int32		i, idx;
	int32		size;
	_idx_info	*ip;				// a pointer to an index descriptor
	int32		*idxArr;			// a pointer to an index array

	_gen_operand	res;
	_idx_HowToProc	ProcInf;

	_idx1dfunc	_getfunc;			// a pointer to the datatype-specific indexing function
	_idx1dfunc	_setfunc;			// a pointer to the datatype-specific indexed assignment function
	char		buf[2*sizeof(float64)];
	//-----------

	if (arr->nDims != 1) {
		arr->erMsg = "Internal Error: incorrect call of the function (not 1D array) in "__FUNCTION__"()";
		return;
	}

	ip	= &(idxs->idx1);
	idxArr	= ip->values;

	// settings to initialize the result
	// ---------------------------------
	ProcInf.compound	= FALSE;
	ProcInf.idxAmount	= 1;
	ProcInf.allocArray	= FALSE;

	// ***** prepare the result structure
	// ==================================
	_index_a1_init_res(arr, ip, &res, &ProcInf);
	if (res.erMsg) {
		arr->erMsg = res.erMsg;
		return;
	}
	//---
	if (res.nDims < val->nDims) {
		arr->erMsg = "Incompatible operand dimensions in assignment";
		return;
	}

	// dependencies on the array's element datatype
	// ============================================
	switch (_operand_get_BaseType(arr->ResType)) {
		case OPTYPE_DBL :	_setfunc = set_element_of_A1DBL;
							_getfunc = get_element_of_A1DBL;
							break;
		case OPTYPE_CDB :	_setfunc = set_element_of_A1CDB;
							_getfunc = get_element_of_A1CDB;
							break;
		case OPTYPE_STR:	_setfunc = set_element_of_A1STR;
							_getfunc = get_element_of_A1STR;
							break;
		default:	// just a stub, it can't happen
			arr->erMsg = "Internal Error: unexpected BaseType in "__FUNCTION__"()";
			return;
	}

	// ================================
	// *****    ASSIGN VALUES     *****
	// ================================

	switch(res.nDims) {

	case 0:
		// *** Result is a scalar
		// ======================
		_getfunc(val, 0, buf);				// get a source value
		_setfunc(arr, *idxArr, buf);		// save to the target
		break;
		// ----------------

	case 1:
		// *** Result is an 1D Array
		// =========================
		size = res.size1;
		if (val->nDims) {
			if (val->size1 < size)
				size = val->size1;
		}
		else
			_getfunc(val, 0, buf);			// scalar value
		idx = *idxArr;
		for (i=0; i < size; i++) {
			switch(ProcInf.i1type) {
				case idxArray :			idx = idxArr[i];	break;
				case idxIncremental :	idx = *idxArr + i;	break;
				case idxDecremental :	idx = *idxArr - i;	break;
			}
			if (val->nDims)
				_getfunc(val, i, buf);
			_setfunc(arr, idx, buf);
		}
		break;

	default :
		arr->erMsg = "Internal Error: subscript dimensions are out of range in "__FUNCTION__"()";
		break;
	}
}
