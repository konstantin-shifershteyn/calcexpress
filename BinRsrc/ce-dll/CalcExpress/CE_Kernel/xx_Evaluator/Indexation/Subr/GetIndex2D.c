/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ==============================================================
// The function reads values of indexed elements within 2D Arrays
// ==============================================================
MgErr	_index_2d_arr(	_gen_operand	*opd,
						_indexes_info	*idxs,
						Bool32			onlyDblScalar	// >>> #CE_3.0 2025-04-22
						)
{
	// this typedef defines a generic pointer to a datatype-specific indexing function
	typedef void (*_idx2dfunc)();
	// this structure contains a stub index: nDims == 1 --> index range for entire dimension size
//	static	_idx_info	idxStub = {-1, 0, 0x7FFFFFFF, 0, &idxStub.scalar, FALSE, NULL };
	static	_idx_info	idxStub = {-1, 0, 0x7FFFFFFF, 0, &idxStub.scalar, FALSE, 0, NULL };		// >>> #CE_3.0.4 2026-07-04

	_idx_info		*iip1,	*iip2;		// pointers to index descriptors
	int32			*i1Arr, *i2Arr;		// pointers to index arrays
	_gen_operand	res;				// result structure
	void			*rsArr;				// a pointer to the resulting data array
	int32			i,j, size;
	int32			idx1, idx2, elSize;
	_idx_HowToProc	ProcInf;
	_idx2dfunc		_getfunc;			// a pointer to the datatype-specific indexing function
	MgErr			err = noErr;
	//--------------

	if (opd->nDims != 2) {
		opd->erMsg = "Internal Error: incorrect call of the function (not 2D array) in "__FUNCTION__"()";
		return err;
	}

	iip1 = &(idxs->idx1);
	if (idxs->qty < 2)	iip2 = &(idxStub);
	else				iip2 = &(idxs->idx2);

	// settings to initialize the result
	// ---------------------------------
	ProcInf.compound	= idxs->compound;
	ProcInf.idxAmount	= idxs->qty;
//	ProcInf.allocArray	= TRUE;
	ProcInf.allocArray	= ! onlyDblScalar;	// >>> #CE_3.0 2025-04-22

	// ***** prepare the result structure
	// ==================================
	if (err = _index_a2_init_res(opd, iip1, iip2, &res, &ProcInf))
		goto out;
	if (res.erMsg)
		goto out;

	// dependencies on the array's element datatype
	// ============================================
	switch (_operand_get_BaseType(opd->ResType)) {
		case OPTYPE_DBL :	elSize = sizeof(float64);
							_getfunc = get_element_of_A2DBL;
							break;
		case OPTYPE_CDB :	elSize = sizeof(float64) * 2;
							_getfunc = get_element_of_A2CDB;
							break;
		case OPTYPE_STR:	elSize = sizeof(LStr*);
							_getfunc = get_element_of_A2STR;
							break;
	}

	// ==================================
	// =====   Perform Indexation   =====
	// ==================================
	i1Arr	= iip1->values;
	i2Arr	= iip2->values;
	rsArr	= res.array;	// is NULL if output buffer was not allocated

	switch(res.nDims)
	{
	case 0:
		// *** Result is a scalar
		// ======================
		if (res.ResType == OPTYPE_DBL) {
			_getfunc(opd, *i1Arr, *i2Arr, &(opd->scalar));
			opd->ResType = OPTYPE_DBL;
			opd->nDims   = 0;
			opd->size1   = opd->size2 = 1;
			return err;
		}
		if (!rsArr)	break;	// >>> #CE_3.0 2025-04-22
		_getfunc(opd, *i1Arr, *i2Arr, rsArr);
		break;
		// ---------------------

	case 1:
		// *** Result is an 1D Array
		// =========================
		if (!rsArr)	break;	// >>> #CE_3.0 2025-04-22
		size = res.size1;	// * res.size2;
		idx1 = *i1Arr;
		idx2 = *i2Arr;
		for (i=0; i<size; i++) {
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
			_getfunc(opd, idx1, idx2, rsArr);
			(char*)rsArr += elSize;
		}
		break;
		// ---------------------

	case 2:
		// *** Result is a 2D Array
		// ========================
		if (!rsArr)	break;	// >>> #CE_3.0 2025-04-22
		idx1 = *i1Arr;
		idx2 = *i2Arr;
		for(i=0; i < res.size1; i++) {
			switch(ProcInf.i1type) {
			case idxArray :			idx1 = i1Arr[i];	break;
			case idxIncremental :	idx1 = *i1Arr + i;	break;
			case idxDecremental :	idx1 = *i1Arr - i;	break;
			}
			for (j=0; j < res.size2; j++) {
				switch(ProcInf.i2type) {
				case idxArray :			idx2 = i2Arr[j];	break;
				case idxIncremental :	idx2 = *i2Arr + j;	break;
				case idxDecremental :	idx2 = *i2Arr - j;	break;
				}
				_getfunc(opd, idx1, idx2, rsArr);
				(char*)rsArr += elSize;
			}
		}
		break;
	}
	// *** DONE
out:
	if (opd->alloc)
		err = DSDisposePtr(opd->array);
	*opd = res;
	return	err;
}
