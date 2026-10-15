/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <FastFuncList.h>

#include <float.h>
#include <math.h>

// =========================
// Массив описателей функций
// =========================
_FastFuncInfo	FuncList[] = {
//----------------------------------------------------------------------------------------
//		name	 min max	processor	exec_dbl	exec_cdb
//----------------------------------------------------------------------------------------

// -------- Арифметические функции --------
//_FastFuncDef( "neg"	, 1 , 1 ,	f_math_arg1	, _neg_dbl	, _neg_cdb	),
//_FastFuncDef( "add"	, 2 , 2 ,	f_math_arg2	, _add_dbl	, _add_cdb	),
//_FastFuncDef( "sub"	, 2 , 2 ,	f_math_arg2	, _sub_dbl	, _sub_cdb	),
//_FastFuncDef( "mul"	, 2 , 2 ,	f_math_arg2	, _mul_dbl	, _mul_cdb	),
//_FastFuncDef( "div"	, 2 , 2 ,	f_math_arg2	, _div_dbl	, _div_cdb	),
//_FastFuncDef( "mod"	, 2 , 2 ,	f_math_arg2	, _mod_dbl	, ___noCpx_a2	),
//_FastFuncDef( "rcpl"	, 1 , 1 ,	f_math_arg1	, _rcpl_dbl	, _rcpl_cdb	),

// -------- Тригонометрические Функции -----
_FastFuncDef( "sin"		, 1 , 1 ,	f_math_arg1	, _sin_dbl		, _sin_cdb	),
_FastFuncDef( "cos"		, 1 , 1 ,	f_math_arg1	, _cos_dbl		, _cos_cdb	),
_FastFuncDef( "tan"		, 1 , 1 ,	f_math_arg1	, _tan_dbl		, _tan_cdb	),
_FastFuncDef( "sec"		, 1 , 1 ,	f_math_arg1	, _sec_dbl		, _sec_cdb	),
_FastFuncDef( "csc"		, 1 , 1 ,	f_math_arg1	, _csc_dbl		, _csc_cdb	),
_FastFuncDef( "cot"		, 1 , 1 ,	f_math_arg1	, _cot_dbl		, _cot_cdb	),
//--
_FastFuncDef( "asin"	, 1 , 1 ,	f_math_arg1	, _asin_dbl		, _asin_cdb	),
_FastFuncDef( "acos"	, 1 , 1 ,	f_math_arg1	, _acos_dbl		, _acos_cdb	),
_FastFuncDef( "atan"	, 1 , 1 ,	f_math_arg1	, _atan_dbl		, _atan_cdb	),
_FastFuncDef( "asec"	, 1 , 1 ,	f_math_arg1	, _asec_dbl		, ___noCpx_a1	),
_FastFuncDef( "acsc"	, 1 , 1 ,	f_math_arg1	, _acsc_dbl		, ___noCpx_a1	),
_FastFuncDef( "acot"	, 1 , 1 ,	f_math_arg1	, _acot_dbl		, ___noCpx_a1	),
//--
_FastFuncDef( "sinh"	, 1 , 1 ,	f_math_arg1	, _sinh_dbl		, _sinh_cdb	),
_FastFuncDef( "cosh"	, 1 , 1 ,	f_math_arg1	, _cosh_dbl		, _cosh_cdb	),
_FastFuncDef( "tanh"	, 1 , 1 ,	f_math_arg1	, _tanh_dbl		, _tanh_cdb	),
_FastFuncDef( "sech"	, 1 , 1 ,	f_math_arg1	, _sech_dbl		, _sech_cdb	),
_FastFuncDef( "csch"	, 1 , 1 ,	f_math_arg1	, _csch_dbl		, _csch_cdb	),
_FastFuncDef( "coth"	, 1 , 1 ,	f_math_arg1	, _coth_dbl		, _coth_cdb	),
//--
_FastFuncDef( "asinh"	, 1 , 1 ,	f_math_arg1	, _asinh_dbl	, _asinh_cdb	),
_FastFuncDef( "acosh"	, 1 , 1 ,	f_math_arg1	, _acosh_dbl	, _acosh_cdb	),
_FastFuncDef( "atanh"	, 1 , 1 ,	f_math_arg1	, _atanh_dbl	, _atanh_cdb	),
_FastFuncDef( "asech"	, 1 , 1 ,	f_math_arg1	, _asech_dbl	, ___noCpx_a1	),
_FastFuncDef( "acsch"	, 1 , 1 ,	f_math_arg1	, _acsch_dbl	, ___noCpx_a1	),
_FastFuncDef( "acoth"	, 1 , 1 ,	f_math_arg1	, _acoth_dbl	, ___noCpx_a1	),
//--
_FastFuncDef( "sinc"	, 1 , 1 ,	f_math_arg1	, _sinc_dbl		, _sinc_cdb	),
_FastFuncDef( "atan2"	, 2 , 2 ,	f_math_arg2	, _atan2_dbl	, _atan2_cdb	),


// -------- Разные Математические функции --------
_FastFuncDef( "abs"		, 1 , 1 ,	f_math_arg1	, _abs_dbl		, _norm_cdb	),//для cdb аналог Norm()
_FastFuncDef( "sqrt"	, 1 , 1 ,	f_math_arg1	, _sqrt_dbl		, _sqrt_cdb	),
//--
_FastFuncDef( "deg"		, 1 , 1 ,	f_math_arg1	, _deg_dbl		, _deg_cdb	),
_FastFuncDef( "rad"		, 1 , 1 ,	f_math_arg1	, _rad_dbl		, _rad_cdb	),
//--
_FastFuncDef( "clip"	, 1 , 3 ,	f_math_argNx, _clip_dbl		, ___noCpx_aNx	),
_FastFuncDef( "sign"	, 1 , 1 ,	f_math_arg1	, _sign_dbl		, ___noCpx_a1	),
_FastFuncDef( "spike"	, 1 , 1 ,	f_math_arg1	, _spike_dbl	, ___noCpx_a1	),
_FastFuncDef( "square"	, 1 , 1 ,	f_math_arg1	, _square_dbl	, ___noCpx_a1	),
_FastFuncDef( "step"	, 1 , 1 ,	f_math_arg1	, _step_dbl		, ___noCpx_a1	),
//--
_FastFuncDef( "_finite"	, 1 , 1 ,	f_math_arg1	, _finite_dbl	, ___noCpx_a1	),
_FastFuncDef( "isfinite", 1 , 1 ,	f_math_arg1	, _finite_dbl	, ___noCpx_a1	),	// >>> #CE_3.1.0-r6 2026-09-24
_FastFuncDef( "_isnan"	, 1 , 1 ,	f_math_arg1	, _isnan_dbl	, ___noCpx_a1	),
_FastFuncDef( "isnan"	, 1 , 1 ,	f_math_arg1	, _isnan_dbl	, ___noCpx_a1	),	// >>> #CE_3.1.0-r6 2026-09-24
_FastFuncDef( "isinf"	, 1 , 1 ,	f_math_arg1	, _isinf_dbl	, ___noCpx_a1	),	// >>> #CE_3.1.0-r6 2026-09-24
_FastFuncDef( "_infton"	, 1 , 1 ,	f_math_arg1	, _infton_dbl	, ___noCpx_a1	),

// -------- Логарифмические функции --------
_FastFuncDef( "exp"		, 1 , 1 ,	f_math_arg1	, _exp_dbl		, _exp_cdb	),
_FastFuncDef( "pow"		, 2 , 2 ,	f_math_arg2	, _pow_dbl		, _pow_cdb	),
_FastFuncDef( "pow2"	, 1 , 1 ,	f_math_arg1	, _pow2_dbl		, _pow2_cdb	),
_FastFuncDef( "pow10"	, 1 , 1 ,	f_math_arg1	, _pow10_dbl	, _pow10_cdb	),
_FastFuncDef( "ln"		, 1 , 1 ,	f_math_arg1	, _ln_dbl		, _ln_cdb	),
_FastFuncDef( "log"		, 2 , 2 ,	f_math_arg2	, _log_dbl		, _log_cdb	),
_FastFuncDef( "log2"	, 1 , 1 ,	f_math_arg1	, _log2_dbl		, _log2_cdb	),
_FastFuncDef( "log10"	, 1 , 1 ,	f_math_arg1	, _log10_dbl	, _log10_cdb	),

// -------- Функции с необязательными аргументами -----
_FastFuncDef( "e"		, 0 , 1 ,	f_math_arg01	, _e_dbl	 , _e_cdb	),
_FastFuncDef( "pi"		, 0 , 1 ,	f_math_arg01	, _pi_dbl	 , _pi_cdb	),
_FastFuncDef( "random"	, 0 , 2 ,	f_math_arg02	, _random_dbl, ___noCpx_a2),

_FastFuncDef( "nan"		, 0 , 0 ,	f_nan			, NULL		, NULL	),			// >>> #CE_3.1.0-r6 2026-09-24
_FastFuncDef( "inf"		, 0 , 0 ,	f_inf			, NULL		, NULL	),			// >>> #CE_3.1.0-r6 2026-09-24
_FastFuncDef( "infn"	, 0 , 0 ,	f_infn			, NULL		, NULL	),			// >>> #CE_3.1.0-r6 2026-09-24

// -------- Функции чисел с плавающей точкой --------
_FastFuncDef( "int"		, 1 , 1 ,	f_math_arg1	, _int_dbl		, ___noCpx_a1	),
_FastFuncDef( "fract"	, 1 , 1 ,	f_math_arg1	, _fract_dbl	, ___noCpx_a1	),
_FastFuncDef( "floor"	, 1 , 1 ,	f_math_arg1	, _floor_dbl	, ___noCpx_a1	),
_FastFuncDef( "ceil"	, 1 , 1 ,	f_math_arg1	, _ceil_dbl		, ___noCpx_a1	),
_FastFuncDef( "getexp"	, 1 , 1 ,	f_math_arg1	, _getexp_dbl	, ___noCpx_a1	),
_FastFuncDef( "getman"	, 1 , 1 ,	f_math_arg1	, _getman_dbl	, ___noCpx_a1	),
_FastFuncDef( "ldexp"	, 2 , 2 ,	f_math_arg2	, _ldexp_dbl	, ___noCpx_a2	),

// -------- Функции целочисленной арифметики --------
_FastFuncDef( "i32s"	, 1 , 1 ,	f_math_arg1	, _i32s_dbl		, ___noCpx_a1	),	// >>> #CE_3.1.0-r6 2026-09-26
_FastFuncDef( "i32b"	, 1 , 1 ,	f_math_arg1	, _i32b_dbl		, ___noCpx_a1	),	// >>> #CE_3.1.0-r6 2026-09-26
_FastFuncDef( "u32s"	, 1 , 1 ,	f_math_arg1	, _u32s_dbl		, ___noCpx_a1	),	// >>> #CE_3.1.0-r6 2026-09-26
_FastFuncDef( "u32b"	, 1 , 1 ,	f_math_arg1	, _u32b_dbl		, ___noCpx_a1	),	// >>> #CE_3.1.0-r6 2026-09-26
_FastFuncDef( "u32mod"	, 2 , 2 ,	f_math_arg2	, _u32mod_dbl	, ___noCpx_a2	),
_FastFuncDef( "u32sub"	, 2 , 2 ,	f_math_arg2	, _u32sub_dbl	, ___noCpx_a2	),

// -------- Функции комплексных чисел ----------
_FastFuncDef( "complex"	, 1 , 2 ,	f_math_arg02	, _complex_cdb	, _complex_cdb	),
_FastFuncDef( "polar"	, 1 , 2 ,	f_math_arg02	, _polar_cdb	, _polar_cdb	),
_FastFuncDef( "real"	, 1 , 1 ,	f_math_arg1		, _real_cdb		, _real_cdb	),
_FastFuncDef( "imag"	, 1 , 1 ,	f_math_arg1		, _imag_cdb		, _imag_cdb	),
_FastFuncDef( "norm"	, 1 , 1 ,	f_math_arg1		, _norm_cdb		, _norm_cdb	),//для cdb аналог Abs()
_FastFuncDef( "arg"		, 1 , 1 ,	f_math_arg1		, _arg_cdb		, _arg_cdb	),
_FastFuncDef( "conj"	, 1 , 1 ,	f_math_arg1		, _conj_cdb		, _conj_cdb	),

// -------- Функции с переменным числом аргументов -----
_FastFuncDef( "sum"		, 1 , 0x7fff ,	f_math_argN	, _sum_dbl		, _sum_cdb	),
_FastFuncDef( "prod"	, 1 , 0x7fff ,	f_math_argN	, _prod_dbl		, _prod_cdb	),
_FastFuncDef( "mean"	, 1 , 0x7fff ,	f_math_argN	, _mean_dbl		, _mean_cdb	),
_FastFuncDef( "min"		, 1 , 0x7fff ,	f_math_argN	, _min_dbl		, ___noCpx_aN	),
_FastFuncDef( "max"		, 1 , 0x7fff ,	f_math_argN	, _max_dbl		, ___noCpx_aN	),
//--
_FastFuncDef( "land"	, 1 , 0x7fff ,	f_math_argN	, _landFunc_dbl	, ___noCpx_aN	),
_FastFuncDef( "lor"		, 1 , 0x7fff ,	f_math_argN	, _lorFunc_dbl	, ___noCpx_aN	),
_FastFuncDef( "lxor"	, 1 , 0x7fff ,	f_math_argN	, _lxorFunc_dbl	, ___noCpx_aN	),

// -------- Функции управления переменными -----
_FastFuncDef( "dbl"		, 1 , 3 ,	v_dbl		, NULL		, NULL		),
_FastFuncDef( "cdb"		, 1 , 3 ,	v_cdb		, NULL		, NULL		),
_FastFuncDef( "str"		, 1 , 3 ,	v_str		, NULL		, NULL		),
_FastFuncDef( "a1dbl"	, 1 , 3 ,	v_a1dbl		, NULL		, NULL		),
_FastFuncDef( "a1cdb"	, 1 , 3 ,	v_a1cdb		, NULL		, NULL		),
_FastFuncDef( "a1str"	, 1 , 3 ,	v_a1str		, NULL		, NULL		),
_FastFuncDef( "a2dbl"	, 1 , 3 ,	v_a2dbl		, NULL		, NULL		),
_FastFuncDef( "a2cdb"	, 1 , 3 ,	v_a2cdb		, NULL		, NULL		),
_FastFuncDef( "a2str"	, 1 , 3 ,	v_a2str		, NULL		, NULL		),
// -----
_FastFuncDef( "vclone"	, 2 , 4 ,	v_vclone	, NULL		, NULL		),
_FastFuncDef( "vcopy"	, 2 , 4 ,	v_vcopy		, NULL		, NULL		),
_FastFuncDef( "vset"	, 2 , 3 ,	v_vset		, NULL		, NULL		),
// -----
_FastFuncDef( "vundef"	, 1 , 2 ,	v_vundef	, NULL		, NULL		),
_FastFuncDef( "vtest"	, 1 , 2 ,	v_vtest		, NULL		, NULL		),
// ----- 
_FastFuncDef( "vtype"	, 1 , 2 ,	v_vtype		, NULL		, NULL		),		// >>> #CE_3.1.0-r6 2026-09-20
_FastFuncDef( "vstcid"	, 1 , 2 ,	v_vstcid	, NULL		, NULL		),		// >>> #CE_3.1.0-r6 2026-09-20
_FastFuncDef( "vstcms"	, 1 , 2 ,	v_vstcms	, NULL		, NULL		),		// >>> #CE_3.1.0-r6 2026-09-20
_FastFuncDef( "vstcts"	, 1 , 2 ,	v_vstcts	, NULL		, NULL		),		// >>> #CE_3.1.0-r6 2026-09-20
_FastFuncDef( "vstwid"	, 1 , 2 ,	v_vstwid	, NULL		, NULL		),		// >>> #CE_3.1.0-r6 2026-09-20
_FastFuncDef( "vstwms"	, 1 , 2 ,	v_vstwms	, NULL		, NULL		),		// >>> #CE_3.1.0-r6 2026-09-20
_FastFuncDef( "vstwts"	, 1 , 2 ,	v_vstwts	, NULL		, NULL		),		// >>> #CE_3.1.0-r6 2026-09-20
_FastFuncDef( "vstwcnt" , 1 , 2 ,	v_vstwcnt	, NULL		, NULL		),		// >>> #CE_3.1.0-r6 2026-09-20
// -----
_FastFuncDef( "vch"		, 0 , 1 ,	v_vch		, NULL		, NULL		),
_FastFuncDef( "vchtest"	, 0 , 1 ,	v_vchtest	, NULL		, NULL		),
_FastFuncDef( "vcname"	, 0 , 1 ,	v_vcname	, NULL		, NULL		),
// -----
_FastFuncDef( "idn"		, 1 , 1 ,	v_vnidn		, NULL		, NULL		),
_FastFuncDef( "vnidn"	, 1 , 1 ,	v_vnidn		, NULL		, NULL		),
_FastFuncDef( "vnstr"	, 1 , 1 ,	v_vnstr		, NULL		, NULL		),

// -------- Функции Теории Вероятноcти, Комбинаторики и Статистики --------
_FastFuncDef( "madev"	, 1 , 1 ,	f_math_arg1	, _madev_dbl	, ___noCpx_a1	),
_FastFuncDef( "median"	, 1 , 1 ,	f_math_arg1	, _median_dbl	, ___noCpx_a2	),
_FastFuncDef( "rms"		, 1 , 1 ,	f_math_arg1	, _rms_dbl		, ___noCpx_a1	),
_FastFuncDef( "stdev"	, 1 , 1 ,	f_math_arg1	, _stdev_dbl	, ___noCpx_a1	),
_FastFuncDef( "_stdev"	, 1 , 1 ,	f_math_arg1	, _stdevs_dbl	, ___noCpx_a1	),
_FastFuncDef( "var"		, 1 , 1 ,	f_math_arg1	, _var_dbl		, ___noCpx_a1	),
_FastFuncDef( "_var"	, 1 , 1 ,	f_math_arg1	, _vars_dbl		, ___noCpx_a1	),
//--
_FastFuncDef( "mse"		, 2 , 2 ,	f_math_arg2	, _mse_dbl		, ___noCpx_a2	),
_FastFuncDef( "moment"	, 2 , 2 ,	f_math_arg2	, _moment_dbl	, ___noCpx_a2	),
//--
_FastFuncDef( "mode"	, 1 , 2 ,	f_math_arg02	, _mode_dbl		, ___noCpx_a2	),
_FastFuncDef( "histy"	, 1 , 2 ,	f_math_arg02	, _histy_dbl	, ___noCpx_a2	),
_FastFuncDef( "histx"	, 1 , 2 ,	f_math_arg02	, _histx_dbl	, ___noCpx_a2	),
//--
_FastFuncDef( "erf"		, 1 , 1 ,	f_math_arg1	, _erf_dbl		, ___noCpx_a1	),
_FastFuncDef( "erfc"	, 1 , 1 ,	f_math_arg1	, _erfc_dbl		, ___noCpx_a1	),
_FastFuncDef( "gamma"	, 1 , 1 ,	f_math_arg1	, _gamma_dbl	, ___noCpx_a1	),
_FastFuncDef( "factr"	, 1 , 1 ,	f_math_arg1	, _factr_dbl	, ___noCpx_a1	),
//--
_FastFuncDef( "beta"	, 2 , 2 ,	f_math_arg2	, _beta_dbl		, ___noCpx_a2	),
_FastFuncDef( "gammai"	, 2 , 2 ,	f_math_arg2	, _gammai_dbl	, ___noCpx_a2	),
//--
_FastFuncDef( "betai"	, 3 , 3 ,	f_math_arg03	, _betai_dbl	, ___noCpx_a3	),
//--
_FastFuncDef( "_j0"		, 1 , 1 ,	f_math_arg1	, _j0_dbl	, ___noCpx_a1	),
_FastFuncDef( "_j1"		, 1 , 1 ,	f_math_arg1	, _j1_dbl	, ___noCpx_a1	),
_FastFuncDef( "_y0"		, 1 , 1 ,	f_math_arg1	, _y0_dbl	, ___noCpx_a1	),
_FastFuncDef( "_y1"		, 1 , 1 ,	f_math_arg1	, _y1_dbl	, ___noCpx_a1	),
//--
_FastFuncDef( "_jn"		, 2 , 2 ,	f_math_arg2	, _jn_dbl	, ___noCpx_a2	),
_FastFuncDef( "_yn"		, 2 , 2 ,	f_math_arg2	, _yn_dbl	, ___noCpx_a2	),


// -------- Функции поддержки Index Range --------
_FastFuncDef( "ir"		, 0 , 2 ,	f_math_argNx	, _ir_idx	, ___noCpx_aNx	),
_FastFuncDef( "irr"		, 0 , 2 ,	f_math_argNx	, _irr_idx	, ___noCpx_aNx	),
_FastFuncDef( "irb1"	, 1 , 1 ,	f_irb1			, NULL		, NULL		),
_FastFuncDef( "irb2"	, 1 , 1 ,	f_irb2			, NULL		, NULL		),
	//_FastFuncDef( "num2ir"	, 1 , 1 ,	f_math_argNx	, _num2ir_idx	, ___noCpx_aNx	),
	//_FastFuncDef( "ir2num"	, 1 , 1 ,	f_ir2num	, NULL		, NULL		),

// -------- Функции массивов --------
_FastFuncDef( "vector"	, 1 , 0x7FFF,	f_arr_vector	, NULL			, NULL		),
_FastFuncDef( "vectorn"	, 2 , 2		,	f_general_Nx	, _vectorn_arr	, NULL		),
//--
_FastFuncDef( "matrix"	, 1 , 0x7FFF,	f_arr_matrix	, NULL			, NULL		),
_FastFuncDef( "matrixn"	, 2 , 3		,	f_general_Nx	, _matrixn_arr	, NULL		),
_FastFuncDef( "matrixnv", 1 , 2		,	f_general_Nx	, _matrixnv_arr	, NULL		),
_FastFuncDef( "matrixd"	, 2 , 3		,	f_general_Nx	, _matrixd_arr	, NULL		),
_FastFuncDef( "matrixdv", 1 , 2		,	f_general_Nx	, _matrixdv_arr	, NULL		),
//--
_FastFuncDef( "nvecins"	, 2 , 3		,	f_math_arg03	, _nvecins_arr	, ___noCpx_a3	),
_FastFuncDef( "nvecifs"	, 2 , 3		,	f_math_arg03	, _nvecifs_arr	, ___noCpx_a3	),
_FastFuncDef( "nvecifn"	, 2 , 3		,	f_math_arg03	, _nvecifn_arr	, ___noCpx_a3	),
//--
_FastFuncDef( "nmtxins"	, 3 , 5		,	f_math_argNx	, _nmtxins_arr	, ___noCpx_aNx	),
_FastFuncDef( "nmtxifs"	, 3 , 5		,	f_math_argNx	, _nmtxifs_arr	, ___noCpx_aNx	),
_FastFuncDef( "nmtxifn"	, 3 , 5		,	f_math_argNx	, _nmtxifn_arr	, ___noCpx_aNx	),
//--
_FastFuncDef( "mtx2vec"	, 1 , 1		,	f_general_Nx	, _mtx2vec_arr	, NULL		),
_FastFuncDef( "vec2mtx"	, 3 , 3		,	f_general_Nx	, _vec2mtx_arr	, NULL		),
_FastFuncDef( "subvec"	, 3 , 3		,	f_general_Nx	, _subvec_arr	, NULL		),
_FastFuncDef( "submtx"	, 3 , 5		,	f_general_Nx	, _submtx_arr	, NULL		),
_FastFuncDef( "resize"	, 2 , 2		,	f_general_Nx	, _resize_arr	, NULL		),
_FastFuncDef( "resize2d", 3 , 3		,	f_general_Nx	, _resize2d_arr	, NULL		),
//--
_FastFuncDef( "size"	, 1 , 1		,	f_arr_size		, NULL			, NULL		),
_FastFuncDef( "rows"	, 1 , 1		,	f_arr_rows		, NULL			, NULL		),
_FastFuncDef( "cols"	, 1 , 1		,	f_arr_cols		, NULL			, NULL		),
//--
_FastFuncDef( "rever2c"	, 1 , 1		,	f_arr_1a		, _arr_rever2c	, NULL		),
_FastFuncDef( "rever2r"	, 1 , 1		,	f_arr_1a		, _arr_rever2r	, NULL		),
_FastFuncDef( "reverse"	, 1 , 1		,	f_arr_1a		, _arr_reverse	, NULL		),
_FastFuncDef( "transp"	, 1 , 1		,	f_arr_1a		, _arr_transpose, NULL		),
//--
_FastFuncDef( "sort"	, 1 , 2		,	f_arr_1ar2dbl	, _arr_sort		, NULL		),
_FastFuncDef( "rotate2c", 2 , 2		,	f_arr_1ar2dbl	, _arr_rotate2c	, NULL		),
_FastFuncDef( "rotate2r", 2 , 2		,	f_arr_1ar2dbl	, _arr_rotate2r	, NULL		),
_FastFuncDef( "rotate"	, 2 , 2		,	f_arr_1ar2dbl	, _arr_rotate	, NULL		),
_FastFuncDef( "shift"	, 2 , 2		,	f_arr_1ar2dbl	, _arr_shift	, NULL		),
_FastFuncDef( "search"	, 2 , 4		,	f_general_Nx	, _search_arr	, NULL		),

// -------- Функции строк --------
_FastFuncDef( "slen"	, 1 , 1		,	f_general_Nx	, _s_slen		, NULL		),
_FastFuncDef( "sfton"	, 1 , 1		,	f_general_Nx	, _s_sfton		, NULL		),
_FastFuncDef( "siton"	, 1 , 2		,	f_general_Nx	, _s_siton		, NULL		),
//--
_FastFuncDef( "slwr"	, 1 , 1		,	f_general_Nx	, _s_slwr		, NULL		),
_FastFuncDef( "supr"	, 1 , 1		,	f_general_Nx	, _s_supr		, NULL		),
_FastFuncDef( "srefine"	, 1 , 1		,	f_general_Nx	, _s_srefine	, NULL		),
_FastFuncDef( "sreverse", 1 , 1		,	f_general_Nx	, _s_sreverse	, NULL		),
_FastFuncDef( "sline"	, 1 , 2		,	f_general_Nx	, _s_sline		, NULL		),
_FastFuncDef( "srotate"	, 2 , 2		,	f_general_Nx	, _s_srotate	, NULL		),
_FastFuncDef( "sshift"	, 2 , 2		,	f_general_Nx	, _s_sshift		, NULL		),
_FastFuncDef( "substr"	, 3 , 3		,	f_general_Nx	, _s_substr		, NULL		),
//--
_FastFuncDef( "scmp"	, 2 , 2		,	f_general_Nx	, _s_scmp		, NULL		),
_FastFuncDef( "sccmp"	, 2 , 2		,	f_general_Nx	, _s_sccmp		, NULL		),
//--
_FastFuncDef( "smix"	, 2 , 3		,	f_smix			, NULL			, NULL		),
_FastFuncDef( "ntosi"	, 1 , 2		,	f_num_to_str	, _s_ntosi		, NULL		),
_FastFuncDef( "ntosf"	, 1 , 3		,	f_num_to_str	, _s_ntosf		, NULL		),
_FastFuncDef( "ntose"	, 1 , 3		,	f_num_to_str	, _s_ntose		, NULL		),


// -------- Функции анализа ---------
_FastFuncDef( "interpl"	, 3 , 4		,	f_math_argNxErr	, _interpl		, ___noCpx_aNxErr),
_FastFuncDef( "integral", 1 , 4		,	f_math_argNxErr	, _integral		, ___noCpx_aNxErr),
_FastFuncDef( "derivate", 1 , 4		,	f_math_argNxErr	, _derivate		, ___noCpx_aNxErr),
_FastFuncDef( "decimate", 1 , 3		,	f_math_argNxErr	, _decimate		, ___noCpx_aNxErr),

// -------- Функции времени ---------
_FastFuncDef( "GetTime"	, 0 , 0		,	f_GetTime		, NULL			, NULL		),
_FastFuncDef( "GetTicks", 0 , 0		,	f_GetTicks		, NULL			, NULL		),

// -------- Разные Функции ----------
_FastFuncDef( "abort"	, 0 , 0		,	f_abort			, NULL			, NULL		),
_FastFuncDef( "errno"	, 0 , 1		,	f_errno			, NULL			, NULL		),
_FastFuncDef( "vwrid"	, 0 , 1		,	f_vwrid			, NULL			, NULL		),	// >>> #CE_3.0.7 2026-07-25
_FastFuncDef( "lvmem"	, 0 , 1		,	f_lvmem			, NULL			, NULL		),	// >>> #CE_3.1.0 2026-08-27
_FastFuncDef( "_fflist"	, 0 , 0		,	_fflist			, NULL			, NULL		),
_FastFuncDef( "_nclist"	, 0 , 1		,	_nclist			, NULL			, NULL		),	// >>> #CE_3.1.0-r6 2026-09-27

// -------- Функция идентификации (должна быть последней) --------
_FastFuncDef( "_ce_inf"	, 0 , 1		,	_ce_inf			, NULL			, NULL		)

};

