/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#ifndef	__CE_EVAL_OPERATORS_H__
#define	__CE_EVAL_OPERATORS_H__


#define	LogXOR(a,b)	(((a) && !(b)) || (!(a) && (b)))

__inline MgErr EvalOperator(_CE_Instance *ip, _Operation *op, LVBoolean *err_f);

// ------------------------------------------------

typedef MgErr	(*_op_dbl)();
typedef MgErr	(*_op_cdb)();

typedef struct {
	uChar		*oname;
	Bool32		isAssign;
	_op_dbl		exec_dbl;
	_op_cdb		exec_cdb;
} _OpsNames;

extern	_OpsNames	op1_names[];	// list of unary operators
extern	_OpsNames	op2_names[];	// list of binary operators

// >>> #CE_3.0.5 2025-07-14
MgErr op_opd1	( _CE_Instance   *ip, _op_dbl exec_dbl, _op_cdb exec_cdb, _OperandBase *Lopd, LVBoolean *err_f);
MgErr op_opd2	( _CE_Instance   *ip, _op_dbl exec_dbl, _op_cdb exec_cdb, _OperandBase *Lopd, _OperandBase *Ropd, LVBoolean *err_f);

MgErr CpxOperation( _CE_Instance *ip, _Operation *op, _OperandBase *Lopd, _OperandBase *Ropd, LVBoolean *err_f);
// <<< #CE_3.0.5 2025-07-14

////////////////////////////////////////////////////////////////


// ########  CE_Kernel\xx_Evaluator\Subr_Operators.c  ##########

MgErr CompareOperandsForEquality(_CE_Instance *ip, _Operand *Lopd, _Operand *Ropd, LVBoolean *err_f);

__forceinline MgErr __fastcall  exec_IncrementDecrement(
					_CE_Instance	*ip,
					_Operation		*op,
					_OperandBase	*lValue,
					LVBoolean		*err_f)
{
	if (lValue->type != OPTYPE_DBL)
		return CpxOperation(ip, op, lValue, NULL, err_f);

	switch (op->code) {
		case OP1_IncPref :
		case OP1_IncPost :	lValue->num += 1.0;
							break;
		case OP1_DecPref :
		case OP1_DecPost :	lValue->num -= 1.0;
							break;
		default :
			*err_f = LVTRUE;
			return CE_InternalError(ip, "Unexpected Operator: code=%d, category=%d)", op->code, op->category);
	}
	return noErr;
}


#endif
