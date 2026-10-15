/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef __ceTime_H__
#define __ceTime_H__

#include <extcode.h>


#ifdef __cplusplus
extern "C" {
#endif

#define	MillisecsTimer()	MilliSecs()			// call the LV function, slowe but with 1 ms resolution
//#define	MillisecsTimer()	GetTickCount()		// call the System function, it works faster than LV' MilliSecs() but 16 ms resolution

// =============================================
//	Time function
// =============================================

extern	float64	__GetTime_Labview_DBL();


#ifdef __cplusplus
}
#endif

#endif
