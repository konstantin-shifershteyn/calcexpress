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


// ##################################################
// =============  Test Variable VI  =================
// ##################################################
MgErr __cdecl	viVarTest(int32 *VCH, LStrHandle vName, uInt32 *vType, LVBoolean *err_f, uInt16 *err_c)
{
	LVBoolean	global;
	_VarInfo	vPar;
	MgErr		err;

	vPar.str	= NULL;
	global		= (*VCH == -1);

	if (err = vmCmd_Var_GetValue(*VCH, &global, vName, &vPar, NULL, err_f, err_c)) {
		vm_LVError(err, err_f, err_c);
	}
	else {
		if (*err_f)	*vType = 255;			// CHECK_ME: 255 ???
		else		*vType = vPar.type;
	}
	return err;
}

// ##################################################
// ============  Destroy Variable VI  ===============
// ##################################################
MgErr __cdecl	viVarDestroy( int32 *VCH, LStrHandle name, LVBoolean *glob, LVBoolean *err_f, uInt16 *err_c )
{
	MgErr	err;
	if (err = vmCmd_Var_Destroy(*VCH, glob, name, err_f, err_c))
		vm_LVError(err, err_f, err_c);
	return err;
}

// ##################################################
// =======  Variable Info VI : VER 2.2.x  ===========
// ##################################################
MgErr __cdecl	viVarInfo(
				int32		*VCH,
				LStrHandle	NameSpec,
				LStrHandle	name,
				LStrHandle	alias,
				uInt32		*type,
				LVBoolean	*fHist,
				LVBoolean	*err_f,
				uInt16		*err_c )
{
	_VarInfoReport	vir;
	LVBoolean	global;
	_VarInfo	vPar;
	MgErr		err;

	vPar.str = NULL;	// !!! will be allocated in vmCmd_Var_InfoList UNLESS AN ERROR HAPPENS

	global = (*VCH == -1);

	// ***	Call a VarMan's command routine
	err = vmCmd_Var_InfoList(*VCH, &global, NameSpec, &vPar, err_f, err_c);
	if (err || *err_f)
		goto out;

	// *** parse variable record
#pragma warning (push)
#pragma warning (disable: 6011)		// we cannot come here if vPar.str == NULL
	VInfo_From_CString( LStrBuf(*(vPar.str)), &vir);
#pragma warning (pop)

	// ***	save information on variable
	*VCH	= vir.vch;
	*type	= vir.type;
	*fHist	= vir.fHist;

	// ***	name
	if (err = LStr_SetSize(name, vir.lenName) )
		goto out;
	_MoveBlock( vir.pntName, LStrBuf(*name), vir.lenName);

	// ***	alias
	if (err = LStr_SetSize(alias, vir.lenAlias) )
		goto out;
	_MoveBlock( vir.pntAlias, LStrBuf(*alias), vir.lenAlias);
out:
	// *** Dispose of vPar->str if it was allocated
	if (vPar.str)
		LStr_Destroy(vPar.str);

	if (err) {
		LStr_Clear(name);
		LStr_Clear(alias);
		vm_LVError(err, err_f, err_c);
	}
	return err;
}

// >>> #CE_3.0.8 2026-07-31
// ##################################################
// =======  Variable Info VI : VER 3.0.8  ===========
// ##################################################
MgErr __cdecl	viVarInfo2(
				int32		*VCH,
				LStrHandle	NameSpec,
				LStrHandle	name,
				LStrHandle	alias,
				uInt32		*type,
				LVBoolean	*fHist,
				uInt16		*err_c )
{
	_VarInfoReport	vir;
	LVBoolean		global;
	_VarInfo		vPar;
	LVBoolean		errf = LVFALSE;
	MgErr			err;

	vPar.str = NULL;	// !!! will be allocated in vmCmd_Var_InfoList UNLESS AN ERROR HAPPENS

	global = (*VCH == -1 ? LVTRUE : LVFALSE);

	// ***	Call a VarMan's command routine
	err = vmCmd_Var_InfoList(*VCH, &global, NameSpec, &vPar, &errf, err_c);
	if (err || errf)
		goto out;

	// *** parse variable record
#pragma warning (push)
#pragma warning (disable: 6011)		// we cannot come here if vPar.str == NULL
	VInfo_From_CString( LStrBuf(*(vPar.str)), &vir);
#pragma warning (pop)

	// ***	save information on variable
	*VCH	= vir.vch;
	*type	= vir.type;
	*fHist	= vir.fHist;

	// ***	name
	if (vir.lenName) {
		if (err = LStr_SetSize(name, vir.lenName) )
			goto out;
		_MoveBlock( vir.pntName, LStrBuf(*name), vir.lenName);
	}
	// ***	alias
	if (vir.lenAlias) {
		if (err = LStr_SetSize(alias, vir.lenAlias) )
			goto out;
		_MoveBlock( vir.pntAlias, LStrBuf(*alias), vir.lenAlias);
	}
out:
	// *** Dispose of vPar->str if it was allocated
	if (vPar.str)
		LStr_Destroy(vPar.str);

	if (err) {
		LStr_Clear(name);
		LStr_Clear(alias);
		vm_LVError(err, &errf, err_c);
	}
	return err;
}
// <<< #CE_3.0.8 2026-07-31

