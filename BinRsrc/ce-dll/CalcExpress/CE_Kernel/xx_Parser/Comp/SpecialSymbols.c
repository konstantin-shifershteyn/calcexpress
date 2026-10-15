/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// тело в конце
MgErr	__Make_IndexRange(_Operand *opd, Bool32 incremental);



// =====================================
// >>> #CE_3.1.0-r6 2026-09-28
// IMPORTANT: CSE Parser also treats the Parsing Directives.
// When CE is called from CSE, the Directives are already processed and do not pass to CE Parser.
// so, the Directives can only appear in this function when a CE VI evaluates an expression (not CSE)
int32 SpecialSymbols(
			_CE_Instance	*ip,
			uChar			*pnt,
			int32			off,
			 int32			*expIdx,
			Bool32			*Operand,
			Bool32			*comment,	// >>> #CE_3.1.0-r6 2026-09-28
			LVBoolean		*err_f,
			MgErr			*errp)
{
	_Operand	*opd;
	char		*eos;
	int32		len;

	*comment = FALSE;	// >>> #CE_3.1.0-r6 2026-09-28

	opd = &(ip->LOpd);

	// ### Check for an Index Range Symbol
	// -----------------------------------
	if (__IsKeyword("$>", pnt+off, 2))	{ *errp = __Make_IndexRange(opd, TRUE);  off += 2;	goto save_operand; }
	if (__IsKeyword("$<", pnt+off, 2))	{ *errp = __Make_IndexRange(opd, FALSE); off += 2;	goto save_operand; }

	// ### Check for a Parsing Directive
	// ---------------------------------
	eos = pnt + off + 1;
	for(;;) {	// scan for the end
		int32 ct = CharType(*eos);
		if ((ct != 1) && (ct != 2))
			break;
		eos++;
	}
	len = (int32)(eos - (pnt + off));
	// compare
	if (__IsKeyword("$NCON", pnt+off, len))	 { ip->procPars.enaNamedConst = BitTRUE;	goto directive;	}
	if (__IsKeyword("$NCOFF", pnt+off, len)) { ip->procPars.enaNamedConst = BitFALSE;	goto directive;	}

	// ### ERROR: INVALID DIRECTIVE NAME
	*Operand = TRUE;
	*err_f	 = LVTRUE;
	// ### create an error message
	if (len>1)	CE_Error( ip, "Invalid special symbol or directive \'", NULL, NULL);
	else		CE_Error( ip, "Incomplete special symbol or directive \'", NULL, NULL);
	CE_SrcToError(ip, pnt+off, len, len);	// add text to error protocol
	CE_Error( ip, "\'", NULL,NULL);

	off += 1;	// !!!
	return off;

save_operand:
	// ### Put the operand to the Expression Array
	*Operand = TRUE;
	*errp = ExAr_PutOperand(ip, opd, expIdx);			// >>> #CE_3.0.4 2026-06-22
	return off;

directive:
	// ### Just skeep the directive and mark it as a comment
	off += len;
	*comment = TRUE;
	//*errp = noErr;
	return off;
}
// >>> #CE_3.1.0-r6 2026-09-28

// =====================================
MgErr	__Make_IndexRange(_Operand *opd, Bool32 incremental)
{
	opd->type = OPTYPE_IDXRANGE;
	opd->num  = _MakeIndexRange(incremental);

	return LStr_Clear(opd->str);
}
