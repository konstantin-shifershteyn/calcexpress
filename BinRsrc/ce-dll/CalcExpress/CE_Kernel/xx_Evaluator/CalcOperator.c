/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include "CalcOperator.h"


MgErr EvalOperator(		_CE_Instance	*ip,
						_Operation		*op,
						LVBoolean		*err_f )
{
	_Operand		*Lopd, *Ropd;
	_Operand		*Vopd = NULL;		// >>> #CE_3.0 2025-04-20
	_OperandBase	*lValue;			// >>> #CE_3.0.5 2026-07-12 : pointer to actual value, either _Operand in Operand Stack or _VarInfo in VarMan
	_OperandBase	*rValue;			// >>> #CE_3.0.5 2026-07-12 : pointer to actual value, either _Operand in Operand Stack or _VarInfo in VarMan
	_VarRecord		*vp;				// >>> #CE_3.0.7 2026-07-25 : we need it to complete writing to a variable when doing direct access
	char			*opname;
	uInt32			VMLOCKFLAGS = OPDVARDATAREF_NOLOCK;
	Bool32			isAssign = FALSE;
	LVBoolean		errf = LVFALSE;		//*err_f;
	MgErr			err = noErr;

	Lopd	= &(ip->LOpd);
	Ropd	= &(ip->ROpd);


	switch (op->category) {
		case CAT_OBRACKET :	err = CE_Error(ip, "Missing closing parenthesis ')'",NULL,NULL);
					errf = LVTRUE;
					goto out;
		case CAT_CBRACKET :	err = CE_Error(ip, "Unexpected closing parenthesis ')'",NULL,NULL);
					errf = LVTRUE;
					goto out;
		case CAT_OINDEX :	err = CE_Error(ip, "Missing closed bracket ']'",NULL,NULL);
					errf = LVTRUE;
					goto out;
		case CAT_CINDEX :	err = CE_Error(ip, "Unexpected closing bracket ']'",NULL,NULL);
					errf = LVTRUE;
					goto out;
// >>> #CE_3.0.5 2025-07-17 : this code is called in the main Evaluator function
//		case CAT_INDEX:
//					err = IndexingOperator(ip, op, &errf);
//					goto out;
// <<< #CE_3.0.5 2025-07-17
// >>> #CE_3.0.5 2025-07-17 : this code is called in the main Evaluator function
//		case CAT_SEMICOLON:
//					Lopd = OpdStack_TopPnt( ip, &errf);
//					OpdStack_DecLength( ip, &errf);
//					if ((Lopd->type == OPTYPE_IDENT) && (Lopd->vpnt == NULL)) {	// >>> #CE_3.0.5 2025-07-15 : check only if variable was not evaluated
//						OpdStack_VerifyOperand( ip, Lopd, &errf);
//					}
//					// >>> #CE_3.0.5 2025-07-15 : disabled: indexation is only used with OPTYPE_IDENT, but if we keep var name in the Operands Stack, we also must keep its indexation
//					//_IndexInfo_ResetIndexation(Lopd);		// >>> #CE_3.0.4 2026-07-05
//					goto exit;
// <<< #CE_3.0.5 2025-07-17
		case CAT_COMMA :
					Ropd = OpdStack_TopPnt( ip, &errf);
					OpdStack_DecLength( ip, &errf );
					Lopd = OpdStack_TopPnt( ip, &errf);
					if ((Lopd->type == OPTYPE_IDENT) && (Lopd->vpnt == NULL)) {	// >>> #CE_3.0.5 2025-07-15 : check only if variable was not evaluated
						OpdStack_VerifyOperand( ip, Lopd, &errf);
					}
					// >>> #CE_3.0.5 2025-07-15 : disabled: indexation is only used with OPTYPE_IDENT, but if we keep var name in the Operands Stack, we also must keep its indexation
					//_IndexInfo_ResetIndexation(Lopd);		// >>> #CE_3.0.4 2026-07-05
					err = Swap_Operands(Ropd, Lopd);
					goto exit;

// >>> #CE_3.0 2025-04-21 / 2025-03-22
		case CAT_INCPREF :	// ### Prefix Increment/Decrement operators (++/--)
					Lopd = OpdStack_TopPnt(ip, &errf);
					if (errf)
						goto error_MissingOperand;
					if ((Lopd->type != OPTYPE_IDENT) || Lopd->deref)		// >>> #CE_3.0.5 2025-07-14
						goto error_OperandNotVar;
// >>> #CE_3.0.5 2025-07-14
					Vopd = &(ip->LOpd);
					copy_operand(Lopd, Vopd);	// copy var name and indexation info, they can be used to write the variable's value

					err = OpdStack_OpdVarDataRef(ip, TRUE, Lopd, &lValue, &errf, &VMLOCKFLAGS, OPDVARDATAREF_LOCKHARD);
					if (err || errf)
						goto exit;
					Lopd->deref = TRUE;			// >>> #CE_3.0.5 2025-07-14 : dereferce the operand form its variable

					// === execute
					err = exec_IncrementDecrement(ip, op, lValue, &errf);
					if (err || errf)
						goto exit;

					if (lValue == (_OperandBase*)Lopd)	{	// TRUE if lValue doesn't refer to a variable (i.e. Operand Stack contains a value)
						// === operation has been performed on data in the left-hand operand (top of the Operands Stack)
						// === this means the top of Operand Stack contains the new value and it must be applied to the variable
						// === IMPORTANT: it's typically not a whole variable value but a result of indexation
						// === so, we copy the operand value to the variable, according to indexation
						err = SetVarValue(ip, Vopd, lValue, &errf, VMLOCKFLAGS == OPDVARDATAREF_NOLOCK);
						// >>> #CE_3.0.5 2025-07-15 : disabled: indexation is only used with OPTYPE_IDENT, but if we keep var name in the Operands Stack, we also must keep its indexation
						//_IndexInfo_ResetIndexation(Lopd);
					}
					else {
						vp = __VarInfoPnt_To_VarRecordPnt(lValue);
						vp->writerID = ip->writerID;			// >>> #CE_3.0.7 2026-07-23
						__UpdateVarWritingTimeNow(vp);			// >>> #CE_3.1.0-r6 2026-09-17
						if (vp->hist)
							err = _VHistory_AddCurValue(vp);	// >>> #CE_3.0.7 2026-07-25
						// === operation has been performed directly on the vatiable 
						// === we preserve all the variable info on the top of the Operand Stack (i.e. result of operation is the variable)
					}
					goto exit;
// <<< #CE_3.0.5 2025-07-14

		case CAT_INCPOST :	// ### Postfix Increment/Decrement operators (++/--)
					Lopd = OpdStack_TopPnt(ip, &errf);
					if (errf)
						goto error_MissingOperand;
					if ((Lopd->type != OPTYPE_IDENT) || Lopd->deref)		// >>> #CE_3.0.5 2025-07-14
						goto error_OperandNotVar;
// >>> #CE_3.0.5 2025-07-14
					Vopd = &(ip->LOpd);
					copy_operand(Lopd, Vopd);	// copy var name and indexation info, they can be used to write the variable's value

					err = OpdStack_OpdVarDataRef(ip, TRUE, Lopd, &lValue, &errf, &VMLOCKFLAGS, OPDVARDATAREF_LOCKHARD);
					if (err || errf)
						goto exit;
					Lopd->deref = TRUE;			// >>> #CE_3.0.5 2025-07-14 : dereferce the operand form its variable

					if (lValue == (_OperandBase*)Lopd)	{	// TRUE if lValue doesn't refer to a variable (i.e. Operand Stack contains a value)
						// === lValue contains a pointer to the operand on the top of the Operand Stack.
						// === this means the top of Operand Stack contains an old value and it must stay there as a result of operation
						// === IMPORTANT: it's typically not a whole variable value but a result of indexation
						// === so, we should perform the operation on a temporary data copy and assign the result to the variable, according to indexation
						if (err = copy_operand(Lopd, Ropd))			// make a temporary copy of the operand value
							goto exit;

						// ===  execute the operation on temporary value
						err = exec_IncrementDecrement(ip, op, (_OperandBase*)Ropd, &errf);
						if (err || errf)
							goto exit;
						// ===  assign the value to the variable
						err = SetVarValue(ip, Vopd, (_OperandBase*)Ropd, &errf, VMLOCKFLAGS == OPDVARDATAREF_NOLOCK);
						// >>> #CE_3.0.5 2025-07-15 : disabled: indexation is only used with OPTYPE_IDENT, but if we keep var name in the Operands Stack, we also must keep its indexation
						//_IndexInfo_ResetIndexation(Lopd);
					}
					else {
						// === lValue contains a pointer to the variable's value inside VarMan
						// === so, we copy the old variable value to the top of the Operand Stack; herewith, it will be a value but not IDENT anymore
						// === afterward, execute the operation directly on the variable value in VarMan
						//err = CpxOperation(ip, op, lValue, NULL, &errf);
						if (err = Var_Copy((_OperandBase*)Lopd, lValue))
							goto exit;
						err = exec_IncrementDecrement(ip, op, lValue, &errf);
						vp = __VarInfoPnt_To_VarRecordPnt(lValue);
						vp->writerID = ip->writerID;			// >>> #CE_3.0.7 2026-07-23
						__UpdateVarWritingTimeNow(vp);			// >>> #CE_3.1.0-r6 2026-09-17
						if (vp->hist)
							err = _VHistory_AddCurValue(vp);	// >>> #CE_3.0.7 2026-07-25
					}
					goto exit;
// <<< #CE_3.0.5 2025-07-14

// <<< #CE_3.0 2025-04-21 / 2025-03-22
	}

	// Выполнить операцию
	// ------------------
	switch(op->n_ops) {

// ******************
// Invalid operations
// ******************
	case 0 :
		errf = LVTRUE;
		err = CE_InternalError(ip, "Invalid operator (N_Opd==0)",NULL,NULL);
		goto out;
	default :
		errf = LVTRUE;
		err = CE_InternalError(ip, "Invalid operator (N_Opd>2)",NULL,NULL);
		goto out;

// ****************
// Unary operations
// ****************
	case 1 :
		// ==========================================
		// !!! ОПЕРАНД РАСКРУЧИВАЕМ ПРЯМО В СТЕКЕ !!!
		// ==========================================
		Lopd = OpdStack_TopPnt(ip, &errf);
		if (errf)
			goto error_MissingOperand;
		err  = OpdStack_OpdValue(ip, Lopd, &errf);
		if (err || errf)	goto exit;

		// ***  Выполняем операцию  ***
		// ============================
		if (Lopd->type != OPTYPE_DBL) {
			err = CpxOperation(ip, op, (_OperandBase*)Lopd, (_OperandBase*)Ropd, &errf);
		}
		else {
			switch(op->code) {
				case OP1_LogNOT :	if (_DblToI32Bits(Lopd->num))	Lopd->num = 0;
									else						Lopd->num = 1;
									break;
				case OP1_BitNOT :	Lopd->num = ~_DblToI32Bits(Lopd->num);	break;
				case OP1_Plus :		Lopd->num = +Lopd->num;					break;
				case OP1_Minus :	Lopd->num = -Lopd->num;					break;
// >>> #CE_3.0 2025-03-18
				default:	goto error_BadOperator;
							// increment/decrement operators are handled earlier in the respective Operator Category
							break;
// <<< #CE_3.0 2025-03-18
			}
		}
		goto exit;


// *****************
// Binary operations
// *****************
	case 2 :
		// moved to "rest operators" section
		//isAssign = op2_names[op->code].isAssign;

		Ropd = OpdStack_TopPnt( ip, &errf);	//правый операнд
		OpdStack_DecLength( ip, &errf );
		Lopd = OpdStack_TopPnt( ip, &errf);	//левый операнд
		if (errf)
			goto error_MissingOperand;

		switch(op->code) {
			// ****  Диапазон индексов ****
			case OP2_IdxRange :
				err = IdxRangeOperator(ip, Lopd, Ropd, &errf);
				goto exit;

			// ****  Присваивание ****
			case OP2_Assign :
// >>> #CE_3.0.5 2025-07-13
				if ((Lopd->type != OPTYPE_IDENT) || (Lopd->deref)) 		// >>> #CE_3.0.5 2025-07-14
					goto error_LeftOperandNotVar;
				err = OpdStack_OpdVarDataRef(ip, TRUE, Ropd, &rValue, &errf, &VMLOCKFLAGS, OPDVARDATAREF_LOCKHARD);
				if (err || errf)
					goto exit;
				Lopd->deref = TRUE;			// >>> #CE_3.0.5 2025-07-14 : dereferce the left-hand operand form its variable
				// info on the target variable (l-value) stays on the top of the Operand Stack but we mark is as dereferenced
				err = SetVarValue(ip, Lopd, rValue, &errf, VMLOCKFLAGS == OPDVARDATAREF_NOLOCK);
				goto exit;
// <<< #CE_3.0.5 2025-07-13

			// **** Операции сравнения ****
			case OP2_Equal :
			case OP2_NotEqual :
				if (err = CompareOperandsForEquality( ip, Lopd, Ropd, &errf))
					goto out;
				if (op->code == OP2_NotEqual)
					Lopd->num = !(Lopd->num);
				goto exit;
		}

		// #########################
		// ===  REST OPERATORS  ====
		// #########################

		isAssign = op2_names[op->code].isAssign;
		if (isAssign) {
			Vopd = &(ip->LOpd);
			copy_operand(Lopd, Vopd);		// copy var name and indexation info, they can be used to write the variable's value
// >>> #CE_3.0.5 2026-07-12
			if ((Vopd->type != OPTYPE_IDENT) || (Vopd->deref))	// >>> #CE_3.0.5 2025-07-14
				goto error_LeftOperandNotVar;
			Lopd->deref = TRUE;				// >>> #CE_3.0.5 2025-07-14 : dereferce the left-hand operand form its variable
// <<< #CE_3.0.5 2026-07-12
		}
// >>> #CE_3.0.5 2026-07-12
		err = OpdStack_OpdVarDataRef(ip, TRUE,     Ropd, &rValue, &errf, &VMLOCKFLAGS, isAssign ? OPDVARDATAREF_LOCKHARD : OPDVARDATAREF_LOCKSOFT);
		if (err || errf)
			goto exit;
		err = OpdStack_OpdVarDataRef(ip, isAssign, Lopd, &lValue, &errf, &VMLOCKFLAGS, isAssign ? OPDVARDATAREF_LOCKHARD : OPDVARDATAREF_LOCKSOFT);
		if (err || errf)
			goto exit;

		// #####  NOT DBL SCALAR ##### (if any operand is not DBL scalar)
		if ((lValue->type != OPTYPE_DBL) || (rValue->type != OPTYPE_DBL)) {
			err = CpxOperation(ip, op, lValue, rValue, &errf);
			if (err || errf)
				goto exit;
			goto CheckAssign;
		}

		// ##### DBL SCALAR #####
		switch(op->code) {
		// **** Арифметические операции ****
			case OP2_AssPlus :
			case OP2_Plus :		lValue->num += rValue->num;
								break;
			case OP2_AssMul :
			case OP2_Mul :		lValue->num *= rValue->num;
								break;
			case OP2_AssMinus:
			case OP2_Minus:		lValue->num -= rValue->num;
								break;
			case OP2_AssDiv :
			case OP2_Div :		lValue->num /= rValue->num;
								break;
			case OP2_AssMod :
			case OP2_Mod :		lValue->num = fmod(lValue->num, rValue->num);
								break;
			case OP2_AssPower :
			case OP2_Power :	lValue->num = pow(lValue->num, rValue->num);
								break;
		// **** Сдвиги ****
#pragma warning (push)
#pragma warning (disable: 6297)		// x << y causes this warning, in CE help we warn end-users about undefined result if y is out of 0..31
			case OP2_AssLShift :
			case OP2_LShift :	lValue->num = _DblToI32Bits(lValue->num) << _DblToI32Bits(rValue->num);
								break;
#pragma warning (pop)
			case OP2_AssRShift :
			case OP2_RShift :	lValue->num = _DblToI32Bits(lValue->num) >> _DblToI32Bits(rValue->num);
								break;
		// **** Поразрядные операции ****
			case OP2_AssBitAND :
			case OP2_BitAND :	lValue->num = _DblToI32Bits(lValue->num) & _DblToI32Bits(rValue->num);
								break;
			case OP2_AssBitOR :
			case OP2_BitOR :	lValue->num = _DblToI32Bits(lValue->num) | _DblToI32Bits(rValue->num);
								break;
			case OP2_AssBitXOR :
			case OP2_BitXOR :	lValue->num = _DblToI32Bits(lValue->num) ^ _DblToI32Bits(rValue->num);
								break;
		// **** Операции отношения ****
			case OP2_LessOrEq :	lValue->num = (lValue->num <= rValue->num) ? 1.0 : 0.0;
								break;
			case OP2_Less :		lValue->num = (lValue->num <  rValue->num) ? 1.0 : 0.0;
								break;
			case OP2_GreatOrEq:	lValue->num = (lValue->num >= rValue->num) ? 1.0 : 0.0;
								break;
			case OP2_Great :	lValue->num = (lValue->num >  rValue->num) ? 1.0 : 0.0;
								break;
		// **** Логические операции ****
			case OP2_LogAND :	lValue->num = _DblToI32Bits(lValue->num) && _DblToI32Bits(rValue->num);
								break;
			case OP2_LogOR :	lValue->num = _DblToI32Bits(lValue->num) || _DblToI32Bits(rValue->num);
								break;
			case OP2_LogXOR :	lValue->num = LogXOR(_DblToI32Bits(lValue->num) , _DblToI32Bits(rValue->num));
								break;
		// *****************************
			default :			goto error_BadOperator;
		}
		break;		// goto CheckAssign;
// <<< #CE_3.0.5 2026-07-12
	}

CheckAssign:
/*
	// ### we come here if only an operator has been done NOT DIRECTLY ON A VARIABLE
	if (isAssign && !errf) {
		// ***** Unlock VarMan to avoid a deadlock because SetVarValue() locks it also
		if      (VMLOCKFLAGS == OPDVARDATAREF_LOCKHARD)	VARMAN_UNLOCK_EXCLUSIVE();
		else if (VMLOCKFLAGS == OPDVARDATAREF_LOCKSOFT)	VARMAN_UNLOCK_SHARED();
		VMLOCKFLAGS = OPDVARDATAREF_NOLOCK;

		// ***** DO ASSIGNMENT *****
		err = SetVarValue(ip, Vopd, lValue, &errf, TRUE);
		// >>> #CE_3.0.5 2025-07-15 : indexation is only used with OPTYPE_IDENT, but if we keep var name in the Operands Stack, we also must keep its indexation
	}
*/
	if (isAssign && !errf) {
		if (lValue == (_OperandBase*)Lopd) {		// TRUE if lValue doesn't refer to a variable (i.e. Operand Stack contains a value)
			// === operation has been performed on data in the left-hand operand (top of the Operands Stack)
			// === this means the top of Operand Stack contains the new value and it must be applied to the variable
			// === IMPORTANT: it's typically not a whole variable value but a result of indexation
			// === so, we copy the operand value to the variable, according to indexation

			// ***** Unlock VarMan to avoid a deadlock because SetVarValue() locks it also
#pragma warning (push)
#pragma warning (disable: 26110)	// MS Code Analyser can't understand the idea
			if      (VMLOCKFLAGS == OPDVARDATAREF_LOCKHARD)	VARMAN_UNLOCK_EXCLUSIVE();
			else if (VMLOCKFLAGS == OPDVARDATAREF_LOCKSOFT)	VARMAN_UNLOCK_SHARED();
			VMLOCKFLAGS = OPDVARDATAREF_NOLOCK;
#pragma warning (pop)
			// ***** DO ASSIGNMENT *****
			err = SetVarValue(ip, Vopd, lValue, &errf, TRUE);
			// >>> #CE_3.0.5 2025-07-15 : indexation is only used with OPTYPE_IDENT, but if we keep var name in the Operands Stack, we also must keep its indexation
		}
		else {
			// === lValue contains a pointer to the variable's value inside VarMan
			// === so, we have performed an operation directly on a variable value within VarMan
			// === afterwards, we need to complete the assignment by updating some variable's service data
			vp = __VarInfoPnt_To_VarRecordPnt(lValue);
			vp->writerID = ip->writerID;			// >>> #CE_3.0.7 2026-07-25
			__UpdateVarWritingTimeNow(vp);			// >>> #CE_3.1.0-r6 2026-09-17
			if (vp->hist)
				err = _VHistory_AddCurValue(vp);	// >>> #CE_3.0.7 2026-07-25
		}
	}
exit:
	if (errf && !err) {
		switch (op->n_ops) {
			case 1 :	opname = op1_names[op->code].oname;	break;
			case 2 :	opname = op2_names[op->code].oname;	break;
			default :	goto out;
		}
		err = CE_Error(ip, " : %s", (LStrHandle)opname, NULL);
	}
out:
	if (errf) {
		*err_f = errf;
	}
	// ***** Unlock VarMan if we had locked it
#pragma warning (push)
#pragma warning (disable: 26110)	// MS Code Analyser can't understand the idea
	if      (VMLOCKFLAGS == OPDVARDATAREF_LOCKHARD)	VARMAN_UNLOCK_EXCLUSIVE();
	else if (VMLOCKFLAGS == OPDVARDATAREF_LOCKSOFT)	VARMAN_UNLOCK_SHARED();
	VMLOCKFLAGS = OPDVARDATAREF_NOLOCK;
#pragma warning (pop)

// >>> #CE_3.0.5 2025-07-13
//#define		TRACE_ME_HERE
#if defined(TRACE_ME_HERE)
{
	LVBoolean err = LVFALSE;
	Lopd = OpdStack_TopPnt(ip, &err);

	if (op->category == CAT_COMMON) {
		switch (op->n_ops) {
		case 1 :	opname = op1_names[op->code].oname;	break;
		case 2 :	opname = op2_names[op->code].oname;	break;
		default:	opname = "<unknown operation>";
		}
	}
	else if	(op->category == CAT_SEMICOLON) opname = " ; ";
	else if (op->category == CAT_COMMA)		opname = " , ";
	else if (op->category == CAT_INDEX)		opname = "[..]";
	else if (op->category == CAT_INCPREF) {
		if      (op->code == OP1_IncPref)	opname = "++v";
		else if (op->code == OP1_DecPref)	opname = "--v";
		else								opname = "<unknown_prefix_op";
	}
	else if (op->category == CAT_INCPOST) {
		if      (op->code == OP1_IncPost)	opname = "v++";
		else if (op->code == OP1_DecPost)	opname = "v--";
		else								opname = "<unknown_postfix_op";
	}
	else 									opname = "<unknown_category>";
	if (err) {
		DbgPrintf("result (%s): ---\n", opname);
	}
	else {
		if (Lopd->type == OPTYPE_IDENT)
			if (Lopd->deref)	DbgPrintf("result (%s): deref.var='%H' #idxs=%d -- OpdStkLen=%d\n", opname, getIdentString(Lopd), Lopd->iip->qty, ip->OpdNum);
			else				DbgPrintf("result (%s): valid_var='%H' #idxs=%d -- OpdStkLen=%d\n", opname, getIdentString(Lopd), Lopd->iip->qty, ip->OpdNum);
		else					DbgPrintf("result (%s): valuetype=%d #idxs=%d -- OpdStkLen=%d\n" , opname, Lopd->type, Lopd->iip->qty, ip->OpdNum);
	}
}
#undef TRACE_ME_HERE
#endif
// <<< #CE_3.0.5 2025-07-13
	return err;

// ----------------------------------------------
// error handlers
// ----------------------------------------------
error_MissingOperand:
	errf = LVTRUE;
	err  = CE_Error(ip, "Missing operand", NULL,NULL);
	goto exit;	// add operator name

error_OperandNotVar:
	errf = LVTRUE;
	err  = CE_Error(ip, "Operand must be a variable",NULL,NULL);
	goto exit;	// add operator name

error_LeftOperandNotVar:
	errf = LVTRUE;
	err  = CE_Error(ip, "Left operand in assignment must be a variable",NULL,NULL);
	goto exit;	// add operator name

error_BadOperator:
	errf = LVTRUE;
	err  = CE_InternalError(ip, "Unexpected Operator: code=%d, category=%d)", op->code, op->category);
	goto out;	// don't add operator name !
}
