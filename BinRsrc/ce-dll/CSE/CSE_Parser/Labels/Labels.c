/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <CSE Parser.h>
#include <celib\ceMemLib.h>


#define	LABEL_DIRECTION_THRESHOLD	-10000

// --------------------
//CSE_PRIVATEFUNC
__forceinline _LabelInfo * LblTbl_GetPnt(_ScrParserInst *pi, int32 labelIdx)
{
	if (labelIdx >= pi->LblNum)
		return NULL;
	return	&((*(pi->LblTbl))->data[labelIdx]);
}
// ---------------------
// returns index of statement to which a label refers
//CSE_PRIVATEFUNC
__inline	int32 LblTbl_GetLabel(_ScrParserInst *pi, int32 labelIdx)
{
	_LabelInfo	*lp;

	lp = LblTbl_GetPnt(pi, labelIdx);
	if (lp) {
		return	lp->expIdx;
	}
	return -1;
}
// ---------------------
//CSE_PRIVATEFUNC
__inline	int32 LblTbl_SetLabel(_ScrParserInst *pi, int32 labelIdx, int32 expIdx)
{
	_LabelInfo	*lp;

	lp = LblTbl_GetPnt(pi, labelIdx);
	if (lp) {
		lp->expIdx = expIdx;
		return	expIdx;
	}
	return -1;
}

// --------------------
CSE_PRIVATEFUNC
MgErr LblTbl_CreateTable(_ScrParserInst *pi)
{
	MgErr	err = noErr;

	pi->LblTbl = (_LabelInfoArrayHdl)DSNewHandle(sizeof(_LabelInfoArray) + sizeof(_LabelInfo) * LBL_BLKSIZE);

	if (!(pi->LblTbl)) {
		err = mFullErr;
		goto out;
	}
	(*(pi->LblTbl))->dimSize = LBL_BLKSIZE;	// размер массива
out:
	pi->LblNum = 0;
	return err;
}

// --------------------
CSE_PRIVATEFUNC
MgErr LblTbl_DestroyTable(_ScrParserInst *pi)
{
	int32		i;
	_LabelInfo	*lp;
	MgErr		err = noErr;

	if (pi->LblTbl) {
		for(i=0; i < pi->LblNum; i++) {
			lp = LblTbl_GetPnt(pi, i);
			if (lp) {
				if (lp->name) {
					DSDisposePtr(lp->name);
					lp->name = NULL;
				}
			}
		}
		err = DSDisposeHandle(pi->LblTbl);
	}
	pi->LblTbl = NULL;
	pi->LblNum = 0;
	return err;
}

// --------------------
CSE_PRIVATEFUNC
MgErr LblTbl_NewLabel(_ScrParserInst *pi, int32 *pLabelIdx)
{
	_LabelInfo	*lp;
	MgErr		err = noErr;

	*pLabelIdx	= pi->LblNum;		// save index of new label !!!
	pi->LblNum	+= 1;				// увеличить счетчик элементов

	// Если таблица заполнена, то расширить
	// ------------------------------------
	if (*pLabelIdx  >= (*(pi->LblTbl))->dimSize) {
		int32	size	= (*(pi->LblTbl))->dimSize;
		int32	newSize = size + LBL_BLKSIZE;

		if (err = ce_1DArrayResize((UHandle*)&(pi->LblTbl), newSize, sizeof(_LabelInfo)) )
			goto out;
	}
	// заполнить элемент таблицы
	// -------------------------
	lp = LblTbl_GetPnt(pi, *pLabelIdx);
	lp->expIdx = -1;
	lp->name   = NULL;
	lp->nLine  = pi->curLine;
out:
	return err;
}

