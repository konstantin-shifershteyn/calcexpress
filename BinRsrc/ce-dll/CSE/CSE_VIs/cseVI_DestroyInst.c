/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Destroy Context
// -----------------------------------

#include <extcode.h>

#include <CSE.h>
#include <CSE_AccessContext.h>
#include <RsrcManager.h>

// =================================================================

MgErr	__cdecl	cseVIDestroyInst(	
					LVRefNum	*pRefNum,		// iPScrInstancePnt	*iCSE_Hdl,
					LVBoolean	*Destroyed,
					int32		*ExecStatus)
{
	iPScrInstance	*iCSE;
	MgErr			err = noErr;

	// ***** Convert LVRefNum to CSE Instance Reference (CE 3.0)
	if ( !(iCSE = (iPScrInstance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// ----------------------------------------------
	// Если контекст зарегистрирован в Resource Store
	// (т.е. открыт динамически), то разрушить его,
	// иначе освободить (уменьшить) занимаемую память
	// ----------------------------------------------
	*Destroyed  = !cerstor(cersCmd_TagRemove, CSE_RESSTOR_OWNER, (size_t*)&iCSE);
	*ExecStatus = estSuccess;

	// -------------------------
	// *****  Захватить контекст
	// -------------------------
	if (!CSE_Context_Lock(iCSE)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*ExecStatus = estInvalidRefNum;
		*Destroyed  = FALSE;
		return noErr;
	}

	if (*Destroyed)
			err = CSE_InstDestroy(iCSE);
	else	err = CSE_InstFreeMem(iCSE);

	// --------------------------------------------------------------
	// *****  Освободить контекст (ничего страшного, если уничтожили)
	// --------------------------------------------------------------
	CSE_Context_Unlock(iCSE);

	// *****  Return LabVIEW Manager Errors as CSE Execution Status
	if (err) {
		*ExecStatus = err;
	}
	return err;
}
