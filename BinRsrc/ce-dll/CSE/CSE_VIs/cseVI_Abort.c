/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// CSE Script Processor
// VI Interface:		Access to Abort Trigger
// -----------------------------------

#include <extcode.h>

#include <CSE.h>
#include <CSE_AccessContext.h>
#include <RsrcManager\RsrcManager.h>


enum	{
	actTrigger = 0,
	actReset,
	actCheck,
	actFlip
};

// =================================================================

MgErr	__cdecl	cseVIAbort(	
				LVRefNum	*pRefNum,	// iPScrInstancePnt	*iCSE_Hdl,
				int32		action,
				LVBoolean	*state,
				int32		*ExecStatus)
{
	iPScrInstance		*iCSE, *iTMP;
	MgErr				err = noErr;

	if ( *ExecStatus )
		return noErr;

	// ***** Convert LVRefNum to CSE Instance Reference (CE 3.0)
	if ( !(iCSE = (iPScrInstance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	if (action == actTrigger) {
		// ========================================
		//  'Trigger Abort' action
		//	!!! ¬€œŒÀÕﬂ≈“—ﬂ ¿—»Õ’–ŒÕÕŒ !!!
		// ========================================
		if ( ! (iTMP = CSE_Context_Abort(iCSE)) ) {
err_InstanceReference:
			// *** IF INVALID INSTANCE REFERENCE ***
			*ExecStatus = estInvalidRefNum;
			*state = LVFALSE;
			return noErr;
		}
		*state = LVTRUE;

		// ***	Set Cancel Trigger in all called CSE instances
		// ***	It is risky because of no synchronization
		//	while(iTMP = CSE_Context_Abort(iTMP->rtlCallee)) ;

		// ***	To spread abort signal to all cellers and callees CSE instances
		// ***	It is risky because of no synchronization
		for( iTMP = iCSE; iTMP = CSE_Context_Abort(iTMP->rtlCallee); ) ;
		for( iTMP = iCSE; iTMP = CSE_Context_Abort(iTMP->rtlCaller); ) ;

		return noErr;
	}

	// ---------------------------------------
	// ***	«‡ı‚‡ÚËÚ¸ CSE Instance Context
	// ---------------------------------------
	if (!CSE_Context_Lock(iCSE)) {
		*ExecStatus	= estInvalidRefNum;
		return noErr;
	}

	switch(action) {
//		case actTrigger :	iCSE->CancelTag = TRUE;	*state = LVTRUE;
//					break;
		case actReset :		iCSE->CancelTag = FALSE;	*state = LVFALSE;
					break;
		case actCheck :		*state = (LVBoolean)(iCSE->CancelTag);
					break;
		case actFlip :		*state = (LVBoolean)(iCSE->CancelTag);
					iCSE->CancelTag = FALSE;
					break;
	}

//out:
	// -------------------------------------
	// ***	ŒÒ‚Ó·Ó‰ËÚ¸ CSE Instance Context
	// -------------------------------------
	CSE_Context_UnlockReliable(iCSE);

	return noErr;
}