// =======================
// Содержит кол-во функций
// =======================
int32	FuncListLen	= sizeof(FuncList)/sizeof(_FastFuncInfo);


// ================================================================
// ***  Function returns an index of FastFunc function in FuncList
// ***  if a search fails, the function returns -1
// ================================================================
/* BEFORE #CE_3.0.2
int32	__FastFunc_GetIndex(char *name, int32 len)
{
	int32	idx;
	char	nBuf[FF_MaxNameLen];

	if (len <= FF_MaxNameLen) {
		//ClearMem(nBuf, FF_MaxNameLen);
		memset(nBuf, 0, FF_MaxNameLen);	// >>> #CE_3.0.2 2026-06-09
		_MoveBlock(name, nBuf, len);

		// ***** Поиск в таблице *****
		for (idx=0; idx < FuncListLen; idx++) {
			//if (!BlockCmp(nBuf, FuncList[idx].name, FF_MaxNameLen))
			if (!memcmp(nBuf, FuncList[idx].name, FF_MaxNameLen))		// >>> #CE_3.0.2 2026-06-06
				return idx;	// return index of FastFunc
		}
	}
	return -1;	// function not found
}
*/
// >>> #CE_3.0.2 2026-06-10
int32	__FastFunc_GetIndex(char *name, int32 len)
{
	int32	idx;
	union	{ 
		char	name[FF_MaxNameLen];
		uInt64	nTag;
	} look;

	if (len <= FF_MaxNameLen) {
		look.nTag = 0;
		_MoveBlock(name, look.name, len);

		// ***** Поиск в таблице *****
		for (idx=0; idx < FuncListLen; idx++) {
			if (look.nTag == FuncList[idx].ntag)
				return idx;	// return index of FastFunc
		}
	}
	return -1;	// function not found
}
// <<< #CE_3.0.2 2026-06-10
