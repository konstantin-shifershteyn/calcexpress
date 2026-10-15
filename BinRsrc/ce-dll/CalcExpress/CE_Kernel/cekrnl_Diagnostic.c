/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr	_diag_Script(	_CE_Instance *ip,	_ExpresElement *expEl,	LStrHandle dStr);
MgErr	_diag_FuncVI(	_CE_Instance *ip,	_ExpresElement *expEl,	LStrHandle dStr);
MgErr	_diag_Vars(		_ExpresElement *expEl,	LStrHandle dStr);
MgErr	_diag_FastFunc(	_ExpresElement *expEl,	LStrHandle dStr);

// =======================================================================================
MgErr CEKernel_Diag(
			_CE_Instance		*ip,
			_CEKernel_Par_Diag	*Pars,
			LVBoolean			*err_f )
{
	_ExpresElement	*expData;
	int32		expIdx;
	int32		expSize;
	MgErr		err = noErr;

	Pars->out_status = 0;

	if (*err_f)
		return noErr;

	if (err = LStr_Clear(Pars->io_List))
		return err;

	expSize	= ip->expArrLen;			// >>> #CE_3.0.4 2026-06-22
	expData	= ip->expArrDat;			// >>> #CE_3.0.4 2026-06-22

	for (expIdx=0; expIdx < expSize; expIdx++) {

		switch(Pars->in_Mode) {
			case CEK_MOD_Diag_FuncVI :
				err = _diag_FuncVI( ip,	&(expData[expIdx]), Pars->io_List);
				break;
			case CEK_MOD_Diag_Vars :
				err = _diag_Vars(	&(expData[expIdx]), Pars->io_List);
				break;
			case CEK_MOD_Diag_FastFunc :
				err = _diag_FastFunc(	&(expData[expIdx]), Pars->io_List);
				break;

			case CEK_MOD_Diag_Script :
				err = LStrPrintf(Pars->io_List, "%H%3d)  ", Pars->io_List, expIdx);
				err = _diag_Script(ip,	&(expData[expIdx]), Pars->io_List);
				break;

			case CEK_MOD_Diag_Functions :
				if (expData[expIdx].ops.category == CAT_FASTFUNC)
					err = _diag_FastFunc(	&(expData[expIdx]), Pars->io_List);
				else	err = _diag_FuncVI( ip,	&(expData[expIdx]), Pars->io_List);
				break;
		}
		if (err)
			break;
	}
	return err;
}

// ============================================
//	FuncVI
// ============================================
MgErr	_diag_FuncVI(	_CE_Instance *ip, _ExpresElement *expEl, LStrHandle dStr )
{
	int32		expSize;
	int32		nameIdx;
	LStrHandle	sStr;
	int32		nArg;

	if (!(expEl->IsOps))			return noErr;
	if (expEl->ops.category != CAT_FUNC)	return noErr;

	nArg	= expEl->ops.n_ops - 1;
	nameIdx	= expEl->ops.misc;

	expSize	= ip->expArrLen;			// >>> #CE_3.0.4 2026-06-22
	if ((nameIdx < 0) || (nameIdx >= expSize)) {
		return LStrPrintf(dStr, "%H<Unknown_FuncVI>\n", dStr);
	}
	// теперь указывает на элемент с именем FuncVI
	expEl	= &(ip->expArrDat[nameIdx]);			// >>> #CE_3.0.4 2026-06-22
	if (expEl->opd.type != OPTYPE_IDENT) {
		return LStrPrintf(dStr, "%H<Indirect_Func>\n", dStr);
	}
	sStr = getIdentString(&(expEl->opd));
	return LStrPrintf(dStr, "%H%H\t%d\n", dStr, sStr, nArg);
}
// ============================================
//	Variables
// ============================================
MgErr	_diag_Vars(	_ExpresElement *expEl, LStrHandle dStr )
{
	LStrHandle	sStr;

	if (expEl->IsOps)			return noErr;
	if (expEl->opd.type != OPTYPE_IDENT)	return noErr;
	if (expEl->ops.category == CAT_FUNC)	return noErr;

	sStr = getIdentString(&(expEl->opd));
	return LStrPrintf(dStr, "%H%H\n", dStr, sStr);
}
// ============================================
//	FastFunc
// ============================================
MgErr	_diag_FastFunc(	_ExpresElement *expEl, LStrHandle dStr )
{
	char	*cStr;
	int32	nArg;

	if (!(expEl->IsOps))				return noErr;
	if (expEl->ops.category != CAT_FASTFUNC)	return noErr;

	nArg = expEl->ops.n_ops;
	if (expEl->ops.code < FuncListLen)
		cStr = FuncList[expEl->ops.code].name;
	else	cStr = "unknown_FastFunc";

	return LStrPrintf(dStr, "%H%.8s\t%d\n", dStr, cStr, nArg);
}