// ---------------------
CSE_PRIVATEFUNC
MgErr NLabel_SetOrCreateLabel(
				_ScrParserInst	*pi,
				int32			*pLabelIdx,
				char			*namePnt,
				int32			nameLen,
				int32			expIdx,
				LVBoolean		*err_f)
{
	int32		i;
	_LabelInfo	*lp;
	MgErr		err = noErr;

	if (expIdx < 0)	expIdx = pi->PScrIdx;

	// поиск метки
	// -----------
	for(i=0; i < pi->LblNum; i++) {
		if (!(lp = LblTbl_GetPnt(pi, i))) break;
		if (!(lp->name)) continue;
		// нашли именованную метку
		if (strlen(lp->name) != (size_t)nameLen)	continue;	// если другое имя
		if (StrNCmp(lp->name, namePnt, nameLen))	continue;	// если другое имя

		// нашли нашу метку
		*pLabelIdx = i;
		if (lp->expIdx >= 0) {
			// Если метка уже имеет значение
			err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, FALSE,
					"Label '%s' is already defined in line %d", lp->name, (void*)(size_t)lp->nLine);

			*err_f = LVTRUE;
			goto out;
		}
		// были ссылки на метку, но значение еще не определено
		lp->expIdx = expIdx;
		lp->nLine  = pi->curLine;
		goto out;
	}
	// -------------
	// Создать метку
	// -------------
	if (err = LblTbl_NewLabel(pi, pLabelIdx))  goto out;
	lp = LblTbl_GetPnt(pi, *pLabelIdx);
	lp->expIdx = expIdx;
	lp->nLine  = pi->curLine;
	lp->name   = ce_DSNewPClr(nameLen+1);
	if (!(lp->name)) {
		err = mFullErr;	goto out;
	}	
	StrNCpy(lp->name, namePnt, nameLen);
out:
	return err;
}

// ---------------------
CSE_PRIVATEFUNC
MgErr NLabel_GetOrCreateLabel(
				_ScrParserInst	*pi,
				int32			*pLabelIdx,
				char			*namePnt,
				int32			nameLen,
				LVBoolean		*err_f)
{
	int32		i;
	_LabelInfo	*lp;
	MgErr		err = noErr;

	// -----------
	// поиск метки
	// -----------
	for(i=0; i < pi->LblNum; i++) {
		if (!(lp = LblTbl_GetPnt(pi, i))) break;
		if (!(lp->name)) continue;
		// нашли именованную метку
		if (strlen(lp->name) != (size_t)nameLen)	continue;	// если другое имя	>>> #CE_3.0 2025-05-02 : BUGFIX
		if (StrNCmp(lp->name, namePnt, nameLen))	continue;	// если другое имя

		// нашли нашу метку
		*pLabelIdx = i;
		goto out;
	}
	// -------------
	// Создать метку
	// -------------
	if (err = LblTbl_NewLabel(pi, pLabelIdx))  goto out;
	lp = LblTbl_GetPnt(pi, *pLabelIdx);
	lp->name   = ce_DSNewPClr(nameLen+1);
	if (!(lp->name)) {
		err = mFullErr;	goto out;
	}	
	StrNCpy(lp->name, namePnt, nameLen);
out:
	return err;
}

// ===========================================================================

CSE_PRIVATEFUNC
MgErr  ConvertLabelsToValues(	_ScrParserInst	*pi, LVBoolean	*err_f)
{
	iPScrElement	*pe;
	_LabelInfo		*lp;
	char			*ln;
	int32			i, line, labelIdx;
	MgErr			err = noErr;

	for (i=0; i <= pi->PScrIdx; i++) {

		pe = iPScrEPnt(*(pi->iPScr), i);

// ???????
		if (pe->jumpAction == JA_STOP) {
			pe->jumpIndex = pi->PScrIdx + 1;
			continue;
		}

		if (pe->jumpIndex < 0) {	// нет метки
			if (pe->jumpIndex > LABEL_DIRECTION_THRESHOLD) {
				// jump forward
				pe->jumpIndex = i + abs(pe->jumpIndex);
			}
			else {
				// jump backward
				pe->jumpIndex = i - abs(pe->jumpIndex - LABEL_DIRECTION_THRESHOLD);
			}
			continue;
		}

		// ***	look for label
		labelIdx = pe->jumpIndex;
		pe->jumpIndex = LblTbl_GetLabel(pi, labelIdx);
		if (pe->jumpIndex >= 0)
			continue;		// OK, label is found !!!

		// ***	undefined label (значение метки не определено)
		// --------------------
		if (lp = LblTbl_GetPnt(pi, labelIdx))
			 {	ln = lp->name;	line = lp->nLine;	}
		else {	ln = NULL;		line = pi->curLine;	}

		// ***	Error Message
		if (ln)	err = CSE_MakeErrorMessage(pi->iCSE, i, estSyntaxError, TRUE, "Undefined label '%s'", ln, NULL);
		else	err = CSE_MakeErrorMessage(pi->iCSE, i, 0, FALSE, "Undefined temporary label #%d", (void*)(size_t)labelIdx, NULL);

		pi->iCSE->ErrIndex = i;	// save index of wrong statement
		*err_f = LVTRUE;		// error flag
		break;				// exit
	}
	return	err;
}
