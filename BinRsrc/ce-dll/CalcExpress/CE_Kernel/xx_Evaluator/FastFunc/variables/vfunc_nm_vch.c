/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ############################################################################################
// VarMan functions of the type:	function ( varname, vch )
// ############################################################################################

// ### helper function to get arguments
MgErr	_vfunc_nam_vch(
				_CE_Instance	*ip,
				int32			arg_qty,	//кол-во аргументов
				_Operand		**var,
				_Operand		**result,
				int32			*VCHandle,
				LVBoolean		*err_f)
{
	_Operand	*arg;
	int32		idx;
	MgErr		err = noErr;

	*VCHandle = 0;

	idx = ip->OpdNum - arg_qty + 1;	// индекс 1-го аргумента
	arg = OpdStack_EPnt(ip, idx);	// указатель на 1-й аргумент, он же результат

	*var = *result = arg;

	// =======================================
	// Определить имя переменной (1-й операнд)
	// =======================================
	err = ff_GetArgumentValue(ip, arg, OPTYPE_STR, ErrMsg_bad_Var_Name, err_f);
	if (err || *err_f)	goto out;

	// ==================================
	// Определить VC handle (2-й операнд)
	// ==================================
	if (arg_qty == 2) {
		arg = OpdStack_EPnt(ip, ++idx);		// указатель на 2-й аргумент

		err = ff_GetArgumentValue(ip, arg, OPTYPE_DBL, ErrMsg_bad_VCHandle2, err_f);
		if (err || *err_f)	goto out;
		*VCHandle = _DblToI32(arg->num);
	}
out:
	// ===  СТЕК ОПЕРАНДОВ. КОЛ-ВО ЭЛЕМЕНТОВ
	ip->OpdNum -= arg_qty-1;

	if (!(*VCHandle))
		*VCHandle = ip->VCHandle;
	return err;
}

// ==============================================================
//	vundef(name [,vch])
// ==============================================================
MgErr v_vundef(	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,
				int32			arg_qty,
				LVBoolean		*err_f)
{
	_Operand	*vname;
	_Operand	*res;		// Указатель на имя ф-ии, он-же и результат
//	_Operand	defval;
	int32		VCHandle;
	uInt16		errc = 0;
	LVBoolean	errf = LVFALSE;
	LVBoolean	glob;
	MgErr		err;


	// Определить параметры аргументов
	// ===============================
	err = _vfunc_nam_vch( ip, arg_qty, &vname, &res, &VCHandle, err_f );
	if (err || *err_f)
		goto out;

	glob =	(VCHandle == -1);

	err = vmCmd_Var_Destroy(
			VCHandle,						// handle
			&glob,							// global flag
			vname->str,						// name of variable
			&errf, &errc );

		//*VCH, glob, name, err_f, err_c);
/*
// >>> #CE_3.0.2 2026-06-08
	err =	VMEngine( VM_CMD_VAR_DESTROY,	// cmd
			&VCHandle,						// handle
			&glob,							// global flag
			vname->str,						// name of variable
			(_VarInfo*)&defval,				// type & value of variable
			0,								// writerID		// >>> #CE_3.0.7 2026-07-23
			NULL, NULL,						// extfunc & extdata
			&errf, &errc
		);
// <<< #CE_3.0.2 2026-06-08
*/
out:
	res->num	= __vfunc_err_code(errc);
	res->type	= OPTYPE_DBL;
	return err;
}

// ==============================================================
//	vtest(name [,vch])
// ==============================================================
MgErr v_vtest (	_CE_Instance	*ip,
				_FastFuncInfo	*fInfo,
				int32			arg_qty,
				LVBoolean		*err_f)
{
	_Operand	*vname;
	_Operand	*res;		// Указатель на имя ф-ии, он-же и результат
	_Operand	defval;
	int32		VCHandle;
	uInt16		errc = 0;
	LVBoolean	errf = LVFALSE;
	LVBoolean	glob;
	MgErr		err;

	// Подготовить блок параметров
	// ===========================
	defval.type = OPTYPE_DBL;
	defval.num = 0;
	defval.str = NULL;		// !!! LStrHandle = 0 !!!

	// Определить параметры аргументов
	// ===============================
	err = _vfunc_nam_vch( ip, arg_qty, &vname, &res, &VCHandle, err_f );
	if (err || *err_f)
		goto out;

	glob	= (VCHandle == -1);
	errf	= LVFALSE;

	err = vmCmd_Var_GetValueEx(VCHandle, &glob, vname->str, (_VarInfo*)&defval, NULL, NULL, NULL, &errf, &errc);
out:
	if (errf)	res->num = __vfunc_err_code(errc);
	else 		res->num = defval.type;
	res->type = OPTYPE_DBL;
	return noErr;
}

