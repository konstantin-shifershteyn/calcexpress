/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include "OpMacros.h"

//										code		   pry n_ops
static _Operation	op_obracket		= { 0,	/* ( */		 0,  0, CAT_OBRACKET,	DIR_LR }; // (
static _Operation	op_cbracket		= { 1,	/* ) */		 1,  0, CAT_CBRACKET,	DIR_LR }; // )
//--
static _Operation	op_oindex		= { 0,	/* [ */		 0,  0, CAT_OINDEX,		DIR_LR }; // [
static _Operation	op_cindex		= { 1,	/* ] */		 1,  0, CAT_CINDEX,		DIR_LR }; // ]
//--
static _Operation	op1_LogNOT		= { OP1_LogNOT,		16,  1, CAT_COMMON,		DIR_RL }; // !
static _Operation	op1_BitNOT		= { OP1_BitNOT,		16,  1, CAT_COMMON,		DIR_RL }; // ~
static _Operation	op1_Plus		= { OP1_Plus,		16,  1, CAT_COMMON,		DIR_RL }; // + unary
static _Operation	op1_Minus		= { OP1_Minus,		16,  1, CAT_COMMON,		DIR_RL }; // - unary

static _Operation	op2_Power		= { OP2_Power,		15,  2, CAT_COMMON,		DIR_LR }; // **

static _Operation	op2_Mul			= { OP2_Mul,		14,  2, CAT_COMMON,		DIR_LR }; // *
static _Operation	op2_Div			= { OP2_Div,		14,  2, CAT_COMMON,		DIR_LR }; // /
static _Operation	op2_Mod			= { OP2_Mod,		14,  2, CAT_COMMON,		DIR_LR }; // %
static _Operation	op2_Plus		= { OP2_Plus,		13,  2, CAT_COMMON,		DIR_LR }; // +
static _Operation	op2_Minus		= { OP2_Minus,		13,  2, CAT_COMMON,		DIR_LR }; // -
static _Operation	op2_LShift		= { OP2_LShift,		12,  2, CAT_COMMON,		DIR_LR }; // <<
static _Operation	op2_RShift		= { OP2_RShift,		12,  2, CAT_COMMON,		DIR_LR }; // >>

static _Operation	op2_LessOrEq	= { OP2_LessOrEq,	11,  2, CAT_COMMON,		DIR_LR }; // <=
static _Operation	op2_Less		= { OP2_Less,		11,  2, CAT_COMMON,		DIR_LR }; // <
static _Operation	op2_GreatOrEq	= { OP2_GreatOrEq,	11,  2, CAT_COMMON,		DIR_LR }; // >=
static _Operation	op2_Great		= { OP2_Great,		11,  2, CAT_COMMON,		DIR_LR }; // >
static _Operation	op2_NotEqual	= { OP2_NotEqual,	10,  2, CAT_COMMON,		DIR_LR }; // !=
static _Operation	op2_Equal		= { OP2_Equal,		10,  2, CAT_COMMON,		DIR_LR }; // ==

static _Operation	op2_BitAND		= { OP2_BitAND,		 9,  2, CAT_COMMON,		DIR_LR }; // &
static _Operation	op2_BitXOR		= { OP2_BitXOR,		 8,  2, CAT_COMMON,		DIR_LR }; // ^
static _Operation	op2_BitOR		= { OP2_BitOR,		 7,  2, CAT_COMMON,		DIR_LR }; // |

static _Operation	op2_LogAND		= { OP2_LogAND,		 6,  2, CAT_COMMON,		DIR_LR }; // &&
static _Operation	op2_LogXOR		= { OP2_LogXOR,		 5,  2, CAT_COMMON,		DIR_LR }; // ^^
static _Operation	op2_LogOR		= { OP2_LogOR,		 4,  2, CAT_COMMON,		DIR_LR }; // ||

static _Operation	op2_IdxRange	= { OP2_IdxRange,	 3,  2, CAT_COMMON,		DIR_LR }; // :

static _Operation	op2_Assign		= { OP2_Assign,		 2,  2, CAT_COMMON,		DIR_RL }; // =
static _Operation	op2_AssPower	= { OP2_AssPower,	 2,  2, CAT_COMMON,		DIR_RL }; // **=
static _Operation	op2_AssMul		= { OP2_AssMul,		 2,  2, CAT_COMMON,		DIR_RL }; // *=
static _Operation	op2_AssDiv		= { OP2_AssDiv,		 2,  2, CAT_COMMON,		DIR_RL }; // /=
static _Operation	op2_AssMod		= { OP2_AssMod,		 2,  2, CAT_COMMON,		DIR_RL }; // %=
static _Operation	op2_AssPlus		= { OP2_AssPlus,	 2,  2, CAT_COMMON,		DIR_RL }; // +=
static _Operation	op2_AssMinus	= { OP2_AssMinus,	 2,  2, CAT_COMMON,		DIR_RL }; // -=
static _Operation	op2_AssLShift	= { OP2_AssLShift,	 2,  2, CAT_COMMON,		DIR_RL }; // <<=
static _Operation	op2_AssRShift	= { OP2_AssRShift,	 2,  2, CAT_COMMON,		DIR_RL }; // >>=
static _Operation	op2_AssBitAND	= { OP2_AssBitAND,	 2,  2, CAT_COMMON,		DIR_RL }; // &=
static _Operation	op2_AssBitXOR	= { OP2_AssBitXOR,	 2,  2, CAT_COMMON,		DIR_RL }; // ^=
static _Operation	op2_AssBitOR	= { OP2_AssBitOR,	 2,  2, CAT_COMMON,		DIR_RL }; // |=

