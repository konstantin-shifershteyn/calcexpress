/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include "extcode.h"


// ====================================
MgErr __cdecl	lvmemstats(	MemStatRec *DS_MemStats)
{
	DSMemStats( DS_MemStats );
	return noErr;
}

// ====================================
MgErr __cdecl	lvmemuse(
					uInt64 *totAllocSize,
					uInt64 *totPhysSize
				)
{
	MemStatRec DS_MemStats;

	DSMemStats( &DS_MemStats );

	*totAllocSize	= DS_MemStats.totAllocSize;
	*totPhysSize	= DS_MemStats.totPhysSize;

	return noErr;
}
