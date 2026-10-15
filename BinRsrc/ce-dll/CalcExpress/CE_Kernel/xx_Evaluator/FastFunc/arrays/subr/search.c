/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


int32 _arr_search_cmp_dbl(float64 *a, float64 *b);
int32 _arr_search_cmp_cdb(float64 *a, float64 *b);
int32 _arr_search_cmp_str(LStr   **a, LStr   **b);	//case sensitive
int32 _arr_search_CMP_STR(LStr   **a, LStr   **b);	//case insensitive

// =========================================
//	search(arr, key [,idx [,cs]] )
// =========================================
MgErr	_search_arr(	_gen_operand	*opdArr,
						int32			qty,
						_gen_operand	*out )
{
	typedef	int32	(*_Cmp_Func_)(void*, void*);

	_gen_operand	*arr, *key;
	int32			i, elSize, size, idx=0;
	char			*pnt;
	_Cmp_Func_		_cmpFunc;
	MgErr			err = noErr;

	// 
	out->ResType	= OPTYPE_DBL;
	out->scalar	= -1;	// default value if search failed

	arr = opdArr++;
	key = opdArr++;

	// *** test 1st argument type
	// --------------------------
	switch(arr->ResType) {
		case OPTYPE_A1DBL :
			elSize = sizeof(float64);	_cmpFunc = _arr_search_cmp_dbl;	break;
		case OPTYPE_A1CDB :
			elSize = sizeof(float64) * 2;	_cmpFunc = _arr_search_cmp_cdb;	break;
		case OPTYPE_A1STR :
			elSize = sizeof(LStr*);		_cmpFunc = _arr_search_cmp_str;	break;
		default:
			out->erMsg = err_InvArgType_needArr1D;	goto out;
	}

	// *** test 2nd argument type
	// --------------------------
	if (key->nDims) {
		out->erMsg = err_InvArgType2_cantArr;	goto out;
	}

	// *** test argument type compatibility
	if (	_operand_get_BaseType(arr->ResType) !=
		_operand_get_BaseType(key->ResType) ) {
		out->erMsg = err_IncompatibleArgTypes;	goto out;
	}

	// *** GET OPTIONAL ARGUMENTS
	// ==========================
	if (qty >= 3) {
		// *** get index (if present)
		if (opdArr->ResType != OPTYPE_DBL) {
			out->erMsg = err_InvArgType3_needScal;	goto out;
		}
		idx = _DblToI32(opdArr->scalar);

		opdArr++;

		if (qty >=4) {
			// *** get CaseSensitivity (if present)
			if (opdArr->ResType != OPTYPE_DBL) {
				out->erMsg = err_InvArgType4_needScal;	goto out;
			}
			if (_DblToI32(opdArr->scalar))
				if (arr->ResType == OPTYPE_A1STR)
					_cmpFunc = _arr_search_CMP_STR;
		}
	}

	// =======================
	// *** looking for key ***
	// =======================
	if (idx < 0)
		goto out;

	pnt  = arr->array;
	size = arr->size1;

	for (i=idx; i < size; i++) {
		if (_cmpFunc(pnt + (i*elSize), key->array)) {
			out->scalar = i;
			break;
		}
	}
out:	return err;
}

// ======================================

// *** функция сравнения DBL для search()
int32 _arr_search_cmp_dbl(float64 *a, float64 *b)
{
	return	(*a == *b);
}
// *** функция сравнения CDB для search()
int32 _arr_search_cmp_cdb(float64 *a, float64 *b)
{
	if (*a++ == *b++)
		return (*a == *b);
	return 0;
}
// *** функция сравнения strings для search() : Case Sensitive
int32 _arr_search_cmp_str(LStr **a, LStr **b)
{
	return !LStrCmp(*a, *b);
}
// *** функция сравнения strings для search() : Case Insensitive
int32 _arr_search_CMP_STR(LStr **a, LStr **b)
{
	if (LStrLen(*a) != LStrLen(*b))
		return 0;
	return !StrNCaseCmp( LStrBuf(*a), LStrBuf(*b), LStrLen(*a));
}
