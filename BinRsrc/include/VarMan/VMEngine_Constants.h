/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef __VME_CONSTANTS_H__
#define __VME_CONSTANTS_H__

#include <extcode.h>

// ===================================
// Table of Contexts related constants
// ===================================

//#define CTBL_BLKSIZE		256					// number of contexts in a block
#define CTBL_BLKSIZE		1					// number of contexts in a block [#CE_3.1.0 2026-09-15] : to avoid memory wasting
#define CTBL_GBLIDX			(CTBL_BLKSIZE-1)	// index of global context

// ***	number of miscellaneous fields in context info
#define	CNTX_MISC_FIELDS	4

// ***	Constands to support Context Table operations

#define	CTBL_HANDLE_INC		0x00010000
#define	CTBL_HANDLE_MASK	0x7fff0000

#define CTBL_INDEX_MASK		0x0000FFFF
#define	CTBL_MAX_INDEX		CTBL_INDEX_MASK
#define	CTBL_INDEX(hndl)	((hndl) & CTBL_INDEX_MASK)


// ====================================
// Variable Hash-Table constants
// ====================================
#define	VAR_HASH_TABLE_SIZE		256			// DON'T CHANGE!!! must be power of 2 !!!
#define VAR_HASH_KEY_MASK		(VAR_HASH_TABLE_SIZE-1)

// ====================================
// Alias Hash-Table constants
// ====================================
#define	ALIAS_HASH_TABLE_SIZE	256			// DON'T CHANGE!!! must be power of 2 !!!
#define ALIAS_HASH_TABLE_MASK	(ALIAS_HASH_TABLE_SIZE-1)


#endif
