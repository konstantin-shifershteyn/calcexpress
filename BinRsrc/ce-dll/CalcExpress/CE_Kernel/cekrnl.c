/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


#include <extcode.h>
#include <lvsup\lvsup.h>

#include <math.h>
#include <float.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>

// ------------------

#include <ceKernel.h>
#include <ceError.h>
#include <VarMan.h>

//#include <Identifier_Routines.h>
//#include <Index_Routines.h>
#include <Operand_Routines.h>

#include <ceAccessContext.h>
#include <rtErrorCode.h>

#include <celib\cdb.h>
#include <celib\ceMemLib.h>

#include <FastFuncList.h>


//=======================

#include "cekrnl_Parser.c"
#include "cekrnl_Evaluator.c"

#include "FastFuncList.c"
#include "MathConst.c"

#include "cekrnl_Diagnostic.c"
#include "cekrnl_CntxCreate.c"
#include "cekrnl_CntxDestroy.c"
#include "cekrnl_CntxFreeMem.c"


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
MgErr	__cdecl	cekrnl_Initializing(void)
{
	setlocale(LC_NUMERIC, "C");	// *** ”станавливаем точку как дес€тичный разделитель
	MathConst_Initialize();		// *** »нициализаци€ мат. констант
	return noErr;
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
MgErr	__cdecl	cekrnl_Finalizing(void)
{
	return noErr;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
MgErr	__cdecl	cekrnl(
			int32				cmd,
			_CE_InstancePnt		*InstanceRef,
			_CEKernel_Par_Parse	*ParParse,
			_CEKernel_Par_Eval	*ParEval,
			LVBoolean			*err_f	)
{
	_CE_Instance	*ip;
	MgErr		err = noErr;

	switch(cmd & CEK_CMD_CNTX_MASK) {

		case 0:	break;

		case CEK_CMD_Cntx_Create :	// create CE context
			if (!(*err_f))
				err = CEKernel_Context_Create(InstanceRef);
			return	err;

		case CEK_CMD_Cntx_Destroy :	// destroy CE context
			return CEKernel_Context_Destroy(*InstanceRef);

		case CEK_CMD_Cntx_FreeMem :	// free allocated memory
			if (!(*err_f))
				err = CEKernel_Context_FreeMem(*InstanceRef, TRUE);
			return	err;

		default :
			return	mgArgErr;
	}
	// ==============
	ip = *InstanceRef;
/*
	if (err = CECheckReference(ip)) {
		*err_f = LVTRUE;
		goto out;
	}
*/
	// -------------------------------------
	if (ParParse) {

		if (cmd & CEK_CMD_PARSE_MASK) {
			ParParse->tmp_Mode = cmd & CEK_CMD_PARSE_MASK;
			err = CEKernel_Parser(ip, ParParse, err_f);
		}
		else {
			if (cmd & CEK_CMD_DIAG_MASK) {
				err = CEKernel_Diag(ip, (_CEKernel_Par_Diag*)ParParse, err_f);
			}
		}
		// выходим только если ошибка, т.к. может быть затребовано выполнение
		if (err)
			return err;
	}
	// -------------------------------------
	if (ParEval) {
		if (ParEval->SetInPars) {
			ip->VCHandle		=  ParEval->in_VCHandle;
			ip->procPars.autoVar= (ParEval->in_AutoVar != 0);
			ip->procPars.en_cpx	= (ParEval->in_EnaCpx  != 0);
		}
		ParEval->out_NotEnd		= FALSE;
		ParEval->out_NumOfFuncArgs	= 0;

		switch(cmd & CEK_CMD_EXEC_MASK) {
			case CEK_CMD_Exec_Script :	 // Perform evaluation
				err = CEKernel_Evaluator(ip, ParEval, err_f);
				break;
		}
	}
//out:
	return err;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// >>> #CE_3.0.5 2025-07-18
MgErr CEKernel_MergeExpressions(_CE_Instance *ip_out, _CE_Instance *ip_add)
{
	int32			idx_out, oldSize_out;
	int32			idx_add;
	_ExpresElement	*expElem_add, *expElem_out;
	_ExpresElement	*expData_add;
	int32			expSize_add;
	MgErr			err = noErr;

	expSize_add	= ip_add->expArrLen;	// size of additional Expression Array
	expData_add	= ip_add->expArrDat;	// pointer to data of additional Expression Array

	idx_out		=						// the initial index of the target Expression Array
	oldSize_out	= ip_out->expArrLen;	// initial size of the target Expression Array

//	DbgPrintf(__FUNCTION__"()\n");
	for (idx_add=0; idx_add < expSize_add; idx_add++ ) {

		expElem_add = &(expData_add[idx_add]);	// pointer to source expression element

		if (expElem_add->IsOps) {
			// ### copy operator from the source expression array to the target expression array
			err = ExAr_PutOperation(ip_out, &(expElem_add->ops), &idx_out);		// it can rellocate the expression array
// >>> #CE_3.0.7 2026-07-25
			expElem_out = &(ip_out->expArrDat[idx_out-1]);		// pointer to the added element in the expression array
			// ### for FuncVIs, we must refresh index of expression element with the FuncVI name
			if (expElem_out->ops.category == CAT_FUNC) {
				expElem_out->ops.misc += oldSize_out;
//				DbgPrintf(__FUNCTION__"(): FUNCVI new name index = %d\n", expElem_out->ops.misc);
			}
//			else { DbgPrintf(__FUNCTION__"(): OP_Category = %d, OP_Code = %d\n", expElem_out->ops.category, expElem_out->ops.code); }
// <<< #CE_3.0.7 2026-07-25
		}
		else {
			// ### copy operand from the source expression array to the target expression array
			err = ExAr_PutOperand(ip_out, &(expElem_add->opd), &idx_out);
// >>> #CE_3.0.7 2026-07-25
			expElem_out = &(ip_out->expArrDat[idx_out-1]);		// pointer to the added element in the expression array
			expElem_out->ops = expElem_add->ops;				// copy operator also! e.g. operand's IDENT type can be a function name (FuncVI)
// <<< #CE_3.0.7 2026-07-25
		}
		if (err)
			goto out;
	}
	// resize the target Expression Array to new number of expressions within it
	err = ExAr_Resize(ip_out, idx_out );
out:
	return err;
}
// <<< #CE_3.0.5 2025-07-18

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// >>> #CE_3.1.0-r6 2026-09-24
const _td_named_dbl_const	__namedDblConst[] = {
			{ "NAN"			, 0.0					, &_NaN_	, "Not A Number"	}
		,	{ "INF"			, 0.0					, &_Inf_P_	, "Infinity"		}
		,	{ "TRUE"		, 1						, NULL		, "True (1)"		}
		,	{ "FALSE"		, 0						, NULL		, "False (0)"		}
		,	{ "INT_MAX"		, (float64)INT_MAX		, NULL		, "Max I32 value"	}
		,	{ "INT_MIN"		, (float64)INT_MIN		, NULL		, "Min I32 value"	}
		,	{ "UINT_MAX"	, (float64)UINT_MAX		, NULL		, "Max U32 value"	}
//		,	{ "DBL_DIG"		, DBL_DIG				, NULL		, "decimal digits of precision" }
		,	{ "DBL_EPSILON"	, DBL_EPSILON			, NULL		, "Machine Epsilon"	}
		,	{ "DBL_MAX"		, DBL_MAX				, NULL		, "Max DBL value"	}
		,	{ "DBL_MIN"		, DBL_MIN				, NULL		, "Min DBL value"	}
		,	{ "M_E"			, M_E					, NULL		, "the value of e"	}
		,	{ "M_1_E"		, (1.0/M_E)				, NULL		, "1/e"				}
		,	{ "M_LOG2E"		, M_LOG2E				, NULL		, "log2(e)"			}
		,	{ "M_LOG10E"	, M_LOG10E				, NULL		, "log10(e)"		}
		,	{ "M_LN2"		, M_LN2					, NULL		, "ln(2)"			}
		,	{ "M_LN10"		, M_LN10				, NULL		, "ln(10)"			}
		,	{ "M_LN_PI"		, _LN_PI_				, NULL		, "ln(pi)"			}
		,	{ "M_PI"		, M_PI					, NULL		, "the value of pi"	}
		,	{ "M_PI_2"		, M_PI_2				, NULL		, "pi/2"			}
		,	{ "M_PI_4"		, M_PI_4				, NULL		, "pi/4"			}
		,	{ "M_1_PI"		, M_1_PI				, NULL		, "1/pi"			}
		,	{ "M_2_PI"		, M_2_PI				, NULL		, "2/pi"			}
		,	{ "M_2_SQRTPI"	, M_2_SQRTPI			, NULL		, "2/sqrt(pi)"		}
		,	{ "M_2PI"		, _2PI_					, NULL		, "2*pi"			}
		,	{ "M_SQRT2"		, M_SQRT2				, NULL		, "sqrt(2)"			}
};
// ---------------------
int32	__namedDblConst_size(void)
{
	return sizeof(__namedDblConst)/sizeof(__namedDblConst[0]);
}
// ---------------------
__forceinline Bool32	__cmpSymStr(const char *sym, const uChar *txtPnt, int32 txtLen)
{
	if (txtLen != strlen(sym))
		return FALSE;
	return (memcmp(sym, txtPnt, txtLen) == 0);
}
// ---------------------
const _td_named_dbl_const * __namedDblConst_find(uChar *txtPnt, int32 txtLen)
{
	int i;
	const _td_named_dbl_const * ncp = &__namedDblConst[0];

	for (i=0; i < sizeof(__namedDblConst)/sizeof(__namedDblConst[0]); i++, ncp++) {
		if (__cmpSymStr(ncp->name, txtPnt, txtLen))
			return ncp;
	}
	return NULL;
}
// ---------------------
const _td_named_dbl_const * __namedDblConst_pnt(int32 idx)
{
	if ((idx < 0) ||  (idx >= __namedDblConst_size()))
		return NULL;
	return &(__namedDblConst[idx]);
}
// <<< #CE_3.1.0-r6 2026-09-24
