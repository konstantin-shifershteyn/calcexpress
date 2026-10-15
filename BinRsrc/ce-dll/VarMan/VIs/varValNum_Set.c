/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

MgErr vmExtFunc_setval(	
			LStrHandle	name,		// name of variable
			_VarInfo	*vp_int,	//_VarInfo inside VarMan
			_VarInfo	*vp_ext,	//_VarInfo outside VarMan
			void		*extdata,	// external data
			LVBoolean	*err_f,
			uInt16		*err_c
			)
{
	vmcin_extdata	*vpar;
	int32			nDims, nDbls;
	int32			size, size1, size2;
	int32			*vmpnt, *vipnt;
	uInt32			slen;
	MgErr			err;

	vpar = (vmcin_extdata*)extdata;

	if (*err_f)	return noErr;

// ѕри SETVAL проверку типа выполн€ет VarMan
// ѕри CREATE проверку типа выполн€ть не надо
//	if (vp_int->type != vpar->type)	//поверка соответстви€ типов
//		goto invalid_type;

	switch (vpar->type) {
		case OPTYPE_A1DBL : nDims = 1; nDbls = 1; break;
		case OPTYPE_A2DBL : nDims = 2; nDbls = 1; break;
		case OPTYPE_CDB   : nDims = 0; nDbls = 2; break;
		case OPTYPE_A1CDB : nDims = 1; nDbls = 2; break;
		case OPTYPE_A2CDB : nDims = 2; nDbls = 2; break;
		default:	*err_f = LVTRUE;
					*err_c = VM_ERR_VAR_INVTYPE;
					return noErr;
	}
	switch (nDims) {
		case 2:	vipnt=(int32*)(*vpar->val); size1= *vipnt++; size2= *vipnt++;
				break;
		case 1:	vipnt=(int32*)(*vpar->val); size1= *vipnt++; size2= 1;
// >>> #CE_3.0 2024-12-13
#if defined(_M_X64)
				vipnt++;	// skip padding bytes
#endif
// <<< #CE_3.0 2024-12-13
				break;
		case 0:	vipnt=(int32*)vpar->val;    size1= size2 = 1;
				break;
	}
	size = size1 * size2;

	// ### Copy data (stored enclosed into LVString)
	// =============================================
	slen = nDims * sizeof(int32) + size * nDbls * sizeof(float64);		// container string length

// >>> #CE_3.0.4 2026-07-06
//	if (err = LStr_SetSizeOrCreate(&(vp_int->str), slen) )
//		return err;

	// we create an array (of float64 values) enclosed into LVString
	// the idea is to align array data to 8 bytes (sizeof(float64))
	if (err = ce_CreateOrResizeAlignedArray(
					(UHandle*)&(vp_int->str),	// pointer to LStrHandle
					size * nDbls,				// number of elements in the array
					sizeof(float64),			// element size (bytes)
					sizeof(float64),			// alignment (the array keeps float64 values)
					nDims * sizeof(int32) + sizeof(int32)))	// nDims * int32 are array's dimentions AND one int32 is LVString Length
	{
		return err;
	}
	LStrLen(*(vp_int->str)) = slen;		// explicitly set LVString Length (ce_CreateOrResizeAlignedArray creates 1D Array and sets dimSize/cnt to array size)
// <<< #CE_3.0.4 2026-07-06

	vmpnt = (int32*)LStrBuf(*(vp_int->str));	// pointer to array inside VarMan
	switch (nDims) {							// save dimention(s)
		case 2 :	*vmpnt++ = size1; *vmpnt++ = size2;
					break;
		case 1 :	*vmpnt++ = size1;
// >>> #CE_3.0 2024-12-13
//#if defined(_M_X64)
//					vmpnt++;	// skip padding bytes. IT'S INTERNAL DATA, NO PADDING
//#endif
// <<< #CE_3.0 2024-12-13
					break;
	}
	_MoveBlock( vipnt, vmpnt, size * nDbls * sizeof(float64));	//copy data

	return noErr;
}
