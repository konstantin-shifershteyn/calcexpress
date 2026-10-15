/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <extcode.h>

#include <celib\ceMemLib.h>

#include <VarMan.h>
#include <VarInfo_Routines.h>


int32	VInfo_To_CString(uChar *dst, _VarInfoReport *vir)
{
	int32	len, lenTotal = 0;

	// ***	variable name
	// ==================
	len = vir->lenName;
	if (dst) {
		if (len)
			_MoveBlock(vir->pntName, dst, len);
		dst += len;
		*dst++ = 0;
	}
	lenTotal += len + 1;

	// ***	alias name
	// ===============
	len = vir->lenAlias;
	if (dst) {
		if (len)
			_MoveBlock(vir->pntAlias, dst, len);
		dst += len;
		*dst++ = 0;
	}
	lenTotal += len + 1;

	// ***	VC Handle
	// ==============
	len = sizeof(vir->vch);
	if (dst) {
		_MoveBlock( &(vir->vch), dst, len );
		dst += len;
	}
	lenTotal += len;

	// ***	Variable Type
	// ==================
	len = sizeof(vir->type);
	if (dst) {
		_MoveBlock( &(vir->type), dst, len );
		dst += len;
	}
	lenTotal += len;

	// ***	History Flag
	// =================
	len = sizeof(vir->fHist);
	if (dst) {
		_MoveBlock( &(vir->fHist), dst, len );
		dst += len;
	}
	lenTotal += len;

	return lenTotal;
}

// =================================================

uChar *	VInfo_From_CString(uChar *src, _VarInfoReport *vir)
{

	// ***	variable name
	// ==================
	vir->pntName	= src;
	vir->lenName	= StrLen(vir->pntName);
	src += vir->lenName + 1;

	// ***	alias name
	// ===============
	vir->pntAlias	= src;
	vir->lenAlias	= StrLen(vir->pntAlias);
	src += vir->lenAlias + 1;

	// ***	VC Handle
	// ==============
	_MoveBlock( src, &(vir->vch), sizeof(vir->vch));
	src += sizeof(vir->vch);

	// ***	Variable Type
	// ==================
	_MoveBlock( src, &(vir->type), sizeof(vir->type));
	src += sizeof(vir->type);

	// ***	History Flag
	// =================
	_MoveBlock( src, &(vir->fHist), sizeof(vir->fHist));
	src += sizeof(vir->fHist);

	return src;
}

