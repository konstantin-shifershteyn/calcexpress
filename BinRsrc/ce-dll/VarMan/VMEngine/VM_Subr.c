/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <celib\ceTime.h>

//__inline	Bool32	test_strict_name_syntax	(	uChar *pnt, int32 len);
//__inline	Bool32	test_freestyle_name_syntax(	uChar *pnt, int32 len);

// ============================================================================
// Verifies a string to comply with the strict identifier name syntax (C-style)
// returns TRUE if an error
// ============================================================================
__inline Bool32 __fastcall test_strict_name_syntax(uChar *pnt, int32 len)
{
	const  static uChar	CTTbl[256] = 
	{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0 ,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1 ,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 ,
	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
	};

#define	__OK__	FALSE
#define	__ER__	TRUE
	if (len <= 0)				return __ER__;
	if ( CTTbl[ *pnt++ ] != 1)	return __ER__;

	while (--len > 0)
		if ( !CTTbl[ *pnt++ ] )
			return __ER__;
	return __OK__;
#undef	__OK__
#undef	__ER__
}

// ============================================================================
// Verifies a string to comply with the alias name syntax (free style)
// returns TRUE if an error
// ============================================================================
__inline Bool32	__fastcall test_freestyle_name_syntax(uChar *pnt, int32 len)
{
#define	__OK__	FALSE
#define	__ER__	TRUE

	if (len <= 0)		return __ER__;
	if (*pnt++ <= ' ')	return __ER__;
	
	while(--len) {
		if (*pnt++ < ' ')
			return __ER__;
	}
	return 	(*(--pnt) == ' ');	// __ERR__ if the last char is a space mark

#undef	__OK__
#undef	__ER__
}


// ==============================================
// Parse a string in the format: "[name][=alias]"
// return TRUE if an error
// ==============================================
Bool32	__fastcall _ParseNameAlias(
				LStrHandle		name,
				_NameAliasInfo	*nai,
				Bool32			bTestSyntax,	// >>> #CE_3.0	2025-04-14
//				LVBoolean		*err_f,
				uInt16			*err_c )
{
#define	__OK__	FALSE
#define	__ER__	TRUE

	uChar	*p, *pnt;
	int32	len;

	pnt = LStrBuf(*name);
	len = LStrLen(*name);

	// ***	initial conditions
	nai->flgName	= TRUE;
	nai->lenName	= len;
	nai->flgAlias	= FALSE;
	nai->lenAlias	= 0;
	nai->pntName	= nai->pntAlias	= (char*)pnt;

	// ***	search for an alias
	if (p = (uChar*)memchr(pnt, VM_ALIAS_SIGN, len)) {
		nai->lenName	= (int32)(p - pnt);		// >>> 2024-12-06 : CHECK 64 BIT
		nai->flgName	= (nai->lenName > 0);
		nai->lenAlias	= len - nai->lenName - 1;
		nai->pntAlias	= (char*)p + 1;
		nai->flgAlias	= TRUE;
	}

	if (bTestSyntax) {		// >>> #CE_3.0	2025-04-14
		// ***	test name syntax (if any)
		if (nai->flgName) {
			if (test_strict_name_syntax((uChar*)nai->pntName, nai->lenName)) {
//				*err_f	= LVTRUE;
				*err_c	= VM_ERR_VAR_INVNAME;
				return __ER__;
			}
		}
		// ***	test alias syntax (if any)
		if (nai->flgAlias) {
			if (test_freestyle_name_syntax((uChar*)nai->pntAlias, nai->lenAlias)) {
//				*err_f	= LVTRUE;
				*err_c	= VM_ERR_ALIAS_INVNAME;
				return __ER__;
			}
		}
	}
	return __OK__;
#undef	__OK__
#undef	__ER__
}

// =================================================================
// Функция генерирует уникальное имя переменной
// Выделенный блок памяти должен быть освобожден вызывающей функцией
// =================================================================
MgErr	__Create_RandomName(uChar **str)
{
static	uInt32	rndCnt = 0;
	uInt32	ms;
	uChar	*pnt = NULL;
	int32	len = 0;

	rndCnt	+= 1;
	ms		= MillisecsTimer();

	for(;;) {
		len = 1 + SPrintf(pnt, "__%06x%08x", rndCnt, ms);
		if (pnt)
			break;	// выход на втором проходе

		// выделить память под имя на первом проходе
		if ( !(pnt = ce_DSNewPtr(len))  ) {
			*str = NULL;
			return mFullErr;
		}
	}
	*str = pnt;
	return noErr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
