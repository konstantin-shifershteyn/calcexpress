/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#ifndef	__FastFunc_Subr_C__
#define	__FastFunc_Subr_C__

// =============================================================
// Подготавливает результат если FastFunc вызвана без параметров
// =============================================================
__inline MgErr	ff_PseudoArg( _CE_Instance *ip, float64 num)
{
//	LVBoolean	errf = LVFALSE;

	ip->LOpd.type = OPTYPE_DBL;
	ip->LOpd.num  = num;
	ip->LOpd.iip  = NULL;		// !!! it will be ignored in OpdStack_PUSH() because of datatype (not OPTYPE_IDENT)
	ip->LOpd.vpnt = NULL;		// >>> #CE_3.0.2 2026-06-19
	return  OpdStack_PUSH( ip, &(ip->LOpd));
}

// =============================================================
// Определяет значение аргумента и проверяет его тип
// =============================================================
__inline MgErr	ff_GetArgumentValue(
					_CE_Instance	*ip,
					_Operand		*arg,
					uInt32			type,
					char			*errMsg,
					LVBoolean		*err_f)
{
	MgErr	err = noErr;
	if ((arg->type == OPTYPE_IDENT) && (type != OPTYPE_IDENT))
	{
		err = GetVariable( ip, arg, (_OperandBase*)arg, err_f);
		if (err || *err_f)	return err;
	}
	if (arg->type != type) {
		err = CE_Error( ip, errMsg, NULL, NULL );
		*err_f = LVTRUE;
	}
	return err;
}

#endif

