/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VarMan	VIs Interface
// -----------------------------------

#include <extcode.h>

#include <VarMan.h>
#include <VarInfo_Routines.h>
#include <VMEngine_Routines.h>

#include <celib\ceMemLib.h>

// ==========================================================
// =============  Alias Controller VI  ======================
// ==========================================================

MgErr __cdecl	viAliasController(
						int32		*handle,
						uInt32		*action,
						LStrHandle	name,
						LStrHandle	alias,
						LVBoolean	*err_f,
						uInt16		*err_c )
{
	LVBoolean	global;
	_VarInfo	vPar;
	MgErr		err;

	if (!(*action))
		return noErr;

	// ***	prepare parameters
	vPar.type	= OPTYPE_STR;
	vPar.num	= *action;
	vPar.str	= alias;

	global		= (*handle == -1);

	if (err = vmCmd_Alias_Operation(handle, &global, name, &vPar, err_f, err_c))
		vm_LVError(err, err_f, err_c);

	return err;
}

// ==========================================================
// =============  Alias Info List VI  =======================
// ==========================================================

typedef struct {
	LStrHandle	alias;
	int32		vch;
#if defined(_M_X64)
	int32		pad_1;			// >>> #CE_3.0 2024-12-12
#endif
	LStrHandle	name;
	uInt32		type;
	LVBoolean	fHist;
#if defined(_M_X64)
	uInt8		pad_2[3];		// >>> #CE_3.0 2024-12-12
#endif
} VI_AliasInf;
// -----------------

typedef struct {
	int32		dimSize;
#if defined(_M_X64)
	int32		pad_1;			// >>> #CE_3.0 2024-12-12
#endif
	VI_AliasInf	data[1];
} VI_AliasInfArray,  **h_VI_AliasInfArray;
// -----------------

MgErr __cdecl	viAliasInfoList(
						h_VI_AliasInfArray	AIArr,
						LVBoolean			*err_f,
						uInt16				*err_c)
{
	_VarInfoReport	vir;
	_VarInfo	vPar;
	VI_AliasInf	*arrPnt;
	int32		i, arrSize;
	uChar		*sPnt;
	MgErr		err;


	vPar.str = NULL;	// !!! will be allocated in vmCmd_Alias_InfoList UNLESS AN ERROR HAPPENS

	// ***	Call a VarMan's command routine
	err = vmCmd_Alias_InfoList( &vPar, err_f, err_c);
	if (err || *err_f)
		goto out;

	// ***	Array size (number of alias records)
	arrSize = (int32)vPar.num;	//размер массива	

	// ***	Set size of the output array
	if ( err = ce_1DArraySetSize((UHandle*)&AIArr, arrSize, sizeof(VI_AliasInf)) )
		goto out;

	arrPnt	= (*AIArr)->data;	// pointer to the first element in output array
#pragma warning (push)
#pragma warning (disable: 6011)		// we cannot come here if vPar.str == NULL
	sPnt	= LStrBuf(*(vPar.str));	// pointer to info-string
#pragma warning (pop)

	// ***	Fill in the output array
	for (i=0; i < arrSize; i++) {

		// *** parse variable record
		sPnt = VInfo_From_CString( sPnt, &vir);

		// ***	save information on variable
		if (err = LStr_CreateFromMem( &(arrPnt[i].name), vir.pntName, vir.lenName))
			break;		//goto out;
		if (err = LStr_CreateFromMem( &(arrPnt[i].alias), vir.pntAlias, vir.lenAlias))
			break;		//goto out;
		arrPnt[i].vch	= vir.vch;
		arrPnt[i].type	= vir.type;
		arrPnt[i].fHist	= vir.fHist;
	}
	if (err) {
		// ***	If memory error occurred
		// ***	Free all allocated memory blocks
		for (i=0; i < arrSize; i++) {
			ce_DisposeHandles( (UHandle*)&(arrPnt[i].name) , 1);
			ce_DisposeHandles( (UHandle*)&(arrPnt[i].alias), 1);
		}
		// ***	Set size of output array to zero
		ce_1DArraySetSize((UHandle*)&AIArr, 0, sizeof(VI_AliasInf));
	}

out:
	// *** Dispose of vPar->str if it was allocated
	if (vPar.str)
		LStr_Destroy(vPar.str);

	if (err)
		vm_LVError(err, err_f, err_c);
	return err;
}
