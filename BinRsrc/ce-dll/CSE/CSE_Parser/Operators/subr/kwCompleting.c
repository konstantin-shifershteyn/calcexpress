/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__kwCompleting_C__
#define	__kwCompleting_C__

#include "TestForElse.c"

// ----------------------------------------------
CSE_PRIVATEFUNC
Bool32	kwComplete_if(
				_ScrParserInst	*pi,
				_OprInfo		*opp,
				int32			cOff )	// !!! not pointer
{
	Bool32	ThereIsElse;
	int32	labelValue = pi->PScrIdx + 1;

	// значения меток установить на следующее подвыражение
	LblTbl_SetLabel(pi, opp->label1, labelValue);
	LblTbl_SetLabel(pi, opp->label2, labelValue);

	// определить есть ли else у этого if
	ThereIsElse = TestForElse(pi, cOff);

	if (!ThereIsElse)
		pi->NestingLevel -= 1;

	return	ThereIsElse;
}

// ----------------------------------------------
CSE_PRIVATEFUNC
MgErr	kwComplete_else(
				_ScrParserInst	*pi,
				_OprInfo		*opp)
{
	// значения метки конца if (label2)
	LblTbl_SetLabel(pi, opp->label2, pi->PScrIdx + 1);

	pi->NestingLevel -= 1;

	return	noErr;
}

// ----------------------------------------------
CSE_PRIVATEFUNC
MgErr	kwComplete_while(
				_ScrParserInst	*pi,
				_OprInfo		*opp,
				int32			cOff )
{
	iPScrElement	*pe;
	MgErr			err;

	// ***	создать переход на условное выражение
	if (err = iPScrArray_CreateJumpStub(pi, cOff))
		return err;
	pe = iPScrArray_GetPnt(pi, -1);
	pe->jumpIndex = opp->label1;		// на условное выражение

	// ***	метку выхода установить на следующее подвыражение
	LblTbl_SetLabel(pi, opp->label2, pi->PScrIdx + 1);

	pi->NestingLevel -= 1;

	return	noErr;
}

// ----------------------------------------------
CSE_PRIVATEFUNC
MgErr	kwComplete_for(
				_ScrParserInst	*pi,
				_OprInfo		*opp,
				int32			cOff )
{
	iPScrElement	*pe;
	MgErr			err;

	// ***	создать переход на loop-exp выражение
	if (err = iPScrArray_CreateJumpStub(pi, cOff))
		return err;
	pe = iPScrArray_GetPnt(pi, -1);
	pe->jumpIndex = opp->label1;		// на условное выражение

	// ***	метку выхода установить на следующее подвыражение
	LblTbl_SetLabel(pi, opp->label2, pi->PScrIdx + 1);

	pi->NestingLevel -= 1;

	return	noErr;
}

// ----------------------------------------------
CSE_PRIVATEFUNC
MgErr	kwCompleting(
			_ScrParserInst	*pi,
			int32			cOff )	// !!! not a pointer
{
	_OprInfo	*opp;
	MgErr		err = noErr;

	while (opp = OprStack_TopPnt(pi)) {
		switch (opp->code) {
			default:
					goto out;	// конец если в стеке не kw
			case OP_IF :
					if (kwComplete_if(pi, opp, cOff))	goto out; // выход если есть else
					break;
			case OP_ELSE :
					if (err = kwComplete_else(pi, opp))	goto out;
					break;
			case OP_FOR :
					if (err = kwComplete_for(pi, opp, cOff))	goto out;
					break;
			case OP_DOWHILE:
			case OP_WHILE :	if (err = kwComplete_while(pi, opp, cOff))	goto out;
					break;
		}
		OprStack_DecLen(pi);	// удалить текущий kw из стека
	}
out:
	return err;
}

// -------------------------------------------------
// The function does not complete Compound-Statement
// -------------------------------------------------
CSE_PRIVATEFUNC
MgErr	kwCompleteTopLevelStatement( _ScrParserInst *pi, int32 cOff)
{
	_OprInfo	*opp;

	if (opp = OprStack_TopPnt(pi)) {	// stack is not empty
		switch (opp->code) {
			case OP_IF :
			case OP_ELSE :
			case OP_FOR :
			case OP_WHILE :
			case OP_DOWHILE :
					// !!!CAN ADD ELEMENT(S) TO pi->iPScr
					return	kwCompleting(pi, cOff);
		}
	}
	return noErr;
}

// -------------------------------------------------
// Is called to complete expression statements
// The function does not complete Compound-Statement
// -------------------------------------------------
CSE_PRIVATEFUNC
MgErr	kwCompleteTopLevelStatement_EXP( _ScrParserInst *pi, int32 cOff)
{
	_OprInfo	*opp;

	if (opp = OprStack_TopPnt(pi)) {	// stack is not empty
		switch (opp->code) {
			case OP_IF :
			case OP_ELSE :
			case OP_FOR :
			case OP_WHILE :
			case OP_DOWHILE :
					// !!!CAN ADD ELEMENT(S) TO pi->iPScr
					pi->fCanBeGrouped = FALSE;
					return	kwCompleting(pi, cOff);
		}
	}
	pi->fCanBeGrouped = pi->iCSE->fGroupExpStm;	// TRUE is general permission
	return noErr;
}

#endif
