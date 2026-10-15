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
#include <VMEngine_Routines.h>

#include <celib\ceMemLib.h>
#include <lvsup\lvsup.h>

// ==============================================
// ===========	Init VarMan VI  =================
// ==============================================
MgErr __cdecl	viVMInit( LVBoolean *err_f, uInt16 *err_c)
{
	MgErr	err;
	if (err = vmCmd_VarMan_Init(err_f, err_c))
		vm_LVError(err, err_f, err_c);
	return err;
}

// ===============================================
// ===========	Open Context VI  =================
// ===============================================
MgErr __cdecl	viVCOpen(	
				int32		*VC_Handle,
				LStrHandle	Creator,
				int32		*LifeTime,
				LVBoolean	*err_f,
				uInt16		*err_c )
{
	MgErr	err;
	if (err = vmCmd_Context_Open(VC_Handle, Creator, LifeTime, err_f, err_c))
		vm_LVError(err, err_f, err_c);
	return err;
}

// ================================================
// ===========	Close Context VI  =================
// ================================================
MgErr __cdecl	viVCClose( int32 *VC_Handle, LVBoolean *err_f, uInt16 *err_c)
{
	MgErr	err;
	if (err = vmCmd_Context_Close(VC_Handle, err_f, err_c))
		vm_LVError(err, err_f, err_c);
	return err;
}

// ===============================================
// ===========	Test Context VI  =================
// ===============================================
MgErr __cdecl	viVCTest( int32 *VC_Handle, LVBoolean *err_f, uInt16 *err_c)
{
	MgErr	err;
	if (err = vmCmd_Context_Test(VC_Handle, err_f, err_c))
		vm_LVError(err, err_f, err_c);
	return err;
}

// ===============================================
// ===========	Get Context Info VI  =============
// ===============================================
MgErr __cdecl	viVCInfo(
				int32		*VC_Handle,
				LStrHandle	Creator,
				LStrHandle	vcName,
				int32		*LifeTime,
				LVBoolean	*err_f,
				uInt16		*err_c)
{
	MgErr	err;
	if (err = vmCmd_Context_GetInfo(VC_Handle, Creator, vcName, LifeTime, err_f, err_c))
		vm_LVError(err, err_f, err_c);
	return err;
}

// ===================================================
// ===========	List Of Contexts VI  =================
// ===================================================
typedef struct {
	int32	dimSize;
//#if defined(_M_X64)
//	int32		pad_1;		// >>> #CE_3.0 2024-12-12 : padding bytes
//#endif
	int32	data[1];
} _VCHArray, **VCHArrayHdl;

// ---------
MgErr __cdecl	viVCList(
				LStrHandle	Creator,
				VCHArrayHdl	VCHArr,
				LVBoolean	*err_f,
				uInt16		*err_c )
{
	_VarInfo	vPar;
	int32		i, arrSize;
	int32		*arrPnt, *sPnt;
	MgErr		err;


	vPar.str = NULL;	// !!! will be allocated in vmCmd_Context_InfoList  UNLESS AN ERROR HAPPENS

	// ***	Call a VarMan's command routine
	err = vmCmd_Context_InfoList(Creator, &vPar, err_f, err_c);
	if (err || *err_f)
		goto out;

	// ***	Array size (number of VC handles)
	arrSize = (int32)vPar.num;	//размер массива	

	// ***	Set size of the output array
	if (err = ce_1DArraySetSize((UHandle*)&VCHArr, arrSize, sizeof(int32)) )
		goto out;

	arrPnt	= (*VCHArr)->data;		// pointer to the first element in output array
#pragma warning (push)
#pragma warning (disable: 6011)		// we cannot come here if vPar.str == NULL
	sPnt	= (int32*)LStrBuf(*(vPar.str));	// pointer to info-string
#pragma warning (pop)

	// ***	Fill in the output array
	for (i=0; i<arrSize; i++)
		*arrPnt++ = *sPnt++;

out:
	// *** Dispose of vPar->str if it was allocated
	if (vPar.str)
		LStr_Destroy(vPar.str);

	if (err)
		vm_LVError(err, err_f, err_c);
	return err;
}

// ===========================================================
// ===========	Context Names Controller VI  =================
// ===========================================================
MgErr __cdecl	viVCNames(
				uInt32		*action,
				int32		*VC_Handle,
				LStrHandle	VC_Name,
				uInt16		*VIErrCode,
				uInt16		*VMErrCode)
{
	LVBoolean	errf = FALSE;
	MgErr		err;

	*VMErrCode = 0;
	*VIErrCode = 0;

	switch(*action) {
		default:
		case 0:	return noErr;

		case 1:	err = vmCmd_vcNames_Reset( &errf, VMErrCode );
			break;

		case 2:	err = vmCmd_vcNames_Assign( VC_Handle, VC_Name, &errf, VMErrCode );
			break;

		case 3:	err = vmCmd_vcNames_GetName( VC_Handle, VC_Name, &errf, VMErrCode );
			break;

		case 4:	err = vmCmd_vcNames_FindName( VC_Handle, VC_Name, &errf, VMErrCode );
			break;
	}

	if (err)
		vm_LVError(err, &errf, VMErrCode);

	switch(*VMErrCode) {
		case VM_ERR_NOERROR :		*VIErrCode = 0;	break;
		case VM_ERR_VCNAME_INVNAME :	*VIErrCode = 1;	break;
		case VM_ERR_VCNAME_NOTFOUND :	*VIErrCode = 2;	break;
		case VM_ERR_VCNAME_NONUNIQUE :	*VIErrCode = 3;	break;
		case VM_ERR_CTX_INVHANDLE :	*VIErrCode = 4;	break;
		default :			*VIErrCode = 5;	break;
	}
	return err;
}

