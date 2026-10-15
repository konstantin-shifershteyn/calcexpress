/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		CSE DLL Instance 
// -----------------------------------

#include <extcode.h>

#include <CSE.h>
#include <CSE_AccessContext.h>
#include <RsrcManager.h>


// =================================================================
// CSE DLL Instance control :	main function
// =================================================================

MgErr __cdecl	cseVI_DllInstance(
			InstanceDataPtr		*pDllInstanceData,
			LVRefNum			*pRefNum,	//iPScrInstancePnt	*iCSE_Hdl,
			LVBoolean			*CreateNewInst,
			int32				*ExecStatus)
{
	iPScrInstance	*iCSE;
	MgErr			err = noErr;

	// ----------------------------------------------------
	// ***	If a dynamic context is requested to be created
	// ----------------------------------------------------
	if (*CreateNewInst && (*ExecStatus == estSuccess)) {
		// *****  CREATE CONTEXT
		if ( !(err = CSE_InstCreate(&iCSE)) ) {
			// *****  SAVE IN RESOURCES STORE
			err = cerstor(cersCmd_TagInsert, CSE_RESSTOR_OWNER, (size_t*)&iCSE);
		}
	}
	else {
		// ---------------------------------------------------
		// ***	Use static context of the current CIN instance
		// ---------------------------------------------------
		//*iCESP_Hdl = (iPScrInstance*)GetDSStorage();
		iCSE = (iPScrInstance*)*(pDllInstanceData);
		if (!iCSE) {
			if ( !(err = CSE_InstCreate(&iCSE)) ) {
				//SetDSStorage((int32)*iCESP_Hdl);
				*pDllInstanceData = iCSE;
			}
		}
		*CreateNewInst = LVFALSE;
	}

	if (err || !iCSE) {
		*CreateNewInst	= LVFALSE;
		*ExecStatus		= err ? err : mFullErr;	//estInvalidRefNum;
	}
	*pRefNum = ceref_GetRefnum((LVRefNumData)iCSE,  &err);
	return noErr;
//	return err;
}


// =================================================================
// CSE DLL Instance control :	"Reserve" callback function
// =================================================================

MgErr __cdecl	cseVI_DllInstance_Reserve(InstanceDataPtr *pDllInstanceData)
{
	iPScrInstance	*iCSE	= NULL;
	MgErr		err	= noErr;

	*pDllInstanceData = NULL;

	// ***	Create STATIC Instance Context
	// ----------------------------
	if ( !(err = CSE_InstCreate(&iCSE)) ) {
		//SetDSStorage( (int32)iCESP );
		*pDllInstanceData = iCSE;
	}
return err;
}


// =================================================================
// CSE DLL Instance control :	"Unreserve" callback function
// =================================================================

MgErr __cdecl	cseVI_DllInstance_UnReserve(InstanceDataPtr *pDllInstanceData)
{
	iPScrInstance	*iCSE;
	MgErr			err = noErr;


//	iCSE = (iPScrInstance*)GetDSStorage();
	iCSE = (iPScrInstance*)*pDllInstanceData;

	if (ceref_GetRefnum((LVRefNumData)iCSE,  NULL) != __NotRefNum) {	// if there is RefNum, pointer should be correct
		// *****  Lock STATIC Context
		if (CSE_Context_Lock(iCSE)) {
			// ***	if Instance Reference is valid ***
			// ***	Destroy STATIC Context
			err = CSE_InstDestroy(iCSE);

			// !!!!!  No need to release context because it doesn't exist anymore
		}
	}
//	SetDSStorage(0);
	*pDllInstanceData = NULL;

	return err;
}

// ================================================================

