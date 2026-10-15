/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ========================================================================
//	 decimate( x [,dcf [,ave]] )
// ========================================================================
//
// *** Decimates a 1D array
//
MgErr	_decimate(
			_dbl_operand	*opdArr,
			int32			qty,
			_dbl_operand	*out,
			int32			*lvErr )
{
		_dbl_operand	*arg;
		float64			*src = opdArr->array, *dst;
		int32			dcf=2, ave=FALSE;
		int32			i, size = 0, newSize;

static	uInt32	types[4]  = { OPTYPE_A1DBL, OPTYPE_DBL, OPTYPE_DBL };
static	char	**Msgs[4] = {	& err_1ArgType_A1DBL,
				& err_2ArgType_DBL,
				& err_3ArgType_DBL };


	out->ResType	= opdArr->ResType;
	out->scalar	= opdArr->scalar;

	for(i=0, arg = opdArr; i<qty; i++, arg++) {
		if (arg->ResType != types[i]) {
			out->erMsg = *(Msgs[i]);	goto out;
		}
		switch(i) {
			case 0:	src = arg->array; size = arg->size1;	break;
			case 1:	dcf = _DblToI32(arg->scalar);	break;
			case 2:	ave = _DblToI32(arg->scalar);	break;
		}
	}

	if (!size) {
		*lvErr = LV_SamplesGTZeroErr;		//lvErrorCode
		goto out;
	}
//	if ((dcf < 1) || (dcf > size)) {
//		*lvErr = LV_DecFactErr;
//		goto out;
//	}
//	newSize = size / dcf;
// >>> #CE_3.0 2025-06-25
	// in order to make the function return an empty array if wrong 'dcf' or size of input error
	if ((dcf < 1) || (dcf > size)) {
		*lvErr = LV_DecFactErr;
		newSize = 0;
	}
	else {
		newSize = size / dcf;
	}
// <<< #CE_3.0 2025-06-25

	out->ResType	= OPTYPE_A1DBL;
	out->nDims	= 1;
	out->size1	= newSize;
	out->size2	= 1;
//	out->array	= dst = ce_DSNewPtr(size * sizeof(float64));
	out->array	= dst = (float64*)ce_DSNewPtr(max(1, newSize) * sizeof(float64));	// >>> #CE_3.0 2025-06-25
	out->alloc	= (out->array != NULL);
	if (!(out->alloc))
		return mFullErr;

	if (ave) {
		for(; newSize-- > 0; dst++) {
			for(i=dcf, *dst=0.0; i>0; i--)
				*dst += *src++;
			*dst /= (float64)dcf;
		}
	}
	else {
		for(; newSize-- > 0; dst++, src += dcf) {
			*dst = *src;
		}
	}
out:
	return	noErr;
}
