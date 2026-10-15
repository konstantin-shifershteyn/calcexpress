/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

CSE_PRIVATEFUNC
MgErr	Opr_Colon(	_ScrParserInst	*pi,
			int32		*exOff,
		//	int32		*bOff,
			int32		*cOff,
			LVBoolean	*err_f)
{
	_OprInfo	*opp;
	_LabelInfo	*lp;

	int32		label;
	char		*name;
	char		*errmsg = NULL;
	MgErr		err = noErr;

	// ------------------------------------
	// ѕроверить может ли ':' быть меткой или считать ее IndexRange Operator
	// ------------------------------------
	// *****  1. следует за идентификатором? 
	if (!(pi->MayBeLabel))
		goto NotALabel;

	// *****  2. Ќаходитс€ внутри скобок ? {} и []
	if (opp = OprStack_TopPnt(pi)) {
		switch (opp->code) {
			case OP_BRACKET_L :		
			case OP_SQUARE_BRACKET_L :	goto NotALabel;
			default:			break;
		}
	}
	// *****  3. Ќаходитс€ в начале statement ? (нет ничего перед ней)
//	if (pi->identStat != SUBEXP_NONE) {	// только одна метка ???
	if (pi->identStat > SUBEXP_NONE) {

NotALabel:	// ':' определ€ет не метку
		// -----------------------
		pi->expStat = pi->identStat = SUBEXP_ACTIVE;
		return err;
	}

	// ============================
	//	YES, IT IS A LABEL
	// ============================

	// ***	Create label source reference
	if (err = iPScrArray_CreateSourceReference(pi, *exOff, *cOff))
		goto out;


	// ---------------------------
	// —оздать метку
	// ---------------------------
	name = pi->scr + pi->identOff;
	err = NLabel_SetOrCreateLabel(pi, &label, name, pi->identLen, -1, err_f);
	if (err || *err_f) goto out;
	lp =  LblTbl_GetPnt(pi, label);

	pi->expStat = SUBEXP_LABEL;

out:
	*exOff = *cOff;
	return	err;
}
