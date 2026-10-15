/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <ceKernel.h>
#include <CSE.h>
#include <VarMan.h>
#include <RsrcManager.h>

#include <celib\ceMemLib.h>


HINSTANCE	EngSts_ThisDllHandle = 0;


// =================================================
// ===	Dll Main
// =================================================

BOOL WINAPI DllMain( 	HINSTANCE	hinstThisDLL,	// handle to DLL module 
						DWORD		fdwReason,		// reason for calling function 
						LPVOID		lpvReserved)	// reserved 
{
	int32	num;

	switch(fdwReason) {
		case DLL_PROCESS_ATTACH :

			EngSts_ThisDllHandle = hinstThisDLL;

			cekrnl_Initializing();		// CalcExpress Kernel

			ceref_Initializing();		// LabVIEW RefNum Manager
			cersamg_Initializing();		// Resource Access Manager
			cerstor_Initializing();		// Resource Store Manager
			varman_Initializing();		// Variable Manager
			return TRUE;


		case DLL_PROCESS_DETACH :

			num = CSE_DestroyAllInstances();	// >>> 2024-12-02 : replacement for CINUnload()
			//DbgPrintf("Destroyed CSE instances: %d\n", num);

			num = CEKernel_Context_DestroyAll();	// >>> 2024-12-02 : replacement for CINUnload()
			//DbgPrintf("Destroyed CE instances: %d\n", num);

			varman_Finalizing();		// Variable Manager
			cerstor_Finalizing();		// Resource Store Manager
			cersamg_Finalizing();		// Resource Access Manager
			ceref_Finalizing();			// LabVIEW RefNum Manager
			cekrnl_Finalizing();		// CalcExpress Kernel

			return TRUE;

		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;
	}
	return TRUE;
}
 
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static	char* about_win_title = "About " PRODUCT_NAME;
static	char* about_product = PRODUCT_FULL_NAME;
static	char* about_version = "Version " PRODUCT_VERSION " " PRODUCT_RELEASE " (" __DATE__ ")";
static	char* about_copyright = COPYRIGHT;

// ================================================================
// Returns TRUE
// An idea is to call this function by "Call Library Function Node"
// with dynamic linking to CEEngine DLL in order to determine if
// it is in memory (i.e. any CE VI is loaded)
// ================================================================
uInt32	__cdecl	cestub(void)
{
	return 1;
}



// ================================================================
int32	BuildMainAboutText(LStrHandle msg)
{
	// ***	Product name, version and copyright
	return LStrPrintf(msg, (CStr)"%H%s\n%s\n\n%s\n", msg, about_product, about_version, about_copyright);
}

// ========================================
// Returns CE Engine DLL status
// intended for external call
// ========================================
uInt64	__cdecl	cemodstat(uInt32* LvSystem, uInt32* DemoMode, LStrHandle msgAbout)
{
	*LvSystem = 0;
	*DemoMode = 0;

	BuildMainAboutText(msgAbout);
	return	(uInt64)EngSts_ThisDllHandle;
}

// ========================================
// get license details from arbitrary CE License file
// intended for external call
// ========================================
MgErr	__cdecl cemodval(char* filePath, int32 cmd, Bool32* bad, int32* num, LStrHandle str)
{
	*bad = TRUE;
	*num = 0;
	return noErr;
}
