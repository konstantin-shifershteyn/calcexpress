/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <extcode.h>

#include <ceKernel.h>
#include <CSE.h>
#include <CSE_AccessContext.h>
#include <RsrcManager.h>
#include <celib\ceMemLib.h>

// =================================================================
// Minimize memory allocated during execution of a single
// operator (CalcExpress instance), doesn't destroy 'parsed expresssion'
// =================================================================
MgErr	iPScr_MinElement_RTData(iPScrElement *pe)
{
	if (pe) {
		if (pe->ip)
			CEKernel_Context_FreeMem(pe->ip, FALSE);
	}
	return noErr;
}
// =================================================================
// Minimize memory allocated during execution of all
// operators (CalcExpress instance), doesn't destroy 'parsed expresssion'
// =================================================================
MgErr	iPScr_MinElements_RTData( iPScrArrayHdl ipsa)
{
	int32	idx, size;

	if (!ipsa)
		return mZoneErr;

	size = iPScrSize(*ipsa);
	for (idx = 0; idx < size; idx++) {
		iPScr_MinElement_RTData(iPScrEPnt(*ipsa, idx));
	}
	return noErr;
}

// =================================================================
//	Release an item of 'statements array'
// =================================================================
MgErr	iPScr_FreeElement(iPScrElement *pe)
{
	LVBoolean	errf = LVFALSE;
	MgErr		err = noErr;

	if (!pe)
		return noErr;

	if (pe->ip) {
		err = cekrnl(CEK_CMD_Cntx_Destroy, &(pe->ip), NULL,NULL, &errf);
		pe->ip = NULL;
	}

	pe->expOffset	= pe->stmOffset	= -1;
	pe->expLength	= pe->stmLength	= 0;
	pe->expLine	= pe->stmLine	= -1;

	pe->expNote	= "";	// static !!!
	pe->parentIndex	= -1;	// none
	return err;
}

// =================================================================
//	Release items of 'statements array'
// =================================================================
MgErr	iPScr_FreeElements( iPScrArrayHdl ipsa, int32 idx)
{
	int32	size;

	if (!ipsa)
		return mZoneErr;

	size = iPScrSize(*ipsa);
	for ( ; idx < size; idx++) {
		iPScr_FreeElement(iPScrEPnt(*ipsa, idx));
	}
	return noErr;
}

// =================================================================
//	Resize 'statements array'
// =================================================================
MgErr	iPScr_ResizeArray( iPScrArrayHdl *ipsa, int32 newSize)
{
	int32	i, size;
	MgErr	err = noErr;

	size = iPScrSize(**ipsa);

	// ### if new size is less than current size, release discarded items
	if (newSize < size) {
		iPScr_FreeElements(*ipsa, newSize);
	}

	// ### RESIZE
//	if ( err = ce_1DArrayResize((UHandle*)ipsa, newSize, sizeof(iPScrElement)) )
	if ( err = ce_CreateOrResizeAlignedArray((UHandle*)ipsa, newSize, sizeof(iPScrElement), 8, offsetof(iPScrArray,buff)) )	// >>> #CE_3.0.4 2026-07-09
		goto out;

	// ### if new size is greater than current size, initialize new items
	for (i=size; i < newSize; i++) {
		iPScrEPnt(**ipsa,i)->ip = NULL;
		iPScr_FreeElement(iPScrEPnt(**ipsa,i));
	}
out:
	return err;
}

// =================================================================
//	Create 'statements array'
// =================================================================
/*
MgErr	iPScr_CreateArray(iPScrInstance *iCSE)
{
	iCSE->iPScr = (iPScrArrayHdl)DSNewHandle( sizeof(iPScrArray));
	if ( !(iCSE->iPScr))
		return  mFullErr;

	iPScrSize(*(iCSE->iPScr)) = 0;
	return  noErr;
}
*/
// >>> #CE_3.0.4 2026-07-09
MgErr	iPScr_CreateArray(iPScrInstance *iCSE)
{	
	iCSE->iPScr = NULL;
	return ce_CreateOrResizeAlignedArray((UHandle*)&(iCSE->iPScr), 0, sizeof(iPScrElement), 8, offsetof(iPScrArray,buff));
}
// <<< #CE_3.0.4 2026-07-09


// =================================================================
//	Destroy 'statements array'
// =================================================================
MgErr	iPScr_DestroyArray(iPScrInstance *iCSE)
{
	iPScr_FreeElements(iCSE->iPScr, 0);
	DSDisposeHandle(iCSE->iPScr);
	iCSE->iPScr = NULL;
	return noErr;
}


// ==========================================================================


