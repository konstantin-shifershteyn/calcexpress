/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr	CEKernel_Context_Destroy(_CE_Instance *ip)
{
	if (__CECheckReference(ip))
		goto out;

	// ============================
	// *** clear 'signature' field
	// ============================
	ip->signature = 0;

	// destroy stack of operands
	// ========================
	OpdStack_DestroyStack(ip);

	// destroy stack of operations
	// =======================
	OpsStack_DestroyStack(ip);

	// destroy stack of functions
	// ======================
	FIOStack_DestroyStack(ip);

	// destroy array of expression items
	// ====================================
	ExAr_Destroy(ip);

	// destroy strings
	// ================
	DSDisposeHandle( ip->str );
	DSDisposeHandle( ip->ErrMsg );

	// destroy temporary operands
	// ============================
	DSDisposeHandle( ip->LOpd.str );
	DSDisposeHandle( ip->ROpd.str );

	// ===================
	// destroy context
	// ===================
	DSDisposePtr(ip);

	// ======================
	// destroy LabVIEW RefNum
	// ======================
	{
		LVRefNum	refnum;
		MgErr		err = noErr;
		refnum = ceref_GetRefnum((LVRefNumData)ip,  &err);
		if (refnum)
			ceref_Destroy(refnum, &err);
	}
out:
	return noErr;
}

// =================================================================
//	Destroy all CSE Instance Contexts
//	return: number of instances destroyed
// =================================================================
int32	CEKernel_Context_DestroyAll()
{
	_CE_Instance	*CEInstanceRef;
	LVBoolean		errf = LVFALSE;
	MgErr			err  = noErr;
	int32			num = 0;

	// ***	Destroy all dynamically created CE Instances
	// --------------------------------------------------
	while(!cerstor(cersCmd_TagDelAny, cersOwner_CEInstance, (size_t*)&CEInstanceRef)) {
		num++;
		// *****  Lock Context
		if (_CEContext_Lock(CEInstanceRef)) {
		//if (ceRsAcMg_Pnt(cermCmd_LockRsrc, (__cersamg_RsrcHeader*)CEInstanceRef, CE_CONTEXT_SIGNATURE)) {

			// *****  if locked, destroy instance context
			errf = LVFALSE;
			err  = cekrnl(CEK_CMD_Cntx_Destroy, &CEInstanceRef, NULL, NULL, &errf);

			// !!!!!  No need to release context because it doesn't exist anymore
		}
	}
	return num;
}
