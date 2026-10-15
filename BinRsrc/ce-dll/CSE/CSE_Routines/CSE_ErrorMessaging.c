/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <extcode.h>

#include <celib\ceMemLib.h>
#include <CSE.h>

// ---------------------------

MgErr	CSE_MakeErrorMessage(	iPScrInstance	*iCSE,		// context
				int32		stmIndex,	// statement index
				int32		ErrType,	// 0,estSyntaxError,estRuntimeError,estCancelled
				Bool32		fSourceInfo,	// include source info
				CStr		cfmt,		// format string
				void		*par1,
				void		*par2 )
{
	iPScrElement	*pe;
	int32		len, oldLen;
	LStrHandle	msg	= NULL;
	MgErr		err	= noErr;

//return LStrPrintf(iCSE->ErrMsg, "ERR:\n" "StmIndex = %d\n" "Size = %d", stmIndex, iPScrSize(*(iCSE->iPScr)) );

	if ((stmIndex < 0) || (stmIndex >= iPScrSize(*(iCSE->iPScr))) )
		goto out;

	pe = iPScrEPnt(*(iCSE->iPScr), stmIndex);


	// ***	1. Insert Script Name (if any) and Line Number
	// ---------------------------------------------------
	if ( LStrLen(*(iCSE->ScriptName)) )
			err = LStrPrintf(iCSE->ErrMsg, "line %d (%H): "	, pe->stmLine, iCSE->ScriptName);
	else	err = LStrPrintf(iCSE->ErrMsg, "line %d: "	, pe->stmLine);
	if (err) goto out;


	// ***	2. Insert Error Type
	// -------------------------
	switch (ErrType) {
		case estSyntaxError :
//			err = LStrPrintf(iCSE->ErrMsg, "%HSyntax Error:\n", iCSE->ErrMsg);
			err = LStrPrintf(iCSE->ErrMsg, "%HParsing Error:\n", iCSE->ErrMsg);
			break;
		case estRuntimeError :
			err = LStrPrintf(iCSE->ErrMsg, "%HRuntime Error:\n", iCSE->ErrMsg);
			break;
		case estCancelled:
			err = LStrPrintf(iCSE->ErrMsg, "%HAborted execution ", iCSE->ErrMsg);
			break;
	}
	if (err) goto out;

	// ***	3. Add Custom Message
	// --------------------------
	if (cfmt) {
		if (err = LStr_Create(&msg, 0))					goto out;
		if (err = LStrPrintf(msg, cfmt, par1, par2))	goto out;
		if (err = LStr_Cat(iCSE->ErrMsg, msg))			goto out;
	}

	// ***	is it required to add source info?
	if ((iCSE->ErrSrcMaxLen <= 0) || !fSourceInfo)
		goto out;

	// ***	4. Add Source Info
	// -----------------------
	if (!(pe->expNote))	pe->expNote = "";	// !!! STUB !!!
//	if (strlen(pe->expNote) > 0) {			// >>> #CE_3.1.0-r6 2026-09-28
		if (err = LStrPrintf(iCSE->ErrMsg, "%H\n" "in %s:\n", iCSE->ErrMsg, pe->expNote))
			goto out;
//	}
	if ((pe->parentIndex > 0) && (pe->parentIndex < iPScrSize(*(iCSE->iPScr))) ) {
		pe = iPScrEPnt(*(iCSE->iPScr), pe->parentIndex);
	}

	len = Min(pe->stmLength, iCSE->ErrSrcMaxLen);

	oldLen = LStrLen(*(iCSE->ErrMsg));
	if (err = LStr_Resize(iCSE->ErrMsg, oldLen + len))	goto out;
	_MoveBlock( LStrBuf(*(iCSE->Script)) + pe->stmOffset, LStrBuf(*(iCSE->ErrMsg)) + oldLen, len);

	if (pe->stmLength > iCSE->ErrSrcMaxLen)
		if (err = LStrPrintf(iCSE->ErrMsg, "%H\n... (limited to %d characters)",
				iCSE->ErrMsg, iCSE->ErrSrcMaxLen))
			goto out;

out:
	if (msg)	DSDisposeHandle(msg);
	return err;
}
