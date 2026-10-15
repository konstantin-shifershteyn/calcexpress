/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CDB_FUNC__
#define	__CDB_FUNC__

// === Complex Number ===
typedef	struct {
	double	re;
	double	im;
}_cdb_;


#ifdef __cplusplus
extern "C" {
#endif

// =========================================================
// Function prototypes for low-level complex number handlers
// =========================================================

int add_cdb	( _cdb_ *x, _cdb_ *y, _cdb_ *out);
int mul_cdb	( _cdb_ *x, _cdb_ *y, _cdb_ *out);
int sub_cdb	( _cdb_ *x, _cdb_ *y, _cdb_ *out);
int div_cdb	( _cdb_ *x, _cdb_ *y, _cdb_ *out);

int rcpl_cdb	( _cdb_ *x, _cdb_ *out);
int sqrt_cdb	( _cdb_ *x, _cdb_ *out);

int exp_cdb	( _cdb_ *x, _cdb_ *out);
int pow2_cdb	( _cdb_ *x, _cdb_ *out);
int pow10_cdb	( _cdb_ *x, _cdb_ *out);
int pow_cdb	( _cdb_ *x, _cdb_ *y, _cdb_ *out);

int ln_cdb	( _cdb_ *x, _cdb_ *out);
int log2_cdb	( _cdb_ *x, _cdb_ *out);
int log10_cdb	( _cdb_ *x, _cdb_ *out);
int log_cdb	( _cdb_ *x, _cdb_ *y, _cdb_ *out);

//--------
int sin_cdb	( _cdb_ *x, _cdb_ *out);
int cos_cdb	( _cdb_ *x, _cdb_ *out);
int tan_cdb	( _cdb_ *x, _cdb_ *out);
int sec_cdb	( _cdb_ *x, _cdb_ *out);
int csc_cdb	( _cdb_ *x, _cdb_ *out);
int cot_cdb	( _cdb_ *x, _cdb_ *out);
//--------
int sinh_cdb	( _cdb_ *x, _cdb_ *out);
int cosh_cdb	( _cdb_ *x, _cdb_ *out);
int tanh_cdb	( _cdb_ *x, _cdb_ *out);
int sech_cdb	( _cdb_ *x, _cdb_ *out);
int csch_cdb	( _cdb_ *x, _cdb_ *out);
int coth_cdb	( _cdb_ *x, _cdb_ *out);
//--------
int asin_cdb	( _cdb_ *x, _cdb_ *out);
int acos_cdb	( _cdb_ *x, _cdb_ *out);
int atan_cdb	( _cdb_ *x, _cdb_ *out);
//--------
int asinh_cdb	( _cdb_ *x, _cdb_ *out);
int acosh_cdb	( _cdb_ *x, _cdb_ *out);
int atanh_cdb	( _cdb_ *x, _cdb_ *out);
//--------
int sinc_cdb	( _cdb_ *x, _cdb_ *out);
int atan2_cdb	( _cdb_ *x, _cdb_ *y, _cdb_ *out);



#ifdef __cplusplus
}
#endif

#endif	/*__CDB_FUNC__ */
