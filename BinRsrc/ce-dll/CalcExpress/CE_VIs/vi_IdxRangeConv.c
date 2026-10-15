/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	IndexRange to-from DBL
// -----------------------------------

#include <extcode.h>

#include <CalcExpress.h>
#include <Index_Routines.h>


typedef struct {
	int32 low_index;
	int32 top_index;
} IdxRange_Struct;


// ====================================
MgErr	__cdecl	viIdxRngConv( float64 *dbl_scalar, IdxRange_Struct *Index_Range)
{
	float64	tmp = *dbl_scalar;

	_PackIdxRangeToFloat(dbl_scalar, Index_Range->low_index, Index_Range->top_index);

	Index_Range->low_index = _UnpackIdxRangeFromFloat(&tmp, 0);
	Index_Range->top_index = _UnpackIdxRangeFromFloat(&tmp, 1);

	return noErr;
}
