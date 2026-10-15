/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

CSE_PRIVATEFUNC
Bool32	TestForElse(	_ScrParserInst	*pi,
			int32		off)
{
	int32		boff, lineCnt;
	LVBoolean	errf = LVFALSE;

	off = SkipBlanksAndComments(pi, off, &lineCnt, &errf);
	if (off >= pi->scrLen)
		return	FALSE;

	if (CharType(*(pi->scr + off)) != 1)	// non-letter
		return	FALSE;

	boff = off;
	off = _LookForEndOfToken(pi, boff);

	return	(testKeyword(pi, boff, off) == OP_ELSE);
}
