/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__SetIndexBaseRoutines_C__
#define	__SetIndexBaseRoutines_C__


// ======================================================================
// ==============	Set Elements of 1D Arrays	=================
// ======================================================================

void  set_element_of_A1DBL( _dbl_operand *opd,  int32 idx,  float64 *src )
{
	if ((idx >= 0) && (idx < opd->size1))	*(opd->array + idx) = *src;
}

void  set_element_of_A1CDB( _dbl_operand *opd,  int32 idx,  float64 *src )
{
	register  float64  *pnt = opd->array + idx*2;
	if ((idx >= 0) && (idx < opd->size1))	{ *pnt++ = *src++; *pnt = *src; }
}

void  set_element_of_A1STR( _str_operand *opd,  int32 idx,  LStr **src )
{
static	LStr	nulStr	= {0, 0};
	if ((idx >= 0) && (idx < opd->size1))	*(opd->array + idx) = *src;
}


// ======================================================================
// ==============	Set Elements of 2D Arrays	=================
// ======================================================================

void  set_element_of_A2DBL(_dbl_operand *opd, int32 i1r, int32 i2c, float64 *src)
{
	if ((i2c >= 0) && (i2c < opd->size2))
		if ((i1r >= 0) && (i1r < opd->size1))
			*(opd->array + (i1r * opd->size2 + i2c)) = *src;
}

void  set_element_of_A2CDB(_dbl_operand *opd, int32 i1r, int32 i2c, float64 *src)
{
	register  float64  *pnt = opd->array + (i1r * 2 * opd->size2 + i2c * 2);
	if ((i2c >= 0) && (i2c < opd->size2)) {
		if ((i1r >= 0) && (i1r < opd->size1)) {
			*pnt++ = *src++; *pnt = *src;
		}
	}
}

void  set_element_of_A2STR(_str_operand *opd, int32 i1r, int32 i2c, LStr **src)
{
	static	LStr	nulStr	= {0, 0};
	if ((i2c >= 0) && (i2c < opd->size2))
		if ((i1r >= 0) && (i1r < opd->size1))
			*(opd->array + (i1r * opd->size2 + i2c)) = *src;
}

#endif
