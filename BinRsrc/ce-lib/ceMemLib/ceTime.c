/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <celib\ceTime.h>

#define	WINDOWS_IGNORE_PACKING_MISMATCH
#include <windows.h>

///////////////////////////////////////////////////////////////
// >>> NEW VERSION. it's expected to run faster
// >>> #CE_3.0.7 2026-07-29

#define	WIN_TO_LV_EPOCH_OFFSET_100NS	95616288000000000ULL	// 9561628800 * 10000000
#define	NS100_TO_SEC_MULTIPLIER			0.0000001				// 1.0/10000000.0

float64 __GetTime_Labview_DBL() {
	ULARGE_INTEGER li;

	// Get system UTC time (fastest API for standard resolution)
	GetSystemTimeAsFileTime((FILETIME*)&li);
//	GetSystemTimePreciseAsFileTime((FILETIME*)&li);

	// Perform integer math before converting to float
	// This avoids high-precision division penalties
	return (float64)(li.QuadPart - WIN_TO_LV_EPOCH_OFFSET_100NS) * NS100_TO_SEC_MULTIPLIER;
}
// <<< #CE_3.0.7 2026-07-29
