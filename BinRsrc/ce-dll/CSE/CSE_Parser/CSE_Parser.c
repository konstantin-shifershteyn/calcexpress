/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <ctype.h>

#include <CSE Parser.h>

#include <celib\ceMemLib.h>

// ===================================

#include "iPScrArray-2.c"

#include "ErrorHandlers.c"

#include "comp\GetToken.c"
#include "Labels\Labels.c"

#include "Expressions\ParseExpression.c"

#include "Operators\Operators.h"
#include "Operators\OperatorProcs.c"

// =============================================================
MgErr CSE_ParseScript(
			iPScrInstance	*iCSE,
			LStrHandle		Script,
			LStrHandle		ScriptName,
			_VIParseParams	*params,
			LVBoolean		*fDidParsing,
			int32			*ExecStatus )
{
	_ScrParserInst	*pi	= NULL;		// !!!
	_OprInfo		*opp;
	char			*errmsg = NULL;
	int32			bOffset, cOffset;
	int32			tCode;
	LVBoolean		errf = LVFALSE;
	MgErr			err = noErr;


	// ===================================
	// ***	INITIALIZE RETURNED PARAMETERS
	// ===================================
//	*err_f			= LVFALSE;
	*ExecStatus		= estSuccess;
	*fDidParsing	= LVTRUE;


	// ***	Save Script Name string
	// ============================
	if (err = LStr_Copy(iCSE->ScriptName, ScriptName))
		goto out;

	// ***	Initialize Runtime parameters
	// ==================================
	iCSE->CancelTag		= FALSE;
	iCSE->rtlCallLevel	= 0;
	iCSE->rtlCaller		= NULL;
	iCSE->rtlCallee		= NULL;
	iCSE->rtlRunMode	= 0;
	LStr_Clear(iCSE->rtlCallbackVIPath);
	LStr_Clear(iCSE->rtlSearchPaths);
	LStr_Clear(iCSE->rtlScriptPath);


	// ***	Check for parsing requirement
	// ==================================
	if (params->fAutoParsing)
		if ( iCSE->fScriptParsed)
			if (params->fGroupExpStm == iCSE->fGroupExpStm)
				if (LStr_Cmp(iCSE->Script, Script)) {
					*fDidParsing = LVFALSE;
					return noErr;
				}


	// =====================================
	// ***	INITIALIZE CSE INSTANCE CONTEXT
	// =====================================

	// ***	Save Script string
	if (err = LStr_Copy(iCSE->Script, Script))
		goto out;

// *** NEW  NEW  NEW ***
	iCSE->ErrCode		= estSuccess;		// error code
	iCSE->ErrIndex		= -1;			// index of error element of parsed script
	iCSE->ErrSrcMaxLen	= params->ErrSrcMaxLen;	// max length of error source info
	iCSE->fScriptParsed	= FALSE;
	iCSE->fGroupExpStm	= (Bool32)(params->fGroupExpStm);
	LStr_Clear(iCSE->ErrMsg);			// clear error message
	iPScr_FreeElements(iCSE->iPScr, 0);		// initialize Parsed Script array


	// =============================================
	// ***	INITIALIZE INSTANCE OF WORKING STRUCTURE
	// =============================================

	// ***	Create working structure instance
	if ( !(pi = (_ScrParserInst*)ce_DSNewPClr(sizeof(_ScrParserInst)) ) ) {
		err = mFullErr;
		goto out;
	}

	if ( err = OprStack_CreateStack(pi) )	goto out;
	if ( err = LblTbl_CreateTable(pi) )	goto out;

	pi->iCSE	= iCSE;		// pointer to instance context
	pi->iPScr	= iCSE->iPScr;		// array of parsed statements
	pi->PScrIdx	= -1;			// !!!
	pi->ErrMsg	= iCSE->ErrMsg;

	pi->scr		= LStrBuf(*(iCSE->Script));
	pi->scrLen	= LStrLen(*(iCSE->Script));
	pi->curLine	= pi->expLine	= 1;

	pi->identOff	= 0;
	pi->identLen	= 0;
	pi->MayBeLabel	= FALSE;

	pi->NestingLevel= 0;
	pi->expStat		= SUBEXP_NONE;	// состо€ние текущего подвыражени€
	pi->identStat	= SUBEXP_NONE;	// состо€ние текущего идентификатора

	pi->fCanBeGrouped	= FALSE;	// just an initial value
	pi->fEnaNamedConst	= (Bool32)params->fEnaNamedConst;	// >>> #CE_3.1.0-r6 2026-09-28

	// ===============================
	// ***	PARSE SCRIPT
	// ===============================

	pi->expOffset	= bOffset = cOffset = 0;

	while ( bOffset < pi->scrLen ) {

		// *** NEW
		if (pi->expStat <= SUBEXP_NONE) {
			bOffset			= SkipBlanksAndComments(pi, bOffset, &(pi->curLine), &errf);
			pi->expOffset	= bOffset;	// !!! need be tested !!!
			pi->expLine		= pi->curLine;
			if (errf)
				goto out;
		}

		cOffset = bOffset;

		tCode = GetToken(pi, &cOffset, &(pi->curLine), &errf);
		if (errf)
			goto out;

		switch( tCode ) {
			case OP_EndOfScript:	goto Finish;

			case OP_COMMENT :		break;

			case OP_GENERAL :
				//		if (pi->expStat <= SUBEXP_NONE) {
				//			pi->expOffset	= bOffset;
				//			pi->expLine	= pi->curLine;
				//		}
						pi->identStat	= pi->expStat;	//statement before possible label
						pi->expStat	= SUBEXP_ACTIVE;
						break;

			case OP_BRACKET_L :	// err = Opr_Bracket_L(pi);
						pi->expStat = SUBEXP_ACTIVE;
						break;

			case OP_BRACKET_R :	// err = Opr_Bracket_R(pi, err_f);
						pi->expStat = SUBEXP_ACTIVE;
						break;

			case OP_SQUARE_BRACKET_L :
						// err = Opr_sqBracket_L(pi);
						pi->expStat = SUBEXP_ACTIVE;
						break;

			case OP_SQUARE_BRACKET_R :
						// err = Opr_sqBracket_R(pi, err_f);
						pi->expStat = SUBEXP_ACTIVE;
						break;
			// ---------------------------------------------------------------

			case OP_SEMICOLON :
				err = Opr_Semicolon	(pi, &(pi->expOffset), &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_CURLY_BRACKET_L :
				err = Opr_cuBracket_L	(pi, &(pi->expOffset), &bOffset, &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_CURLY_BRACKET_R :
				err = Opr_cuBracket_R	(pi, &(pi->expOffset), &bOffset, &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_IF:
				err = Opr_if		(pi, &(pi->expOffset), &bOffset, &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_ELSE:
				err = Opr_else		(pi, &(pi->expOffset), &bOffset, &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_FOR:
				err = Opr_for		(pi, &(pi->expOffset), &bOffset, &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_WHILE:
				err = Opr_while		(pi, &(pi->expOffset), &bOffset, &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_DOWHILE:
				err = Opr_dowhile	(pi, &(pi->expOffset), &bOffset, &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_BREAK:
				err = Opr_break		(pi, &(pi->expOffset), &bOffset, &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_CONTINUE:
				err = Opr_continue	(pi, &(pi->expOffset), &bOffset, &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_RETURN:
				err = Opr_return	(pi, &(pi->expOffset), &bOffset, &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_GOTO:
				err = Opr_goto		(pi, &(pi->expOffset), &bOffset, &cOffset, &errf);
				pi->expStat = SUBEXP_NONE;
				break;

			case OP_COLON:
				err = Opr_Colon		(pi, &(pi->expOffset), &cOffset, &errf);
			//	pi->expStat = SUBEXP_LABEL;
				break;

			default :
				err = LStrPrintf(pi->ErrMsg,
						"Internal Error: Unsupported tCode (%d) in "__FUNCTION__"()"
						"\nfile: %s"
						"\nline: %d"
						, tCode, __FILE__, __LINE__);
				errf = LVTRUE;
				goto out;

		}
		bOffset = cOffset;
		if (err || errf) goto out;
	}

Finish:
	// ***	Test for uncompleted expression
	// ------------------------------------
	if (pi->expStat != SUBEXP_NONE) {
		errmsg = "statement. Missing ';' at the end of script";
		if (pi->expStat > SUBEXP_NONE)
				bOffset	= pi->expOffset;					// expression statement
		else	bOffset	= iPScrEPnt(*(pi->iPScr), pi->PScrIdx)->stmOffset;	// label

err_unterminated:
		// ***	Create source reference (bOffset must be set)
		if (err = iPScrArray_CreateSourceReference(pi, bOffset, cOffset))
			goto out;
		// ***	Error message
		err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, TRUE,
						"Incomplete %s", errmsg, NULL);
		errf = LVTRUE;
		goto out;
	}

	// ***	ѕроверить стек операторов
	// ------------------------------
	if (opp = OprStack_TopPnt(pi)) {
		switch (opp->code) {
			case OP_CURLY_BRACKET_L:errmsg = "compound statement '{'";	break;
			case OP_IF:				errmsg = "'if' statement";		break;
			case OP_ELSE:			errmsg = "'else' statement";		break;
			case OP_FOR:			errmsg = "'for' statement";		break;
			case OP_WHILE:			errmsg = "'while' statement";	break;
			case OP_DOWHILE:		errmsg = "'dowhile' statement";	break;
			default:				errmsg = "<unknown> statement";	break;
		}
		iCSE->ErrIndex = opp->PScrIdx;
		bOffset		= iPScrEPnt(*(pi->iPScr), opp->PScrIdx)->stmOffset;
		goto err_unterminated;

//		*err_f = LVTRUE;
//		err = cespErr_MakeMessage(pi->iCSE, opp->PScrIdx, estSyntaxError, TRUE, "%s", errmsg, NULL);
//		goto out;
	}


	// -------------------
	// ѕреобразовать метки
	// -------------------
	err = ConvertLabelsToValues(pi, &errf);		// can change iCSE->ErrIndex
	if (err || errf)  goto out;

out:
	// ---------------------------------
	// ќсвободить все ненужные ресурсы
	// ---------------------------------
	if (pi) {
		if (pi->iPScr)		iPScr_ResizeArray(&(pi->iPScr), pi->PScrIdx + 1);
		if (pi->OprStk)		OprStack_DestroyStack(pi);
		if (pi->LblTbl)		LblTbl_DestroyTable(pi);
		DSDisposePtr(pi);
	}

	// ***	Return LabVIEW Manager Errors as CSE Execution Status
	if (err) {
		*ExecStatus = err;
	}
	else {
		if (errf) {
			*ExecStatus = estSyntaxError;
			if (iCSE->ErrIndex < 0)
				iCSE->ErrIndex	= pi->PScrIdx;			// !!! NEW
		}
	}

	// !!! NEW
	iCSE->ErrCode		= *ExecStatus;
	iCSE->fScriptParsed	= !(iCSE->ErrCode);

	return err;
}

