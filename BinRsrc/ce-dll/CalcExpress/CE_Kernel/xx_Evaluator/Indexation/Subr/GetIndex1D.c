/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


 // ==============================================================
 // The function reads values of indexed elements within 1D Arrays
 // ==============================================================
MgErr	_index_1d_arr(	_gen_operand	*opd,
						_indexes_info	*idxs,
						Bool32			onlyDblScalar	// >>> #CE_3.0 2025-04-22
						)
{
	typedef void (*_idx1dfunc)();	// this typedef defines a generic pointer to a datatype-specific indexing function

	int32			i, idx, size, elSize;
	_idx_info		*iip;			// a pointer to an index descriptor
	int32			*idxArr;		// a pointer to an index array
	void			*rsArr;			// a pointer to the resulting data array
	_gen_operand	res;			// result structure
	_idx_HowToProc	ProcInf;
	_idx1dfunc		_getfunc;		// a pointer to the datatype-specific indexing function
	MgErr			err = noErr;
	//-----------

	if (opd->nDims != 1) {
		opd->erMsg = "Internal Error: incorrect call of the function (not 1D array) in "__FUNCTION__"()";
		return err;
	}

	iip	= &(idxs->idx1);

	// settings to initialize the result
	// ---------------------------------
	ProcInf.compound	= FALSE;
	ProcInf.idxAmount	= 1;
//	ProcInf.allocArray	= TRUE;
	ProcInf.allocArray	= ! onlyDblScalar;	// >>> #CE_3.0 2025-04-22

	// ***** prepare the result structure
	// ==================================
	if (err = _index_a1_init_res(opd, iip, &res, &ProcInf))
		goto out;
	if (res.erMsg)
		goto out;

	// dependencies on the array's element datatype
	// ============================================
	switch (_operand_get_BaseType(opd->ResType)) {
		case OPTYPE_DBL :	elSize   = sizeof(float64);
							_getfunc = get_element_of_A1DBL;
							break;
		case OPTYPE_CDB :	elSize   = sizeof(float64) * 2;
							_getfunc = get_element_of_A1CDB;
							break;
		case OPTYPE_STR:	elSize   = sizeof(LStr*);
							_getfunc = get_element_of_A1STR;
							break;
		default:	// just a stub, it can't happen
			opd->erMsg = "Internal Error: unexpected BaseType in "__FUNCTION__"()";
			goto out;
	}

	// ==================================
	// =====   Perform Indexation   =====
	// ==================================
	idxArr	= iip->values;
	rsArr	= res.array;	// is NULL if output buffer was not allocated

	switch(res.nDims)
	{
	case 0:
		// *** Result is a scalar
		// ======================
		if (res.ResType == OPTYPE_DBL) {
			_getfunc(opd, *idxArr, &(opd->scalar));
			opd->ResType = OPTYPE_DBL;
			opd->nDims   = 0;
			opd->size1   = opd->size2 = 1;
			return err;
		}
		if (!rsArr)	break;	// >>> #CE_3.0 2025-04-22
		_getfunc(opd, *idxArr, rsArr);
		break;
		// ---------------------

	case 1:
		// *** Result is an 1D Array
		// =========================
		if (!rsArr)	break;	// >>> #CE_3.0 2025-04-22
		size = res.size1;
		idx  = *idxArr;
		for (i=0; i<size; i++) {
			switch(ProcInf.i1type) {
				case idxArray :			idx = idxArr[i];	break;
				case idxIncremental :	idx = *idxArr + i;	break;
				case idxDecremental :	idx = *idxArr - i;	break;
			}
			_getfunc(opd, idx, rsArr);
			(char*)rsArr += elSize;
		}
		break;

	default :
		res.erMsg = "Internal Error: subscript dimensions are out of range in "__FUNCTION__"()";
		break;
	}
	// *** DONE
out:
	if (opd->alloc)
		err = DSDisposePtr(opd->array);
	*opd = res;
	return err;
}