// =================================================================
//	Create CSE Instance Context
// =================================================================
MgErr	CSE_InstCreate( iPScrInstancePnt *iCSE_Hdl)
{
	iPScrInstance	*iCSE;
	MgErr			err = noErr;

	// ### Allocate memory for CSE Instance
	iCSE = *iCSE_Hdl = (iPScrInstance*)ce_DSNewPClr( sizeof(iPScrInstance) );
	if (iCSE == NULL) {
		err = mFullErr; goto out;
	}

	if (err = iPScr_CreateArray(iCSE))				// *** Create array of parsed statements
		goto out;

	if (err = LStr_Create( &(iCSE->ScriptName), 0))	// *** Create Script Name string
		goto out;
	if (err = LStr_Create( &(iCSE->Script), 0) )	// *** Create Source Script string
		goto out;
	if (err = LStr_Create( &(iCSE->ErrMsg), 0) )	// *** Create Error Message string
		goto out;

	if (err = LStr_Create( &(iCSE->rtlCallbackVIPath), 0) )	// *** Create Callback VI Path string
		goto out;
	if (err = LStr_Create( &(iCSE->rtlScriptPath), 0) )	// *** Create Script Path string
		goto out;
	if (err = LStr_Create( &(iCSE->rtlSearchPaths), 0) )	// *** Create Search Paths string
		goto out;

	// ### Initialize signature and service fields
	iCSE->signature = CSE_CONTEXT_SIGNATURE;
	iCSE->SyncTag   = 0;
	iCSE->CancelTag = FALSE;
out:
	// ### create LabVIEW RefNum
	if (iCSE)	{
		LVRefNum	refnum;
		MgErr		err = noErr;
		refnum = ceref_Create((LVRefNumData)(iCSE),  cerefKind_CSEInstance,  &err);
	}
	return err;
}

// =================================================================
//	Release memory used by a CSE Instance Context
// =================================================================
MgErr	CSE_InstFreeMem( iPScrInstance *iCSE)
{
	LStr_Clear(iCSE->ScriptName);			// ***	Script Name string
	LStr_Clear(iCSE->Script);				// ***	Source Script string
	LStr_Clear(iCSE->ErrMsg);				// ***	Error Message

	LStr_Clear(iCSE->rtlCallbackVIPath);	// ***	Callback VI Path
	LStr_Clear(iCSE->rtlScriptPath);		// ***	Script Path string
	LStr_Clear(iCSE->rtlSearchPaths);		// ***	Search Paths string

	// ### Array of statements
	return iPScr_ResizeArray(&(iCSE->iPScr), 0);
}

// =================================================================
//	Destroy CSE Instance Context
// =================================================================
MgErr	CSE_InstDestroy( iPScrInstance *iCSE)
{
	LVRefNum	refnum;
	MgErr		err = noErr;

	iCSE->signature = 0;			// *** Reset signature

	LStr_Destroy( iCSE->ScriptName );		// *** Destroy Script Name string
	LStr_Destroy( iCSE->Script );			// *** Destroy Source Script string
	LStr_Destroy( iCSE->ErrMsg );			// *** Destroy Error Message string

	LStr_Destroy( iCSE->rtlCallbackVIPath);	// *** Destroy Callback VI Path string
	LStr_Destroy( iCSE->rtlScriptPath);		// *** Destroy Script Path string
	LStr_Destroy( iCSE->rtlSearchPaths);	// *** Destroy Search Paths string

	iPScr_DestroyArray(iCSE);		// *** Destroy Array of Parsed Statements

	DSDisposePtr(iCSE);				// *** Destroy Context

	// ### destroy LabVIEW RefNum
	refnum = ceref_GetRefnum((LVRefNumData)iCSE,  &err);
	if (refnum)
		ceref_Destroy(refnum, &err);

	return noErr;
}

// =================================================================
//	Destroy all CSE Instance Contexts
//	return: number of instances destroyed
// =================================================================
int32	CSE_DestroyAllInstances( )
{
	iPScrInstance	*iCSE;
	int32		num = 0;
	MgErr		err = noErr;


	// ### Destroy All dynamically created CSE Instances
	while(!cerstor(cersCmd_TagDelAny, CSE_RESSTOR_OWNER, (size_t*)&iCSE)) {
		num++;
		// *****  Lock Instance
		if (CSE_Context_Lock(iCSE)) {
//		if (ceRsAcMg_Pnt(cermCmd_LockRsrc, (__cersamg_RsrcHeader*)iCESP, CSE_CONTEXT_SIGNATURE)) {

			// *****  if locked, destroy instance context
			err = CSE_InstDestroy(iCSE);

			// !!!!!  No need to release context because it doesn't exist anymore
		}
	}
	return num;
}
