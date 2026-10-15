/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <stdlib.h>
#include <ctype.h>

#include "Comments.c"
#include "NumConstant.c"
#include "EscapeSeq.c"
#include "StrConstant.c"
#include "CharConstant.c"
#include "Operations.c"
#include "SpecialSymbols.c"


// ###########################################################################

// -------------------
Bool32	CheckForNamedConstant(_Operand *opd, uChar *pnt, int32 len)
{
	const _td_named_dbl_const * ncp;

	ncp = __namedDblConst_find(pnt, len);
	if (ncp == NULL)
		return	FALSE;

	if (ncp->valPnt != NULL)
			opd->num = *(ncp->valPnt);
	else	opd->num = ncp->val;
	opd->type = OPTYPE_DBL;
	LStr_Clear(opd->str);
	return TRUE;
}
// <<< #CE_3.1.0-r6 2026-09-24

// ############################################################################

MgErr OneToken(	_CE_Instance	*ip,
				int32			*expIdx,
				int32			*offset,
				_Operation		*OpInfo,
				int32			*OpdFlag,
				LVBoolean		*err_f )
{
	_Operand	*opd;
	int32		datlen;			// length of the expression string (w/o the zero terminator)
	Bool32		comment;
	uChar		*pnt;			// pointer to the expression string
	int32		off;			// offset in the expression string
	int32		boff;			// offset of the current token
	int32		toff;			// temporary offset
	int32		PrevOperand;
	LVBoolean	errf;
	int32		identLen;		// Identifiers: length of the found identifier 
	Bool32		checkNC;		// Identifiers: TRUE if the found identifier can be checked for a Named Constant
	_Operation	*ops;			// used for Constant Folding
	int32		idx;			// used for Constant Folding
	MgErr		err;

	opd	= &(ip->LOpd);					// pointer to a temporary operand

	pnt		= LStrBuf(*(ip->str));		// pointer to the expression string
	datlen	= LStrLen(*(ip->str)) - 1;	// length of the expression string (w/o the zero terminator)

	PrevOperand = *OpdFlag;			// non-zero if the previous token was an operand: 1=Constant, -1=Identifier

	off = *offset;
	if ((off < 0) || (off >= datlen)) {
not_found:
		*OpdFlag = 1;	// a positive value signals a constant
		off = -1;
		err = noErr;
		goto out;
	}

beg: // <<< we also come to this point after parsing of comments
	err = noErr;	// >>> #CE_3.1.0-r6 2026-09-28

	// ### skip white spaces
	// ---------------------
	while (*(pnt+off) <= ' ') {
		if (++off >= datlen)
			goto not_found;
	}
	// ### Determine the type of a token
	// ---------------------------------
	boff = off;
	switch( CharType(*(pnt+off)) ) {

		case 1:	// *** Identifier
			while(TRUE) {	// find the end
				switch( CharType(*(pnt + ++off)) ) {
					case 1 :
					case 2 : continue;
					default: break;
				}
				break;
			}
			identLen = off - boff;		// length of the identifier

// >>> #CE_3.1.0-r6 2026-09-25
			if (ip->procPars.enaNamedConst) {
				// find the next token just to learn its type
				checkNC = TRUE;
				for (toff = off; toff < datlen; toff++) {
					if (*(pnt + toff) <= ' ')
						continue;
					if (*(pnt + toff) == '(')
						checkNC = FALSE;		// it looks like a function call (identifier is followed by an opening bracket)
					break;
				}
				// ### check for the named constants
				if (checkNC) {
					if (CheckForNamedConstant(opd, pnt+boff, identLen)) {
						*OpdFlag = 1;	// a positive value signals a constant
						err = ExAr_PutOperand(ip, opd, expIdx);
						break;
					}
				}
			}
// <<< #CE_3.1.0-r6 2026-09-25
			*OpdFlag = -1;	//	a negative value signals an identifier
			opd->type = OPTYPE_IDENT;
			if (err = LStr_copy_n( pnt, opd->str, boff, identLen))
				goto out;
			opd->num = 0;
			opd->hdl = NULL;	// 2025-03-12
			// *** ВНИМАНИЕ *** Указанное ниже выполняется в ExAr_PutOperand
			// -------------------------------------------------------------
			// opd->num = 0;
			// opd->hdl = opd->str;	// комментарии см. ниже
			// Для идентификаторов(!) в _Operand.hdl заносим handle строки имени
			// Это используется при выполнении, для исключения копирования строк
			// при копировании структуры _Operand (в частности - стек операндов)

			err = ExAr_PutOperand(ip, opd, expIdx);			// >>> #CE_3.0.4 2026-06-22
			break;

		case 2:	// Digit or decimal point
		case 3:	// ----------------------
			*OpdFlag = 1;	// a positive value signals a constant
			off = NumConst( ip, pnt, off, expIdx, err_f, &err);
			break;

		case 4:	// string constant
			// -------------------
			*OpdFlag = 1;	// a positive value signals a constant
			off = StrConst( ip, pnt, off+1, expIdx, err_f, &err);
			break;

		case 5:	// character constant
			// ----------------------
			*OpdFlag = 1;	// a positive value signals a constant
			off = CharConst( ip, pnt, off+1, expIdx, err_f, &err);
			break;

		default:
			if (*(pnt+off) == '$') {
//				off = SpecialSymbols(ip, pnt, off, expIdx, OpdFlag, err_f, &err);
// >>> #CE_3.1.0-r6 2026-09-28
				off = SpecialSymbols(ip, pnt, off, expIdx, OpdFlag, &comment, err_f, &err);
				if (*err_f || err)
					goto out;
				if (comment)
					goto beg;
// <<< #CE_3.1.0-r6 2026-09-28
				if (*OpdFlag)	break;
				else			goto out;
			}
			// an Operator is expeted here
			off = Operation(ip, pnt, off, OpInfo, OpdFlag, &comment, err_f, &err);
			if (*err_f || err)
				goto out;
			if (comment)
				goto beg;
			goto out;
	}
	// ****************************************
	// The following code runs for operand only
	// ****************************************
	if (err) goto out;

	// ### Check error: "missed operator"
	if (PrevOperand && !(*err_f)) {
		err = CE_Error( ip, "Missing operator before \'", NULL,NULL);
		err = CE_SrcToError(ip, pnt+boff, off-boff, off-boff);	// add token to error protocol
		err = CE_Error( ip, "\'", NULL,NULL);
		*err_f = LVTRUE;
		goto out;
	}

	// ### if there is an active function call, set sign "has arguments"
	errf = LVFALSE;
	FIOStack_SetArgFlag(ip, &errf);

// >>> #CE_3.1.0-r6 2026-09-24
	// ### CONSTANT FOLDING ###
	// ### if a numeric constant and there is an active unary plus/minus/not operator,
	// ### remove the operator and modify the constant's value
	idx = *expIdx - 1;		// index of the last processed operand (*expIndx is incremented when an operand is put to the expression array)
	if (idx < 0)
		goto out;

	if ((ip->expArrDat[idx].IsOps == FALSE) && (ip->expArrDat[idx].opd.type == OPTYPE_DBL)) {
		// if the token is a numeric constant
		ops = OpsStack_TopPnt(ip);
		if (ops) {
			// if there is an operator in the Stack of Operators
			if ((ops->category == CAT_COMMON) && (ops->n_ops == 1))  {
				// if an unary operator
				switch (ops->code) {
				case OP1_Plus :				// unary Plus: don't modify the operand's value
					OpsStack_DecLen(ip);	// remove the operator from the stack
					break;
				case OP1_Minus :			// unary Minus: negate the operand's value
					ip->expArrDat[idx].opd.num = -ip->expArrDat[idx].opd.num;
					OpsStack_DecLen(ip);	// remove the operator from the stack
					break;
				case OP1_BitNOT :			// unary Bitwise-NOT
					ip->expArrDat[idx].opd.num = ~_DblToI32Bits(ip->expArrDat[idx].opd.num);
					OpsStack_DecLen(ip);	// remove the operator from the stack
					break;
				case OP1_LogNOT :			// unary Logical-NOT
					ip->expArrDat[idx].opd.num = _DblToI32Bits(ip->expArrDat[idx].opd.num) ? 1.0 : 0.0;
					OpsStack_DecLen(ip);	// remove the operator from the stack
					break;
				}
			}
		}
	}
// <<< #CE_3.1.0-r6 2026-09-24

out:
	*offset = off;
	return err;
}
