/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	CE DLL Instance
// -----------------------------------

#include <extcode.h>

#include <ceKernel.h>
#include <ceError.h>
#include <ceAccessContext.h>
#include <RsrcManager\RsrcManager.h>
#include <celib\ceMemLib.h>


// =================================================================
// CE DLL Instance control :	main function
// =================================================================
MgErr	__cdecl		viCE_DllInstance(
			InstanceDataPtr		*pDllInstanceData,
			LVRefNum			*pRefNum,	//_CE_InstancePnt		*InstanceRef,
			LVBoolean			*CreateNewInst,
			LVBoolean			*err_f)
{
	_CE_Instance	*ip;
	MgErr			err = noErr;

	// ----------------------------------------------------
	// ***	If a dynamic context is requested to be created
	// ----------------------------------------------------
	if (*CreateNewInst && !(*err_f)) {
		// *****  CREATE CONTEXT
		if ( !(err = cekrnl(CEK_CMD_Cntx_Create, &ip, NULL,NULL,err_f)) ) {
			// *****  SAVE IN RESOURCES STORE
			err = cerstor(cersCmd_TagInsert, cersOwner_CEInstance, (size_t*)&ip);
		}
	}
	else {
		// ---------------------------------------------------
		// ***	Use static context of the current DLL instance
		// ---------------------------------------------------
		size_t indat = *(size_t*)pDllInstanceData;
		ip = (_CE_Instance*)indat;
		if (!ip) {
			LVBoolean	errf = LVFALSE;
			if (!(err = cekrnl(CEK_CMD_Cntx_Create, &ip, NULL, NULL, &errf)) ) {
				*pDllInstanceData = ip;
			}
		}
		*CreateNewInst = LVFALSE;
	}
	if (err || !ip) {
		*CreateNewInst	= LVFALSE;
		*err_f			= LVTRUE;
	}

	*pRefNum = ceref_GetRefnum((LVRefNumData)ip,  &err);
	return err;
}


// =================================================================
// CE DLL Instance control :	"Reserve" callback function
// =================================================================
MgErr __cdecl	viCE_DllInstance_Reserve(InstanceDataPtr *pDllInstanceData)
{
	_CE_Instance	*ip	 = NULL;
	LVBoolean		errf = LVFALSE;
	MgErr			err  = noErr;

	*(size_t*)pDllInstanceData = (size_t)NULL;

	// ***	Create STATIC Instance Context
	// ----------------------------
	if ( !(err = cekrnl(CEK_CMD_Cntx_Create, &ip, NULL, NULL, &errf)) ) {
		*(size_t*)pDllInstanceData = (size_t)ip;
	}
	return err;
}

// =================================================================
// CE DLL Instance control :	"Unreserve" callback function
// =================================================================
MgErr __cdecl	viCE_DllInstance_UnReserve(InstanceDataPtr *pDllInstanceData)
{
	_CE_Instance	*ip;
	LVBoolean		errf = LVFALSE;
	MgErr			err  = noErr;

	ip = (_CE_Instance*)*pDllInstanceData;

	if (ceref_GetRefnum((LVRefNumData)ip,  NULL) != __NotRefNum) {	// if there is RefNum, pointer should be correct
		// *****  LOCK CONTEXT
		if (_CEContext_Lock(ip)) {
			// ***	if Instance Reference is valid ***
			// ***	Destroy Context
			err = cekrnl(CEK_CMD_Cntx_Destroy, &ip, NULL, NULL, &errf);

			// !!!!!  No need to release context because it doesn't exist anymore
		}
	}
	*(size_t*)pDllInstanceData = (size_t)NULL;

	return err;
}
