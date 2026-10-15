/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ============================
//	rotate2r(Arr2D, n)
// ============================
MgErr _arr_rotate2r(_dbl_operand *nop, _str_operand *sop, float64 n)
{
	int32	rows, elSize;
	void	*pnt;

	if (!sop && !nop)	return noErr;

	if (sop) {
		if (sop->nDims != 2) {
			sop->erMsg = err_InvArgType1_needArr2D;	return noErr;
		}
		pnt	= sop->array;
		rows	= sop->size1;
		elSize	= sizeof(LStr*) * sop->size2;	// whole row
	}
	else {
		if (nop->nDims != 2) {
			nop->erMsg = err_InvArgType1_needArr2D;	return noErr;
		}
		switch (nop->ResType) {
			case OPTYPE_A2DBL : elSize = sizeof(float64);	break;
			case OPTYPE_A2CDB : elSize = 2*sizeof(float64);	break;
			default	:	return noErr;
		}
		pnt	= nop->array;
		rows	= nop->size1;
		elSize *= nop->size2;		// whole row
	}
	return	RotateBlock(pnt, rows, elSize, _DblToI32(n));
}

// ============================
//	rotate2c(Arr2D, n)
// ============================
MgErr _arr_rotate2c(_dbl_operand *nop, _str_operand *sop, float64 n)
{
	int32	i, rows, cols, elSize;
	void	*pnt;
	MgErr	err = noErr;

	if (!sop && !nop)	return noErr;

	if (sop) {
		if (sop->nDims != 2) {
			sop->erMsg = err_InvArgType1_needArr2D;	goto out;
		}
		pnt	= sop->array;
		elSize	= sizeof(LStr*);
		rows	= sop->size1;
		cols	= sop->size2;
	}
	else {
		if (nop->nDims != 2) {
			nop->erMsg = err_InvArgType1_needArr2D;	goto out;
		}
		switch (nop->ResType) {
			case OPTYPE_A2DBL : elSize = sizeof(float64);	break;
			case OPTYPE_A2CDB : elSize = 2*sizeof(float64);	break;
			default	:	return noErr;
		}
		pnt	= nop->array;
		rows	= nop->size1;
		cols	= nop->size2;
	}
	for (i=0; i<rows; i++) {
		err = RotateBlock((char*)pnt + i*cols*elSize, cols, elSize, _DblToI32(n));
		if (err) break;
	}
out:
	return err;
}