static _Operation	op2_Comma		= { OP2_Comma,		 1,  2, CAT_COMMA,		DIR_LR }; // ,
//--
static _Operation	op2_AtDelimiter	= { 0,	/* @ */		 1,  0, CAT_ATDELIM,	DIR_LR }; // @
static _Operation	op_semicolon	= { 0,	/* ; */		 0,  0, CAT_SEMICOLON,	DIR_LR }; // ;

// >>> #CE_3.0 2025-03-12
static _Operation	op1_IncPref		= { OP1_IncPref,	16,  1, CAT_INCPREF,	DIR_RL }; // ++ prefix
static _Operation	op1_IncPost		= { OP1_IncPost,	17,  1, CAT_INCPOST,	DIR_LR }; // ++ postfix
static _Operation	op1_DecPref		= { OP1_DecPref,	16,  1, CAT_INCPREF,	DIR_RL }; // -- prefix
static _Operation	op1_DecPost		= { OP1_DecPost,	17,  1, CAT_INCPOST,	DIR_LR }; // -- postfix
// <<< #CE_3.0


// #####################################################################

int32 Operation(	_CE_Instance	*ip,
					uChar			*pnt,
					int32			off,
					_Operation		*opinfo,
					Bool32			*Operand,
					Bool32			*comment,
					LVBoolean		*err_f,
					MgErr			*errp )
{
	_Operation	*pop;
	int32		roff;
	uChar		tbuf[8];
//	const	uChar		*msg[3] = { "", "unary", "binary" };

	*comment = FALSE;
	roff  = off;
	*errp = noErr;

	switch ( *(pnt + off++) ) {
		case '(' :	pop = &op_obracket;		goto final;
		case ')' :	pop = &op_cbracket;		goto final;
		case '[' :	pop = &op_oindex;		goto final;
		case ']' :	pop = &op_cindex;		goto final;
		case ',' :	pop = &op2_Comma;		goto final;
		case ';' :	pop = &op_semicolon;	goto final;

		case '@' :	pop = &op2_AtDelimiter;	goto final;
		case ':' :	pop = &op2_IdxRange;	break;

		case '~' :	pop = &op1_BitNOT;		break;

		case '*' :	if (*(pnt+off)=='/')	goto comm;		// знак конца комментария
					test3('*', &op2_Mul, &op2_Power, &op2_AssMul, &op2_AssPower);
					break;

		case '/' :	switch (*(pnt+off)) {
						case '/':
						case '*': goto comm;
					}
					test2(&op2_Div,	&op2_AssDiv);
					break;

		case '=' :	test2(&op2_Assign,	&op2_Equal);		break;
		case '%' :	test2(&op2_Mod,		&op2_AssMod);		break;
		case '!' :	test2(&op1_LogNOT,	&op2_NotEqual);		break;
// >>> #CE_3.0 2025-03-12
		case '+' :	if (*(pnt+off) == '+')	{	// if "++"
						off++;
						pop = *Operand ? &op1_IncPost : &op1_IncPref;
						break;
					}
					test2a(&op1_Plus , &op2_Plus , &op2_AssPlus);	break;
		case '-' :	if (*(pnt+off) == '-') {	// if "--"
						off++;
						pop = *Operand ? &op1_DecPost : &op1_DecPref;
						break;
					}
					test2a(&op1_Minus, &op2_Minus, &op2_AssMinus);	break;
// <<< #CE_3.0 2025-03-12
		case '&' :	test2_2('&', &op2_BitAND, &op2_LogAND, &op2_AssBitAND); break;
		case '^' :	test2_2('^', &op2_BitXOR, &op2_LogXOR, &op2_AssBitXOR); break;
		case '|' :	test2_2('|', &op2_BitOR , &op2_LogOR , &op2_AssBitOR);  break;

		case '<' :	test3('<', &op2_Less , &op2_LShift, &op2_LessOrEq , &op2_AssLShift); break;
		case '>' :	test3('>', &op2_Great, &op2_RShift, &op2_GreatOrEq, &op2_AssRShift); break;

		default :
			// Недопустимый символ
			// -------------------
			*Operand = TRUE;
			*err_f	 = LVTRUE;
			*errp = CE_Error( ip, "Invalid character \'%c\' (0x%02x)", (LStrHandle)*(pnt+off-1), (LStrHandle)*(pnt+off-1));
			goto out;
	}
	// ---------------------------
	switch(pop->n_ops) {
	case 2:
		if (!(*Operand)) {
			StrNCpy(tbuf, pnt+roff, off-roff);
			*(tbuf+off-roff)='\0';
			*errp = CE_Error( ip, "Missing operand before \'%s\'", (LStrHandle)tbuf, NULL);
			*err_f = LVTRUE;
		}
		break;
	case 1:
// >>> #CE_3.0	2025-03-17
		if (pop->category == CAT_INCPOST)
			goto final;
// <<< #CE_3.0	2025-03-17
		if (*Operand) {
			StrNCpy(tbuf, pnt+roff, off-roff);
			*(tbuf+off-roff)='\0';
			*errp = CE_Error( ip, "Unexpected \'%s\' operator", (LStrHandle)tbuf, NULL);
			*err_f = LVTRUE;
		}
		break;
	}
final:
	*Operand = FALSE;
	*opinfo = *pop;
out:
	return off;

comm:
	// обработка комментария
	// ---------------------
	off = Comments( ip, pnt, off-1, comment, err_f, errp);
	return off;
}
