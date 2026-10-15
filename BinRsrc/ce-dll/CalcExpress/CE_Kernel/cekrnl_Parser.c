/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ==========  Подпрограммы  ============
#include "xx_Parser\FIOStack\FIOStack.c"
#include "xx_Parser\OpsStack\OpsStack.c"

#include "xx_Parser\ExpArray.c"
#include "xx_Parser\VarRefArray.c"
#include "xx_Parser\CE_Parser_Subr.c"

#include "xx_Parser\Operations\ProcOperation.c"
#include "xx_Parser\Comp\InputString.c"

// =======================================================================================

MgErr CEKernel_Parser(
				_CE_Instance			*ip,
				_CEKernel_Par_Parse		*ParParse,
				LVBoolean				*err_f )
{
	Bool32		ParseDirsChanged = FALSE;
	int32		offset;
	int32		expIdx;
	Bool32		OpdFlag;
	Bool32		subExp_begin, prev_operand, prev_obracket, maybe_func;
	_Operation	ops, *otp;
	LVBoolean	errf = LVFALSE;
	MgErr		err = noErr;

	if (*err_f) {		// input error ?
		return noErr;
	}

	// >>> #CE_3.1.0-r6 2026-09-25
	ip->procPars.enaNamedConst = (ParParse->in_enaNamedConst ? BitTRUE : BitFALSE);
	// >>> #CE_3.1.0-r6 2026-09-29
	ParseDirsChanged = ((ip->old_enaNamedConst) && !( ParParse->in_enaNamedConst) || !(ip->old_enaNamedConst) && ( ParParse->in_enaNamedConst));
//	ParseDirsChanged = (ip->old_enaNamedConst != ParParse->in_enaNamedConst);
	ip->old_enaNamedConst = ParParse->in_enaNamedConst;	// we save it just as reference for next call in AutoParse mode
	// <<< #CE_3.1.0-r6 2026-09-29

	ParParse->out_ParseFlag = FALSE;
	switch (ParParse->tmp_Mode) {
		default :
			goto out;

		case CEK_CMD_Parse_Auto :
//			if (CmpStrings_X( ip->str, ParParse->in_Script)) {
			if (!ParseDirsChanged && CmpStrings_X( ip->str, ParParse->in_Script)) {
				if (ip->procPars.ErrorOfParsing)
					*err_f = LVTRUE;
				goto out;		// не делаем повторную трансляцию !!!
			}
			ip->procPars.testSyntaxOnly = BitFALSE;
			break;
		case CEK_CMD_Parse :
			ip->procPars.testSyntaxOnly = BitFALSE;
			break;
		case CEK_CMD_Parse_Test :
			ip->procPars.testSyntaxOnly = BitTRUE;
			break;
	}
//DbgPrintf(__FUNCTION__"(): compile\n");
	ParParse->out_ParseFlag		= TRUE;

	ip->procPars.ParsingInProgress	= BitTRUE;

	// --------------------------
	// Подготовиться к трансляции
	// --------------------------
	if (err = TransInit(ip, ParParse->in_Script))
		goto out;

	expIdx			= 0;
	offset			= 0;
	OpdFlag			= FALSE;	// текущая лексема - операнд
	prev_operand	= FALSE;	// предыдущая лексема была операндом 
	prev_obracket	= FALSE;	// устанавливается при '('
	maybe_func		= FALSE;	// устанавливается если следующая '(' может быть вызовом функции

	subExp_begin	= TRUE;		// начало (под)выражения, устанавливается при ';'

	// ================================================
	// PARSING LOOP	/ Цикл трансляции
	// ================================================
	while ( !(*err_f) && (offset>=0) ) {

		OpdFlag = prev_operand;
		if ( err = OneToken( ip, &expIdx, &offset, &ops, &OpdFlag, err_f))
			goto out;	//goto trans_end;
		if ( *err_f || (offset < 0))	break;

		// ---------------
		// *** Операнд ***
		// ---------------
		if (OpdFlag) {
			maybe_func		= (OpdFlag == -1);	// TRUE if an identifier
			prev_operand	= TRUE;
			prev_obracket	= FALSE;
			subExp_begin	= FALSE;
			continue;
		}
		// ----------------
		// *** Операция ***
		// ----------------
		switch (ops.category) {
			case CAT_OBRACKET :
				err = PO_OBracket(ip, &expIdx, &ops, &prev_operand, &prev_obracket, maybe_func, err_f);
				subExp_begin	= 
				maybe_func		= FALSE;
				break;
			case CAT_CBRACKET :
				err = PO_CBracket(ip, &expIdx, &ops, &prev_operand, &prev_obracket, &maybe_func, err_f);
				subExp_begin	= FALSE;
				break;
			case CAT_OINDEX :
				err = PO_OIndex(ip, &expIdx, &ops, &prev_operand, err_f);
				prev_obracket	= 
				subExp_begin	= 
				maybe_func		= FALSE;
				break;
			case CAT_CINDEX :
				err = PO_CIndex(ip, &expIdx, &ops, &prev_operand, err_f);
				prev_obracket	= 
				subExp_begin	= 
				maybe_func		= FALSE;
				break;
			case CAT_COMMA :
				err = PO_Comma(	ip, &expIdx, &ops, &prev_operand, err_f);
				prev_obracket	= 
				subExp_begin	= 
				maybe_func		= FALSE;
				break;

			case CAT_ATDELIM:
				err = PO_AtDelimiter( ip, &expIdx, &ops, &prev_operand, err_f);
				prev_obracket	= 
				subExp_begin	= 
				maybe_func		= FALSE;
				break;

			case CAT_SEMICOLON :
				err = PO_Semicolon(	ip, &expIdx, &ops, &subExp_begin, &prev_operand, err_f);
			//	subExp_begin	= TRUE;	// выполняется в функции
			//	prev_operand	= prev_obracket = FALSE;
				prev_obracket	= 
				maybe_func		= FALSE;
				break;

// >>> #CE_3.0 2025-03-22
			case CAT_INCPOST:
				err = ProcOperation( ip, &expIdx, &ops );
				prev_operand	= TRUE;
				prev_obracket	=
				subExp_begin	=
				maybe_func		= FALSE;
				break;
// <<< #CE_3.0 2025-03-22

			default :
				err = ProcOperation( ip, &expIdx, &ops );
				prev_operand	=
				prev_obracket	=
				subExp_begin	=
				maybe_func		= FALSE;
				break;
		}
		if (err) {
			*err_f = LVTRUE;
			goto out;
		}
	}
	// ------------------------------------
	// Извлечь оставшиеся операции из стека
	// ------------------------------------
	errf = LVFALSE;
	while(!(*err_f)) {
		otp = OpsStack_TopPnt(ip);	// указатель на верхний элемент стека
		if (!otp)	break;			// выход если стек пуст
		OpsStack_DecLen(ip);		// уменьшить размер стека

		switch(otp->category) {
			case CAT_OBRACKET:	err = CE_Error(ip, "Missing pair for '('",	NULL,NULL);
						*err_f = LVTRUE;	break;
			case CAT_CBRACKET:	err = CE_Error(ip, "Unexpected ')'",		NULL,NULL);
						*err_f = LVTRUE;	break;
			case CAT_OINDEX:	err = CE_Error(ip, "Missing pair for '['",	NULL,NULL);
						*err_f = LVTRUE;	break;
			case CAT_CINDEX:	err = CE_Error(ip, "Unexpected ']'",		NULL,NULL);
						*err_f = LVTRUE;	break;
			default:
						err = ExAr_PutOperation( ip, otp, &expIdx);			// >>> #CE_3.0.4 2026-06-22
						break;
		}
		if (err) break;
	}
	if (!prev_operand && !subExp_begin && !(*err_f)) {
		err = CE_Error( ip, "Missing operand at the end of expression", NULL,NULL);
		*err_f = LVTRUE;
	}

	// set size of Expression Array
	err = ExAr_Resize(ip, expIdx );			// >>> #CE_3.0.4 2026-06-22

out:
// >>> #CE_3.0 2025-03-22
//	### has been used for debugging
//  ### Displays a message box with parsed expression ###
//  ### the function is defined in "\CE_Kernel\xx_Parser\ExpArray.c" file 
//	ExAr_DebugDisplay( ip->expArr );
// <<< #CE_3.0 2025-03-22

	ip->procPars.ParsingInProgress	= BitFALSE;
	ip->procPars.ErrorOfParsing		= (*err_f != LVFALSE);

	if (err) {
		CEKernel_Context_FreeMem(ip, TRUE);	// free memory allocated
		CE_Error_LVMgErr( ip, err, err_f);	// save LabVIEW error code
	}
	return err;
}
