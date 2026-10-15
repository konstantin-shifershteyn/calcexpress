/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// =================================================
// Возвращает указатель на элемент массива выражений
// =================================================
CSE_PRIVATEFUNC
iPScrElement * iPScrArray_GetPnt(_ScrParserInst *pi, int32 idx)
{
	if (idx > pi->PScrIdx)	return	NULL;
	if (idx == -1)		idx = pi->PScrIdx;
	return iPScrEPnt(*(pi->iPScr), idx);
}

// =========================================
// Создает новый элемент массива выражений
// =========================================
CSE_PRIVATEFUNC
MgErr iPScrArray_NewElement(
				_ScrParserInst	*pi,
				int32			stmType,
				int32			stmOffset,
				int32			expOffset)
{
	int32			size;
	iPScrElement	*pe;

	size = iPScrSize(*(pi->iPScr));
	if (pi->PScrIdx >= (size - 1)) {
		MgErr	err;
		if (err = iPScr_ResizeArray(&(pi->iPScr), size + PSCR_BLKSIZE))
			return err;
	}

	pi->PScrIdx += 1;

	pe = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);

	pe->ip			= NULL;
	pe->stmType		= stmType;
	pe->jumpAction	= JA_JUMP;		// jump
	pe->jumpIndex	= -1;			// to next (на следующий)
	pe->NestingLevel= pi->NestingLevel;	// loop nesting level

	// ***	init statement info
	pe->stmOffset	= stmOffset;
	pe->stmLength	= 0;
	pe->stmLine	= pi->curLine;

	// ***	init expression info
	pe->expLength	=  0;
	if (stmType == STMTYPE_EXPRESS) {
		if (expOffset >= 0)	pe->expOffset	= expOffset;
		else				pe->expOffset	= stmOffset;
		pe->expLine	= pi->curLine;
	}
	else {
		pe->expOffset	= -1;
		pe->expLine		= -1;
	}

	// ***	init 
	pe->expNote		= "";	// static !!!
	pe->parentIndex	= -1;	// none

	return noErr;
}

// ============================================================
// *****	Functions create new element of a parsed script
// ============================================================

CSE_PRIVATEFUNC
MgErr iPScrArray_NewExpression(_ScrParserInst *pi, int32 offset)
{
	return	iPScrArray_NewElement(pi, STMTYPE_EXPRESS, offset, offset);
}
// -----
CSE_PRIVATEFUNC
MgErr iPScrArray_NewExpressionEx(_ScrParserInst *pi, int32 stmOffset, int32 expOffset)
{
	return	iPScrArray_NewElement(pi, STMTYPE_EXPRESS, stmOffset, expOffset);
}
// -----
CSE_PRIVATEFUNC
MgErr iPScrArray_NewSourceReference(_ScrParserInst *pi, int32 offset)
{
	return	iPScrArray_NewElement(pi, STMTYPE_SRCREF, offset, -1);
}
// -----
CSE_PRIVATEFUNC
MgErr iPScrArray_NewJumpStatement(_ScrParserInst *pi, int32 offset)
{
	return	iPScrArray_NewElement(pi, STMTYPE_JUMP, offset, -1);
}
// -----
CSE_PRIVATEFUNC
MgErr iPScrArray_NewJumpStub(_ScrParserInst *pi, int32 offset)
{
	return	iPScrArray_NewElement(pi, STMTYPE_JUMPSTUB, offset, -1);
}

// ============================================================
// *****	Functions finilize element of a parsed script
// ============================================================

