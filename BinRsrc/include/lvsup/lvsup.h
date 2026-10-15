/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__LVSUP_H__
#define	__LVSUP_H__

//#include <limits.h>
//#include <emmintrin.h>

#pragma warning (error	: 4013) /* 'function' undefined; assuming extern returning int */
#pragma warning (error	: 4020) /* 'function' : too many actual parameters */

#pragma warning (error: 4244) /* conversion between different sized ints */
#pragma warning (error: 4761) /* integral size mismatch in argument: conversion supplied */

// >>> #CE_3.1.0-r6 2026-09-25
#define	_DblToI32(xdbl)			((xdbl) < INT_MIN ? INT_MIN : (xdbl) > INT_MAX  ? INT_MAX  : (int32)(xdbl))
#define	_DblToU32(xdbl)			((xdbl) < 0.0     ? 0UL     : (xdbl) > UINT_MAX ? UINT_MAX : (uInt32)(xdbl))
#define	_DblToI32Bits(xdbl)		((int32)(int64)(xdbl))
#define	_DblToU32Bits(xdbl)		((uInt32)(int64)(xdbl))
// <<< #CE_3.1.0-r6 2026-09-25


// ***	Type defines 1D Array of LV String Handles
// -----------------------------------------------
typedef struct {
	int32		size;
#if defined(_M_X64)
	int32		pad_1;		// >>> #CE_3.0 2024-12-11
#endif
	LStrHandle	data[1];
} LStr1DArray, ** LStr1DArrayHdl;

// -------------------------------
// LabVIEW  1D Array of DBL values
// -------------------------------
typedef	struct {
	int32		size;
#if defined(_M_X64)
	int32		pad_1;		// >>> #CE_3.0 2024-12-11
#endif
	float64		val[1];
} _LVA1Dbl, **_hLVA1Dbl;

// -------------------------------
// LabVIEW  1D Array of LV Strings
// -------------------------------
typedef	struct {
	int32		size;
#if defined(_M_X64)
	int32		pad_1;		// >>> #CE_3.0 2024-12-11
#endif
	LStrHandle	val[1];
} _LVA1Str, **_hLVA1Str;

// ------------------------------
// LabVIEW  1D Array of anything !!! with size >= sizeof(size_t) !!!
// ------------------------------
typedef	struct {
	int32		size;
#if defined(_M_X64)
	int32		pad_1;		// >>> #CE_3.0 2024-12-11
#endif
	char		val[1];
} _LVA1Poly, **_hLVA1Poly;

// -------------------------------
// LabVIEW  1D Array of U32 values
// -------------------------------
typedef	struct {
	int32		size;
#if defined(_M_X64)
	// no padding			// >>> #CE_3.1.0 2026-08-26
#endif
	uInt32		val[1];
} _LVA1U32, **_hLVA1U32;


#endif
