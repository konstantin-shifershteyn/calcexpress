/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#define	ARR_SIZE_TOTAL	0
#define	ARR_SIZE_ROWS	1
#define	ARR_SIZE_COLS	2

// ===============================
// 	Function prototypes
// ===============================
// >>> #CE_3.0.2 2026-06-13 : 
/*	DEPRECATED
_VM_EXT_FUNC	vme_func_size;
*/

int32	_get_obj_size	(_OperandBase *arg, int32 dim);
MgErr	_size_common	(_CE_Instance *ip, LVBoolean *err_f, int32 _NDIM_);

// =======================================================
// General Macro defines FastFuncs of array size functions
// =======================================================
#define	_SizeFunc_(_fname_, _ndim_)		\
MgErr _fname_(	_CE_Instance	*ip,	\
				_FastFuncInfo	*fInfo,	\
				int32		arg_qty,	\
				LVBoolean	*err_f)		\
{ return _size_common(ip, err_f, _ndim_); }


_SizeFunc_(	f_arr_size,	ARR_SIZE_TOTAL	);
_SizeFunc_(	f_arr_rows,	ARR_SIZE_ROWS	);
_SizeFunc_(	f_arr_cols,	ARR_SIZE_COLS	);


// ============================================================
// 	General array size function
// ============================================================
MgErr	_size_common(	_CE_Instance	*ip,
						LVBoolean		*err_f,
						int32			subset )	// total, rows, or columns
{
	int32		size;
	_Operand	*arg;
	_VarInfo*	vip;
	MgErr		err = noErr;

//	arg = &((*(ip->OpdStk))->data[ip->OpdNum]);	// указатель на аргумент
	arg = OpdStack_EPnt(ip, ip->OpdNum);	// >>> #CE_3.0.2 2026-06-13 : указатель на аргумент

	size = subset;		// !!!			// !!!

	// Определение значения переменной
	if (arg->type == OPTYPE_IDENT) {

		if (!IsOperandIndexed(arg)) {
			// ### if not indexed variable
			VARMAN_LOCK_SHARED();
			if (GetPointerToVariable2(ip, &vip, arg, err_f, NULL)) {		// >>> #CE_3.0.2 2026-06-19
				// If success
				size = _get_obj_size(vip, subset);
			}
			VARMAN_UNLOCK_SHARED();
			goto out;
		}
		// we need to get a value of the indexed variable
		err = GetVariable(ip, arg, (_OperandBase*)arg, err_f);
	}
	size = _get_obj_size((_OperandBase*)arg, size);
out:
	arg->type = OPTYPE_DBL;
	arg->num  = size;
	arg->vpnt = NULL;			// >>> #CE_3.0.2 2026-06-19
	ClearOperandIndex(arg);		// в принципе не обязательно ???
	return  err;
}

// ===================================================
// Функция возвращает кол-во элементов в объекте
// subset == ARR_SIZE_TOTAL : total amount of elements
// subset == ARR_SIZE_ROWS  : number of roes
// subset == ARR_SIZE_COLS  : number of columns
// ===================================================
int32	_get_obj_size(_OperandBase *arg, int32 subset)
{
	int32	*pnt = (int32*)LStrBuf(*(arg->str));

	switch(_operand_get_nDims(arg->type)) {
		case 0 : return	1;					//size1=size2=1
		case 1 : if (subset == ARR_SIZE_ROWS)	return 1;	//size2=1
			 return *pnt;					//size1
		case 2 : if (subset == ARR_SIZE_ROWS)	return *pnt;	//size1
			 if (subset == ARR_SIZE_COLS)	return *(pnt+1);//size2
			 return  *pnt * *(pnt+1);			//size1 * size2
	}
	return	LStrLen(*(arg->str));	// total bytes of unknown object type
}

// >>> #CE_3.0.2 2026-06-13 : 
/*	DEPRECATED
// ========================================================================
//	VarMan Extension Function
//	it's intended to obtain number of elements in a variable
// ========================================================================
MgErr vme_func_size(	LStrHandle	name,		// name of variable
			_VarInfo	*vp_int,	//_VarInfo inside VarMan
			_VarInfo	*vp_ext,	//_VarInfo outside VarMan
			int32		*ArrSize,	// put size to this
			LVBoolean	*err_f,
			uInt16		*err_c )
{
	// ArrSize have to contain dimension number

	if (*err_f)	*ArrSize = 0;
	else		*ArrSize = _get_obj_size( vp_int, *ArrSize);
	return noErr;
}
*/

#undef	ARR_SIZE_TOTAL
#undef	ARR_SIZE_ROWS
#undef	ARR_SIZE_COLS