// >>> #CE_3.0.8 2026-07-31
// ##################################################
// =====  Variable Data Info VI : VER 3.0.8  ========
// ##################################################
MgErr __cdecl	viVarDataInfo(
					int32				vch,
					LStrHandle			NameSpec,
					uInt32				*datatype,
					LVBoolean			*hasHistory,
					//uInt32				*writerID,
					_VM_VI_WriteStats	*wrStats,
					_VM_VI_CreateStats	*creStats,
					uInt16				*err_c )
{
	_VarRecord		*vp;
	LVBoolean		global;

	global = (vch == -1 ? LVTRUE : LVFALSE);

	VARMAN_LOCK_SHARED();

	vp = __GetVarPointer(vch, &global, NameSpec, NULL, err_c);
	if (vp != NULL) {
		*datatype			= vp->inf.type;

		__ReadVarWritingStats(vp, wrStats);		// >>> #CE_3.1.0-r6 2026-09-18
// >>> #CE_3.1.0-r6 2026-09-19
		creStats->creatorID	= vp->creatorID;
		creStats->createMS	= vp->createMS;
		creStats->createTS	= vp->createTS;
// <<< #CE_3.1.0-r6 2026-09-19
		*hasHistory	= (vp->hist != NULL ? LVTRUE : LVFALSE);
	}

	VARMAN_UNLOCK_SHARED();

	return noErr;
}
// <<< #CE_3.0.8 2026-07-31

// ##################################################
// ===========  Variable Info List VI  ==============
// ##################################################
typedef struct {
	LStrHandle	name;
	LStrHandle	alias;
	uInt32		type;
	LVBoolean	fHist;
#if defined(_M_X64)
	uInt8		pad_2[3];	// >>> #CE_3.0 2024-12-12 : padding bytes
#endif
} VI_VInf;
//------------------------
typedef struct {
	int32		dimSize;
#if defined(_M_X64)
	int32		pad_1;		// >>> #CE_3.0 2024-12-12 : padding bytes
#endif
	VI_VInf		data[1];
} VI_VInfArray, **h_VI_VInfArray;
// ====================================
MgErr __cdecl	viVarInfoList(
				int32			*VCH,
				h_VI_VInfArray	VIArr,
				LVBoolean		*err_f,
				uInt16			*err_c )
{
	_VarInfoReport	vir;
	VI_VInf			*arrPnt;
	int32			i, arrSize;
	uChar			*sPnt;
	MgErr			err;
	LVBoolean		global;
	_VarInfo		vPar;

	global = (*VCH == -1);

	vPar.str = NULL;	// !!! will be allocated in vmCmd_Alias_InfoList UNLESS AN ERROR HAPPENS

	// ***	Call a VarMan's command routine
	err = vmCmd_Var_InfoList(*VCH, &global, NULL, &vPar, err_f, err_c);
	if (err || *err_f)
		goto out;

	// ***	Array size (number of variable records)
	arrSize = (int32)vPar.num;	//размер массива	

	// ***	Set size of the output array
	if (err = ce_1DArraySetSize((UHandle*)&VIArr, arrSize, sizeof(VI_VInf)) )
		goto out;
	arrPnt	= (*VIArr)->data;	// pointer to the first element in output array
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
		ce_1DArraySetSize((UHandle*)&VIArr, 0, sizeof(VI_VInf));
	}
out:
	// *** Dispose of vPar->str if it was allocated
	if (vPar.str)
		LStr_Destroy(vPar.str);

	if (err)
		vm_LVError(err, err_f, err_c);

	return err;
}


// ##################################################
// =======  List Of Variables VI (obsolete)  ========
// ##################################################
typedef struct {
	LStrHandle	name;
	uInt32		type;
} VInf;
typedef struct {
	int32	dimSize;
	VInf	data[1];
} VInfArray, **VInfArrayHdl;
// -------------

MgErr __cdecl	viVarList(
				int32			*VCH,
				VInfArrayHdl	VIArr,
				LVBoolean		*err_f,
				uInt16			*err_c)
{
	_VarInfoReport	vir;
	VInf		*arrPnt;
	int32		arrSize;
	uChar		*sPnt;
	int32		i;
	MgErr		err;
	LVBoolean	global;
	_VarInfo	vPar;

	global = (*VCH == -1);

	vPar.str = NULL;	// !!! will be allocated in vmCmd_Alias_InfoList UNLESS AN ERROR HAPPENS

	// ***	Call a VarMan's command routine
	err = vmCmd_Var_InfoList(*VCH, &global, NULL, &vPar, err_f, err_c);
	if (err || *err_f)
		goto out;

	// ***	Array size (number of variable records)
	arrSize = (int32)vPar.num;	//размер массива	

	// ***	Set size of the output array
	if (err = ce_1DArraySetSize((UHandle*)&VIArr, arrSize, sizeof(VInf)) )
		goto out;
	arrPnt	= (*VIArr)->data;	// pointer to the first element in output array
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
		arrPnt[i].type	= vir.type;
	}
	if (err) {
		// ***	If memory error occurred
		// ***	Free all allocated memory blocks
		for (i=0; i < arrSize; i++) {
			ce_DisposeHandles( (UHandle*)&(arrPnt[i].name) , 1);
		}
		// ***	Set size of output array to zero
		ce_1DArraySetSize((UHandle*)&VIArr, 0, sizeof(VInf));
	}
out:
	// *** Dispose of vPar->str if it was allocated
	if (vPar.str)
		LStr_Destroy(vPar.str);

	if (err)
		vm_LVError(err, err_f, err_c);

	return err;
}

