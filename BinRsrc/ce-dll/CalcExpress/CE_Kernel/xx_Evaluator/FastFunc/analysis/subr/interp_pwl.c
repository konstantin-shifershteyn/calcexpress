/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

void __sort_2_dbl_arrays(float64 *xa, float64 *ya, int32 size);

// ========================================================================
//	 interpl( x, xStamp, yStamp [,sort] )
// ========================================================================
//
// *** Performs Piecewise(?) Linear Interpolation or Extrapolation
//
MgErr _interpl(	_dbl_operand	*opdArr,
				int32			qty,
				_dbl_operand	*out,
				int32			*lvErr )
{
	_dbl_operand	*arg;
	float64			x, y = 0;
	float64			*sxPnt, *syPnt;
	int32			i, size;
	Bool32			sort;

	arg = opdArr;

	x	= arg->scalar;
	sxPnt	= (arg+1)->array;
	syPnt	= (arg+2)->array;

	size	= (arg+1)->size1;
	if (size != (arg+2)->size1) {
		size	= __min( size, (arg+2)->size1);
		*lvErr	= LV_EqSamplesErr;
	}
	if (qty < 4)
		sort = FALSE;
	else {
		sort = (Bool32)(arg->scalar);
		if ((arg+3)->ResType != OPTYPE_DBL)
			out->erMsg = err_4ArgType_DBL;
	}
	if ((arg+2)->ResType != OPTYPE_A1DBL)	out->erMsg = err_3ArgType_A1DBL;
	if ((arg+1)->ResType != OPTYPE_A1DBL)	out->erMsg = err_2ArgType_A1DBL;
	if ( arg->ResType    != OPTYPE_DBL)	out->erMsg = err_1ArgType_DBL;
	if (out->erMsg)
		goto out;

	if (size < 2) {
		y = x;
		*lvErr = LV_SamplesGETwoErr;	// lvErrorCode
		goto out;
	}

	if (sort) {
		__sort_2_dbl_arrays(sxPnt, syPnt, size);
	}

#define  SH	(sxPnt[i])
#define  SL	(sxPnt[i-1])
#define  TH	(syPnt[i])
#define  TL	(syPnt[i-1])

	for(i=0; i<size; i++)
		if (x <= SH) break;	// (x <= sxPnt[i])
	if (i == 0)	i = 1;
	if (i >= size)	i = size-1;

	y = (x-SL)/(SH-SL)*(TH-TL)+TL;

#undef	SH
#undef	SL
#undef	TH
#undef	TL

out:
	out->ResType	= OPTYPE_DBL;
	out->scalar	= y;
	return	noErr;
}

// ==========================
void __sort_2_dbl_arrays(	float64 *xa,
							float64 *ya,
							int32	size )
{
	int32	i;
	Bool32	flag;

	do {
		for(flag=FALSE, i=1; i<size; i++) {
			if (xa[i-1] > xa[i]) {
				flag = TRUE;
				SwapBlock( &(xa[i-1]), &(xa[i]), sizeof(float64) );
				SwapBlock( &(ya[i-1]), &(ya[i]), sizeof(float64) );
			}
		}
		size -= 1;
	} while(flag);
}
