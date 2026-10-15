/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
 
#include <extcode.h>
#include <stdlib.h>


// 1D Array of DBL type Definition
typedef	struct {
	int32		size;
#if defined(_M_X64)
	int32		pad_1;		// >>> #CE_3.0 2024-12-04
#endif
	float64		val[1];
} _LVA1Dbl, **_hLVA1Dbl;

// 2D Array of DBL type Definition
typedef	struct {
	int32		size1;
	int32		size2;
	float64		val[1];
} _LVA2Dbl, **_hLVA2Dbl;


// Function calculates sum of outer product matrices of two vectors
void	sumOuterProduct(int32	n,
			float64	*v1,
			float64	*v2,
			float64	*mtx )
{
	register int32	i,j;

	for (i=0; i<n; i++)
		for (j=0; j<n; j++)
			*(mtx + i*n +j) += *(v1+i) * *(v2+j);
}

// ==============================================

MgErr __cdecl	lmfitabx (
			_hLVA2Dbl	in_VAR,
			_hLVA1Dbl	in_STD,
			_hLVA1Dbl	in_DIF,
			_hLVA2Dbl	out_Alpha,
			_hLVA1Dbl	out_Beta
		)
{

	float64		*VAR_pnt, *STD_pnt, *DIF_pnt, *Alpha_pnt, *Beta_pnt;
	float64		*ColBuf = NULL, *ColBufSub = NULL;
	float64		DIF_val,  STD_val;
	int32		DIF_size, STD_size;
	int32		nVariants, nPoints;
	int32		row, col;
	int32		err = noErr;


	// ***	Sizes
	nVariants = (*in_VAR)->size1;
	nPoints   = (*in_VAR)->size2;
	STD_size  = (*in_STD)->size;
	DIF_size  = (*in_DIF)->size;

	// ***	Initialize pointers
	VAR_pnt = &((*in_VAR)->val[0]);
	STD_pnt = &((*in_STD)->val[0]);
	DIF_pnt = &((*in_DIF)->val[0]);


	// ***	Allocate Alpha array
	if (err = NumericArrayResize( fD, 2, (UHandle*)&out_Alpha, nVariants * nVariants))
		goto out;
	(*out_Alpha)->size1 = (*out_Alpha)->size2 = nVariants;
	Alpha_pnt = &((*out_Alpha)->val[0]);
//	ClearMem( Alpha_pnt, sizeof(float64) * nVariants * nVariants);
	memset(Alpha_pnt, 0, sizeof(float64) * nVariants * nVariants);


	// ***	Allocate Beta array
	if (err = NumericArrayResize( fD, 1, (UHandle*)&out_Beta, nVariants))
		goto out;
	(*out_Beta)->size = nVariants;
	Beta_pnt = &((*out_Beta)->val[0]);
//	ClearMem( Beta_pnt, sizeof(float64) * nVariants);
	memset(Beta_pnt, 0, sizeof(float64) * nVariants);

	// ***	Allocate Buffers
	ColBuf		= (float64*)DSNewPClr(nVariants * sizeof(float64));
	ColBufSub	= (float64*)DSNewPClr(nVariants * sizeof(float64));
	if ( !ColBuf || !ColBufSub ) {
		err = mFullErr; goto out;
	}


	// ***	Computation loop
	for (col=0; col < nPoints; col++) {

		if (col < DIF_size)	DIF_val	= *(DIF_pnt + col);
		if (col < STD_size)	STD_val	= *(STD_pnt + col);

		for (row=0; row < nVariants; row++) {
			*(ColBuf + row)	= *(ColBufSub + row) = *(VAR_pnt + row * nPoints + col);
			if (col < STD_size)	*(ColBufSub + row) /= STD_val;
			if (col < DIF_size)	*(Beta_pnt  + row) += (*(ColBufSub + row) * DIF_val);
		}
		sumOuterProduct(nVariants, ColBuf, ColBufSub, Alpha_pnt);
	}
out:
	if (ColBuf)		DSDisposePtr( ColBuf );
	if (ColBufSub)	DSDisposePtr( ColBufSub );
	return noErr; 
} 

