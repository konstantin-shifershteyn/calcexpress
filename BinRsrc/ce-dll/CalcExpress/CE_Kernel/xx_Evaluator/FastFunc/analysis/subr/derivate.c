/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ========================================================================
//	 derivate( x [,dt [,ic [,fc]]] )
// ========================================================================
//
// *** Performs Discrete Differentiation
//
MgErr	_derivate(
			_dbl_operand	*opdArr,
			int32			qty,
			_dbl_operand	*out,
			int32			*lvErr )
{
	_dbl_operand	*arg;
	float64			*x = opdArr->array;
	float64			k, dt=1.0, ic=0.0, fc=0.0;
	int32			i, size = 0;

static	uInt32	types[4]  = { OPTYPE_A1DBL, OPTYPE_DBL, OPTYPE_DBL, OPTYPE_DBL };
static	char	**Msgs[4] = {
				& err_1ArgType_A1DBL,
				& err_2ArgType_DBL,
				& err_3ArgType_DBL,
				& err_4ArgType_DBL };

	for(i=0, arg = opdArr; i<qty; i++, arg++) {
		if (arg->ResType != types[i]) {
			out->erMsg = *(Msgs[i]);	goto out;
		}
		switch(i) {
			case 0:	x = arg->array;	size = arg->size1;
				break;
			case 1:	dt= arg->scalar; break;
			case 2:	ic= arg->scalar; break;
			case 3:	fc= arg->scalar; break;

		}
	}

	if (!size) {
		*lvErr = LV_SamplesGTZeroErr;		//lvErrorCode
		goto out;
	}
	if (dt <= 0)
		*lvErr = LV_DtGTZeroErr;

	// Computation of derivative
	dt = 1/(2*dt);
	for ( ; --size; x++) {
		k = *x;
		*x = (*(x+1) - ic) * dt;
		ic = k;
	}
	*x = (fc - ic) * dt;
out:
	out->ResType	= OPTYPE_A1DBL;
	return	noErr;
}