// >>> #CE_3.1.0-r6 2026-09-20
// ########################################################################
// Functions to return variable info:	function ( varname, vch )
// ########################################################################

enum _vstats_type	{
	kVSTYPE_DTYPE,
	kVSTYPE_CRE_ID,
	kVSTYPE_CRE_MS,
	kVSTYPE_CRE_TS,
	kVSTYPE_WR_ID,
	kVSTYPE_WR_MS,
	kVSTYPE_WR_TS,
	kVSTYPE_WR_CNT
} ;

MgErr _vfunc_vstats ( enum	_vstats_type	type,
							_CE_Instance	*ip,
							_FastFuncInfo	*fInfo,
							int32			arg_qty,
							LVBoolean		*err_f)
{
	_Operand	*vname;
	_Operand	*res;		// Указатель на имя ф-ии, он-же и результат
	int32		VCHandle;
	_VarRecord	*vp;
	uInt16		errc = 0;
	LVBoolean	glob;
	MgErr		err;

	// ### Take arguments and prepare params for execution
	err = _vfunc_nam_vch( ip, arg_qty, &vname, &res, &VCHandle, err_f );
	if (err || *err_f)
		goto out;
	glob = (VCHandle < 0 ? TRUE : FALSE);

	// ### Execute
	VARMAN_LOCK_SHARED();
	if (vp = __GetVarPointer(VCHandle, &glob, vname->str, NULL, &errc)) {
		switch (type) {
			case kVSTYPE_DTYPE	:	res->num = (float64)(vp->inf.type);		break;
			case kVSTYPE_CRE_ID	:	res->num = (float64)(vp->creatorID);	break;
			case kVSTYPE_CRE_MS	:	res->num = (float64)(vp->createMS);		break;
			case kVSTYPE_CRE_TS	:	res->num = (float64)(vp->createTS);		break;
			case kVSTYPE_WR_ID	:	res->num = (float64)(vp->writerID);		break;
			case kVSTYPE_WR_MS	:	res->num = (float64)(vp->writeMS);		break;
			case kVSTYPE_WR_TS	:	res->num = (float64)(vp->writeTS);		break;
			case kVSTYPE_WR_CNT	:	res->num = (float64)(vp->writeCnt);		break;
			default:				res->num = -1.0;						break;
		}
	}
	VARMAN_UNLOCK_SHARED();		// >>> #CE_3.0.2 2026-06-08

out:
	if (errc)
		res->num = __vfunc_err_code(errc);
	res->type = OPTYPE_DBL;
	return noErr;
}

#define	VAR_STATS_FUNC(fname, type)		MgErr fname(_CE_Instance *ip, _FastFuncInfo *fInfo, int32 arg_qty, LVBoolean *err_f)	\
													{ return _vfunc_vstats (type, ip, fInfo, arg_qty, err_f); }
VAR_STATS_FUNC( v_vtype		, kVSTYPE_DTYPE		)
VAR_STATS_FUNC( v_vstcid	, kVSTYPE_CRE_ID	)
VAR_STATS_FUNC( v_vstcms	, kVSTYPE_CRE_MS	)
VAR_STATS_FUNC( v_vstcts	, kVSTYPE_CRE_TS	)
VAR_STATS_FUNC( v_vstwid	, kVSTYPE_WR_ID		)
VAR_STATS_FUNC( v_vstwms	, kVSTYPE_WR_MS		)
VAR_STATS_FUNC( v_vstwts	, kVSTYPE_WR_TS		)
VAR_STATS_FUNC( v_vstwcnt	, kVSTYPE_WR_CNT	)

// <<< #CE_3.1.0-r6 2026-09-20