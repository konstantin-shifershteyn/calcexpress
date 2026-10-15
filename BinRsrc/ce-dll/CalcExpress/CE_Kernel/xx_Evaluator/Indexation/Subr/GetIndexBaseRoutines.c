/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#ifndef	__GetIndexBaseRoutines_C__
#define	__GetIndexBaseRoutines_C__

// ======================================================================
// ==============	Get Elements of 1D Arrays	=================
// ======================================================================

void  get_element_of_A1DBL( _dbl_operand *opd,  int32 idx,  float64 *dst )
{
	if ((idx >= 0) && (idx < opd->size1))	*dst = *(opd->array + idx);
	else									*dst = 0.0;
}

void  get_element_of_A1CDB( _dbl_operand *opd,  int32 idx,  float64 *dst )
{
	float64	*pnt = opd->array + idx*2;
	if ((idx >= 0) && (idx < opd->size1))	{ *dst++ = *pnt++; *dst = *pnt; }
	else									{ *dst++ = 0.0;	   *dst = 0.0;  }
}

void  get_element_of_A1STR( _str_operand *opd,  int32 idx,  LStr **dst )
{
static	LStr	nulStr	= {0, 0};
	if ((idx >= 0) && (idx < opd->size1))	*dst = *(opd->array + idx);
	else									*dst = &nulStr;
}

// ======================================================================
// ==============	Get Elements of 2D Arrays	=================
// ======================================================================

void  get_element_of_A2DBL(_dbl_operand *opd, int32 i1r, int32 i2c, float64 *dst)
{
	if ((i2c >= 0) && (i2c < opd->size2)) {
		if ((i1r >= 0) && (i1r < opd->size1)) {
			*dst = *(opd->array + (i1r * opd->size2 + i2c));
			return;
		}
	}
	*dst = 0.0;
}

void  get_element_of_A2CDB(_dbl_operand *opd, int32 i1r, int32 i2c, float64 *dst)
{
	float64	*pnt = opd->array + (i1r * 2 * opd->size2 + i2c * 2);
	if ((i2c >= 0) && (i2c < opd->size2)) {
		if ((i1r >= 0) && (i1r < opd->size1)) {
			*dst++ = *pnt++; *dst = *pnt;
			return;
		}
	}
	*dst++ = 0.0; *dst = 0.0;
}

void  get_element_of_A2STR(_str_operand *opd, int32 i1r, int32 i2c, LStr **dst)
{
static	LStr	nulStr	= {0, 0};
	if ((i2c >= 0) && (i2c < opd->size2)) {
		if ((i1r >= 0) && (i1r < opd->size1)) {
			*dst = *(opd->array + (i1r * opd->size2 + i2c));
			return;
		}
	}
	*dst = &nulStr;
}

#endif