// ============================================
//	Script
// ============================================
MgErr	_diag_Script(	_CE_Instance *ip, _ExpresElement *expEl, LStrHandle dStr )
{
	int32		expSize;
	int32		nameIdx;
	char		*cStr;
	LStrHandle	sStr;
	int32		nArg;

	if (expEl->IsOps) {
		if (expEl->ops.category == CAT_INDEX) {
			nArg = expEl->ops.n_ops;
			return LStrPrintf(dStr, "%H  [ ] %d operands: array and %d index(es)\n", dStr, nArg, nArg-1);
		}
		if (expEl->ops.category == CAT_FASTFUNC) {
			nArg = expEl->ops.n_ops;
			if (expEl->ops.code < FuncListLen)
				cStr = FuncList[expEl->ops.code].name;
			else	cStr = "unknown_FastFunc";
			return LStrPrintf(dStr, "%H  %.8s (%d parameters) - FastFunc call\n", dStr, cStr, nArg);
		}
		if (expEl->ops.category == CAT_FUNC) {
			nArg	= expEl->ops.n_ops -1;
			nameIdx	= expEl->ops.misc;

			expSize	= ip->expArrLen;			// >>> #CE_3.0.4 2026-06-22
			if ((nameIdx < 0) || (nameIdx >= expSize)) {
				return LStrPrintf(dStr, "%H  <Unknown_FuncVI>(%d arguments) - FuncVI call\n", dStr, nArg);
			}
			// теперь указывает на элемент с именем FuncVI
			expEl	= &(ip->expArrDat[nameIdx]);		// >>> #CE_3.0.4 2026-06-22
			if (expEl->opd.type != OPTYPE_IDENT) {
				return LStrPrintf(dStr, "%H  <Indirect_Func>(%d arguments) - Func call\n", dStr, nArg);
			}
			sStr = getIdentString(&(expEl->opd));
			return LStrPrintf(dStr, "%H  %H (%d parameters) - FuncVI call\n", dStr, sStr, nArg);
		}

		switch(expEl->ops.category) {

			case CAT_INDEX :	cStr = "[ ]";	break;	// Subscript Operator (операция индексирования)
				nArg = expEl->ops.n_ops;

			case CAT_OBRACKET :	cStr = "(";		break;
			case CAT_CBRACKET :	cStr = ")";		break;
			case CAT_COMMA :	cStr = ",";		break;
//			case CAT_INDEX :	cStr = "[ ]";	break;	// Subscript Operator (операция индексирования)
			case CAT_OINDEX :	cStr = "[...";	break;	// [ - начало индекса (используется только при трансляции)
			case CAT_CINDEX :	cStr = "...]";	break;	// ] - конец индекса (используется только при трансляции)
			case CAT_SEMICOLON:	cStr = ";";		break;	// ; - завершает подвыражение (используется только при трансляции)
			case CAT_ATDELIM :	cStr = "@";		break;	// @ - разделяет индексы x[0..2@0..2] (используется только при трансляции)
// >>> #CE_3.0	2025-04-23
			case CAT_INCPREF :
				switch(expEl->ops.code) {
					case OP1_IncPref :	cStr = "prefix ++";	break;
					case OP1_DecPref :	cStr = "prefix --";	break;
					default:			cStr = "unknown prefix";	break;
				}
				break;
			case CAT_INCPOST :
				switch(expEl->ops.code) {
					case OP1_IncPost :	cStr = "postfix ++";	break;
					case OP1_DecPost :	cStr = "postfix --";	break;
					default:			cStr = "unknown postfix";	break;
				}
				break;
// <<< #CE_3.0	2025-04-23

			case CAT_COMMON :
				switch(expEl->ops.n_ops) {
					default :
						cStr = "<Unknown Operator> (N_Ops is out of range 1..2)";
						break;

					case 1 :
						switch (expEl->ops.code) {
							case OP1_LogNOT :	cStr = "! unary";	break;
							case OP1_BitNOT :	cStr = "~ unary";	break;
							case OP1_Plus :		cStr = "+ unary";	break;
							case OP1_Minus :	cStr = "- unary";	break;
							default :		cStr = "<Unknoun Unary Operator>";	break;
						}
						break;

					case 2 :
						switch (expEl->ops.code) {
							case OP2_AssMul :	cStr = "*=";	break;	// *=
							case OP2_Mul :		cStr = "*";		break;	// *
							case OP2_AssDiv :	cStr = "/=";	break;	// /=
							case OP2_Div :		cStr = "/";		break;	// /
							case OP2_AssMod :	cStr = "%=";	break;	// %=
							case OP2_Mod :		cStr = "%";		break;	// %
							case OP2_AssPlus :	cStr = "+=";	break;	// +=
							case OP2_Plus :		cStr = "+";		break;	// +
							case OP2_AssMinus :	cStr = "-=";	break;	// -=
							case OP2_Minus :	cStr = "-";		break;	// -
							case OP2_AssLShift:	cStr = "<<=";	break;	// <<=
							case OP2_LShift :	cStr = "<<";	break;	// <<
							case OP2_LessOrEq :	cStr = "<=";	break;	// <=
							case OP2_Less :		cStr = "<";		break;	// <
							case OP2_AssRShift:	cStr = ">>=";	break;	// >>=
							case OP2_RShift :	cStr = ">>";	break;	// >>
							case OP2_GreatOrEq:	cStr = ">=";	break;	// >=
							case OP2_Great :	cStr = ">";		break;	// >
							case OP2_NotEqual :	cStr = "!=";	break;	// !=
							case OP2_Equal :	cStr = "==";	break;	// ==
							case OP2_Assign :	cStr = "=";		break;	// =
							case OP2_LogAND :	cStr = "&&";	break;	// &&
							case OP2_AssBitAND:	cStr = "&=";	break;	// &=
							case OP2_BitAND :	cStr = "&";		break;	// &
							case OP2_LogOR :	cStr = "||";	break;	// ||
							case OP2_AssBitOR :	cStr = "|=";	break;	// |=
							case OP2_BitOR :	cStr = "|";		break;	// |
							case OP2_AssBitXOR:	cStr = "^=";	break;	// ^=
							case OP2_BitXOR :	cStr = "^";		break;	// ^
							case OP2_Comma :	cStr = ",";		break;	// ,
							//--- new for CE 2.0 ---
							case OP2_Power :	cStr = "**";	break;	// **	Exponentiation
							case OP2_AssPower :	cStr = "**=";	break;	// **=	Exponentiation Assignment
							case OP2_LogXOR :	cStr = "^^";	break;	// ^^	Logical XOR
							case OP2_IdxRange :	cStr = ":";		break;	// :	Index Range
							default :	cStr = "<Unknoun Binary Operator>";	break;
						}
						break;
				}
				break;
			default :
				cStr = "<Unknown Operator Category>";
				break;
		}
		return LStrPrintf(dStr, "%H  %s\n", dStr, cStr);
	}
	else {
		// OPERAND
		if (expEl->opd.type == OPTYPE_IDENT) {
			sStr = getIdentString(&(expEl->opd));
			return LStrPrintf(dStr, "%H%H\n", dStr, sStr);
		}
		if (expEl->opd.type == OPTYPE_STR) {
			sStr = expEl->opd.str;
			return LStrPrintf(dStr, "%H\"%H\"\n", dStr, sStr);
		}
		if (expEl->opd.type == OPTYPE_DBL) {
			return LStrPrintf(dStr, "%H%.3f\n", dStr, expEl->opd.num);
		}
		return LStrPrintf(dStr, "%HOperand (type=%d)\n", dStr, expEl->opd.type);
	}
	return noErr;
}
