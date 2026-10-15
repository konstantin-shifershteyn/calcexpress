/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

// ====================================================================
//	transp(Arr2D)
// ====================================================================
MgErr _arr_transpose(_dbl_operand *nop, _str_operand *sop)
{
	int32	elSize, size1,size2;
	void	*pnt;

	if (!sop && !nop)	return noErr;

	if (sop) {
		if (sop->ResType != OPTYPE_A2STR) {
			sop->erMsg = err_InvArgType_needArr2D;	return noErr;
		}
		pnt	= sop->array;
		elSize	= sizeof(LStr*);
		size1	= sop->size1;
		size2	= sop->size2;
		// swap dimensions
		sop->size1	= size2;
		sop->size2	= size1;
	}
	else {
		pnt	= nop->array;
		switch (nop->ResType) {
			case OPTYPE_A1DBL :
			case OPTYPE_A1CDB :	nop->erMsg = err_InvArgType_needArr2D;
						return noErr;

			case OPTYPE_A2DBL :	elSize =   sizeof(float64);	break;
			case OPTYPE_A2CDB :	elSize = 2*sizeof(float64);	break;
			default	:	return noErr;
		}
		size1	= nop->size1;
		size2	= nop->size2;
		// swap dimensions
		nop->size1	= size2;
		nop->size2	= size1;
	}
	return Transpose2D(pnt, size1, size2, elSize);
}

// ============================
//	rever2r(Arr2D)
// ============================
MgErr _arr_rever2r(_dbl_operand *nop, _str_operand *sop)
{
	int32	rows, elSize;
	void	*pnt;

	if (!sop && !nop)	return noErr;

	if (sop) {
		if (sop->nDims != 2) {
			sop->erMsg = err_InvArgType_needArr2D;	goto out;
		}
		pnt	= sop->array;
		rows	= sop->size1;
		elSize	= sizeof(LStr*) * sop->size2;	// whole row
	}
	else {
		if (nop->nDims != 2) {
			nop->erMsg = err_InvArgType_needArr2D;	goto out;
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
	ReverseBlock(pnt, rows, elSize);
out:
	return noErr;
}

// ============================
//	rever2c(Arr2D)
// ============================
MgErr _arr_rever2c(_dbl_operand *nop, _str_operand *sop)
{
	int32	i, rows, cols, elSize;
	void	*pnt;

	if (!sop && !nop)	return noErr;

	if (sop) {
		if (sop->nDims != 2) {
			sop->erMsg = err_InvArgType_needArr2D;	goto out;
		}
		pnt	= sop->array;
		elSize	= sizeof(LStr*);
		rows	= sop->size1;
		cols	= sop->size2;
	}
	else {
		if (nop->nDims != 2) {
			nop->erMsg = err_InvArgType_needArr2D;	goto out;
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
	for (i=0; i<rows; i++)
		ReverseBlock((char*)pnt + i*cols*elSize, cols, elSize);
out:
	return noErr;
}

