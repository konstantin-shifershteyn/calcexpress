/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__CE_KERNEL_H__
#define	__CE_KERNEL_H__

#include <extcode.h>

#include <CE_ProductInfo.h>
#include <CalcExpress.h>
#include <VarMan.h>
#include <VMEngine_Routines.h>

#include <MathConst.h>

// =================================================================

typedef struct {
	LStrHandle	in_Script;
	Bool32		out_ParseFlag;
	int32		tmp_Mode;
	Bool32		in_enaNamedConst;	// >>> #CE_3.1.0-r6 2026-09-25
} _CEKernel_Par_Parse;

typedef struct {
	Bool32		SetInPars;
	int32		in_VCHandle;
	Bool32		in_AutoVar;
	Bool32		in_EnaCpx;
	int32		io_ExpIndex;
	int32		out_NumOfFuncArgs;
	Bool32		out_NotEnd;
} _CEKernel_Par_Eval;

typedef	struct {
	int32		in_Mode;
	LStrHandle	io_List;
	int32		out_status;
} _CEKernel_Par_Diag;

// ------------------------------

#define	CEK_CMD_NOTHING		0x00000000

#define	CEK_CMD_PARSE_MASK	0x000000FF
#define	CEK_CMD_Parse		0x00000001
#define	CEK_CMD_Parse_Auto	0x00000002
#define	CEK_CMD_Parse_Test	0x00000003

#define	CEK_CMD_EXEC_MASK	0x0000FF00
#define	CEK_CMD_Exec_Script	0x00000100

#define	CEK_CMD_CNTX_MASK	0x00FF0000
#define	CEK_CMD_Cntx_Create	0x00010000
#define	CEK_CMD_Cntx_Destroy	0x00020000
#define	CEK_CMD_Cntx_FreeMem	0x00030000

#define	CEK_CMD_DIAG_MASK	0x0F000000
#define	CEK_CMD_Diag_Script	0x01000000

// values for the 'in_Mode' member of _CEKernel_Par_Diag
#define	CEK_MOD_Diag_FuncVI		0
#define	CEK_MOD_Diag_Vars		1
#define	CEK_MOD_Diag_FastFunc	2
#define	CEK_MOD_Diag_Script		3
#define	CEK_MOD_Diag_Functions	4	// new in 2.5.1

// ==================================================================

// ***  CalcExpress Kernel: core function and pointer to it: type definitions
// ***
typedef	MgErr	__cdecl	__ceKernel(	int32			cmd,
					_CE_InstancePnt		*InstanceRef,
					_CEKernel_Par_Parse	*ParParse,
					_CEKernel_Par_Eval	*ParEval,
					LVBoolean		*err_f	);
typedef	__ceKernel  *	__ceKernel_Pnt;

#ifdef __cplusplus
extern "C" {
#endif

// ***  CalcExpress Kernel: core function
extern	__ceKernel	cekrnl;

extern	MgErr	__cdecl	cekrnl_Initializing(void);
extern	MgErr	__cdecl	cekrnl_Finalizing(void);

// >>> #CE_3.0.5 2025-07-18
extern	MgErr CEKernel_MergeExpressions(_CE_Instance *ip_out, _CE_Instance *ip_add);	// used in CSE to group expression-statements
// <<< #CE_3.0.5 2025-07-18

// ***
MgErr	CEKernel_Context_Create( _CE_InstancePnt *InstanceRef);
MgErr	CEKernel_Context_Destroy(_CE_Instance *ip);
MgErr	CEKernel_Context_FreeMem(_CE_Instance *ip, Bool32 DestroyParsedScript);
int32	CEKernel_Context_DestroyAll(void);

MgErr	CEKernel_Evaluator( _CE_Instance *ip, _CEKernel_Par_Eval  *Pars, LVBoolean *err_f);
MgErr	CEKernel_Parser(    _CE_Instance *ip, _CEKernel_Par_Parse *Pars, LVBoolean *err_f);
MgErr	CEKernel_Diag(      _CE_Instance *ip, _CEKernel_Par_Diag  *Pars, LVBoolean *err_f);


// ##########  CE_Kernel\xx_Evaluator\Subr_Variables.c  ##########

// >>> #CE_3.0.2 2026-06-20
__forceinline	Bool32	__fastcall	CheckForHardVarManChange(_CE_Instance *ip)
{
	if (ip->vmChangeCnt == VM_HardChangeCounter())
		return FALSE;
	ip->vmChangeCnt = VM_HardChangeCounter();
	return TRUE;
}

void	__fastcall	ClearVarPointers(_CE_Instance *ip);
// <<< #CE_3.0.2 2026-06-20

Bool32	__fastcall	GetPointerToVariable2(  _CE_Instance *ip, _VarInfo **vipp, _Operand *var, LVBoolean *err_f, uInt16 *err_c);
Bool32	__fastcall	GetPointerToIndexedVar2(_CE_Instance *ip, _VarInfo **vipp, _Operand *var, LVBoolean *err_f);
MgErr	__fastcall	GetVariable( _CE_Instance *ip, _Operand *var, _OperandBase *value, LVBoolean *err_f);
MgErr	__fastcall	SetVarValue( _CE_Instance *ip, _Operand *var, _OperandBase *value, LVBoolean *err_f, Bool32 LockVarMan);

// >>> #CE_3.1.0-r6 2026-09-24

typedef struct {
	const char	*name;
	float64		val;
	float64		*valPnt;
	const char	*dscr;
} _td_named_dbl_const;

//extern const _td_named_dbl_const	__namedDblConst[];

extern	int32	__namedDblConst_size(void);
extern	const _td_named_dbl_const * __namedDblConst_find(uChar *txtPnt, int32 txtLen);
extern	const _td_named_dbl_const * __namedDblConst_pnt(int32 idx);

// <<< #CE_3.1.0-r6 2026-09-24

#ifdef __cplusplus
}
#endif

#endif