CSE_PRIVATEFUNC
void iPScrArray_FinishExpression(_ScrParserInst *pi, int32 offset)
{
	iPScrElement	*pe = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);

	if (pe->stmType != STMTYPE_EXPRESS)
		return;
	pe->expLength	= offset - pe->expOffset;
	pe->stmLength	= offset - pe->stmOffset;
}
// -----
CSE_PRIVATEFUNC
void iPScrArray_FinishExpressionEx(_ScrParserInst *pi, int32 stmOffset, int32 expOffset)
{
	iPScrElement	*pe = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);

	if (pe->stmType != STMTYPE_EXPRESS)
		return;
	pe->stmLength	= stmOffset - pe->stmOffset;
	pe->expLength	= expOffset - pe->expOffset;
}
// -----
CSE_PRIVATEFUNC
void iPScrArray_FinishSourceReference(_ScrParserInst *pi, int32 offset)
{
	iPScrElement	*pe = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);

	pe->stmType	= STMTYPE_SRCREF;
	pe->stmLength	= offset - pe->stmOffset;

	pe->expOffset	= -1;
	pe->expLength	=  0;
	pe->expLine		= -1;
}
// -----
CSE_PRIVATEFUNC
void iPScrArray_FinishJumpStatement(_ScrParserInst *pi, int32 offset)
{
	iPScrElement	*pe = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);

	pe->stmType		= STMTYPE_JUMP;
	pe->stmLength	= offset - pe->stmOffset;

	pe->expOffset	= -1;
	pe->expLength	=  0;
	pe->expLine		= -1;
}
// -----
CSE_PRIVATEFUNC
void iPScrArray_FinishJumpStub(_ScrParserInst *pi, int32 offset)
{
	iPScrElement	*pe = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);

	pe->stmType	= STMTYPE_JUMPSTUB;
	pe->stmLength	= offset - pe->stmOffset;

	pe->expOffset	= -1;
	pe->expLength	=  0;
	pe->expLine		= -1;
}

// ============================================================
// *****	Functions update elements of parsed script
// ============================================================

CSE_PRIVATEFUNC
void iPScrArray_UpdateExpressionOffset(_ScrParserInst *pi, int32 offset)
{
	iPScrElement	*pe = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);

	if (pe->stmType != STMTYPE_EXPRESS)
		return;
	pe->expOffset	= offset;
	pe->expLine		= pi->curLine;
}
// -----
CSE_PRIVATEFUNC
void iPScrArray_UpdateSourceReferenceOffset(_ScrParserInst *pi, int32 offset)
{
	iPScrElement	*pe = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);
	pe->stmOffset	= offset;
	pe->stmLine		= pi->curLine;
}
// -----
CSE_PRIVATEFUNC
void iPScrArray_UpdateSourceReferenceLength(_ScrParserInst *pi, int32 offset)
{
	iPScrElement	*pe = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);
	pe->stmLength	= offset - pe->stmOffset;
}
// -----
CSE_PRIVATEFUNC
void iPScrArray_UpdateLineNumbers(_ScrParserInst *pi, int32 stmLineNum, int32 expLineNum)
{
	iPScrElement	*pe = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);
	if (stmLineNum > 0)	pe->stmLine = stmLineNum;
	if (expLineNum > 0)	pe->expLine = expLineNum;
}
// -----
CSE_PRIVATEFUNC
void iPScrArray_SetParentAndNote(_ScrParserInst *pi, int32 parentIndex, char *expNote)
{
	iPScrElement	*pe = iPScrEPnt(*(pi->iPScr), pi->PScrIdx);
	pe->expNote		= expNote;
	pe->parentIndex	= parentIndex;
}

// ==============================================================================
// *****	Functions to create and complete an element of a parsed script
// ==============================================================================

CSE_PRIVATEFUNC
MgErr iPScrArray_CreateExpression(_ScrParserInst *pi, int32 begOff, int32 endOff)
{
	MgErr	err;
	if ( !(err = iPScrArray_NewExpression(pi, begOff)) ) {
		iPScrArray_FinishExpression(pi, endOff);
	}
	return err;
}
// -----
CSE_PRIVATEFUNC
MgErr iPScrArray_CreateSourceReference(_ScrParserInst *pi, int32 begOff, int32 endOff)
{
	MgErr	err;
	if ( !(err = iPScrArray_NewSourceReference(pi, begOff)) ) {
		iPScrArray_FinishSourceReference(pi, endOff);
	}
	return err;
}
// -----
CSE_PRIVATEFUNC
MgErr iPScrArray_CreateJumpStatement(_ScrParserInst *pi, int32 begOff, int32 endOff)
{
	MgErr	err;

	if ( !(err = iPScrArray_NewJumpStatement(pi, begOff)) ) {
		iPScrArray_FinishJumpStatement(pi, endOff);
	}
	return err;
}
// -----
CSE_PRIVATEFUNC
MgErr iPScrArray_CreateJumpStub(_ScrParserInst *pi, int32 offset)
{
	int32	begOff;
	MgErr	err;

	if ((begOff = offset) > 0)
		begOff	-= 1;

	if ( !(err = iPScrArray_NewJumpStub(pi, begOff)) ) {
		iPScrArray_FinishJumpStub(pi, offset);
	}
	return err;
}
