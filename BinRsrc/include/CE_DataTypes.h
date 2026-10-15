/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CE_DATA_TYPES__
#define	__CE_DATA_TYPES__

// Коды типов данных, используемых в CalcExpress и VarMan

#define	OPTYPE_STR		0
#define	OPTYPE_DBL		1
#define	OPTYPE_IDENT	2
#define	OPTYPE_A1DBL	3
#define	OPTYPE_A2DBL	4
#define	OPTYPE_CDB		5
#define	OPTYPE_A1CDB	6
#define	OPTYPE_A2CDB	7
#define	OPTYPE_A1STR	8
#define	OPTYPE_A2STR	9

#define	OPTYPE_UNKNOWN	10

// for compatibility with old versions
#define	OPTYPE_NUMBER	OPTYPE_DBL
#define	OPTYPE_STRING	OPTYPE_STR

// indicates skipped argument in function calls
#define	OPTYPE_SKIPPED_ARG	251
#define	OPTYPE_IDXRANGE		252	// used in CE, VM and other CINs


// A virtual type denotes any type in Flattened-To-String format
// It is used by some VIs.
// Neither operands nor variables may have this type !!!
#define	OPTYPE_ANY_FLATTENED	((uInt32)(-1))

// -------------------------------
// >>> #CE_3.0.4 2026-07-04
#define	STATIC_ASSERT_8_BYTES_OFFSET(s,m)		static_assert(offsetof(s,m) % 8 == 0, "offset of '" #s"."#m "' is not aligned to 8 bytes")
#define	STATIC_ASSERT_8_BYTES_SIZE(s)			static_assert(sizeof(s) % 8 == 0, "size of '" #s "' is not aligned to 8 bytes")
#define	STATIC_ASSERT_4_BYTES_SIZE(s)			static_assert(sizeof(s) % 4 == 0, "size of '" #s "' is not aligned to 4 bytes")
// <<< #CE_3.0.4 2026-07-04
// >>> #CE_3.0.4 2026-07-09
#define	STATIC_ASSERT_PNT_BYTES_OFFSET(s,m)		static_assert(offsetof(s,m) % sizeof(void*) == 0, "offset of '" #s"."#m "' is not aligned to a pointer size")
#define	STATIC_ASSERT_PNT_BYTES_SIZE(s)			static_assert(sizeof(s) % sizeof(void*) == 0, "size of '" #s"' is not aligned to a pointer size")
// <<< #CE_3.0.4 2026-07-09

#endif
