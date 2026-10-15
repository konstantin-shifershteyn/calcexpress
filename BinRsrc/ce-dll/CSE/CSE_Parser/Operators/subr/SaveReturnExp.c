/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__SaveReturnExp_C__
#define	__SaveReturnExp_C__


CSE_PRIVATEFUNC
MgErr SaveReturnExp( _ScrParserInst *pi, int32 *offset, LVBoolean *err_f)
{
	int32		begOff	= *offset;
	int32		curLine	= pi->curLine;
//	int32		brLevel	= 0;
//	int32		sqLevel	= 0;
	char		*errmsg	= NULL;
	MgErr		err	= noErr;


	// поиск ';' после 'return'
	// ------------------------
	while(*offset < pi->scrLen) {

		begOff	= *offset;

		switch( GetToken(pi, offset, &(pi->curLine), err_f) ) {
		//	case OP_BRACKET_L :		brLevel++;	break;
		//	case OP_BRACKET_R :		brLevel--;	break;
		//	case OP_SQUARE_BRACKET_L:	sqLevel++;	break;
		//	case OP_SQUARE_BRACKET_R:	sqLevel--;	break;


			case OP_BRACKET_L :
			case OP_BRACKET_R :
			case OP_SQUARE_BRACKET_L:
			case OP_SQUARE_BRACKET_R:

			case OP_COLON:		// пропускаем, рассматриваем как CE IndexRange
			case OP_COMMENT:	// пропускаем
			case OP_GENERAL:	// пропускаем
						break;

			case OP_SEMICOLON:	goto save;

			default :	if (*err_f)	goto out;
					goto err0;
		}
	}
err0:

	// ***	update source references
	iPScrArray_FinishExpressionEx(pi, begOff, begOff );
//	iPScrArray_UpdateSourceReferenceLength(pi, begOff);

	err = CSE_MakeErrorMessage(pi->iCSE, pi->PScrIdx, estSyntaxError, TRUE,
				"Missing ';' after 'return'", (LStrHandle)errmsg, NULL);

	*err_f = LVTRUE;
	return err;

save:
	// контроль нарности скобок
	// ------------------------
//	if (brLevel > 0) { errmsg = "Missing ')' in expression";	goto err; }
//	if (brLevel < 0) { errmsg = "Unexpected ')' in expression";	goto err; }
//	if (sqLevel > 0) { errmsg = "Missing ']' in expression";	goto err; }
//	if (sqLevel < 0) { errmsg = "Unexpected ']' in expression";	goto err; }

out:
	// ***	Complete 'return' statement
	iPScrArray_FinishExpressionEx(pi, *offset, begOff );

	return	err;
}


#endif
