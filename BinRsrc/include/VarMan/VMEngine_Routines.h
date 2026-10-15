/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__VarMan_Engine_Routines_H__
#define	__VarMan_Engine_Routines_H__

#include <extcode.h>
#include <lvsup\lvsup.h>
#include <celib\ceMemLib.h>
#include <celib\ceTime.h>

#include <VarMan.h>
#include <VMEngine.h>
#include <VHistory.h>

#ifdef  __cplusplus
extern "C" {
#endif


MgErr	vmCmd_Context_Open(		int32 *vch,	LStrHandle Creator, int32 *LifeTime, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_Context_GetInfo(	int32 *vch,	LStrHandle Creator, LStrHandle vcName, int32 *LifeTime, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_Context_InfoList( LStrHandle Creator, _VarInfo *dout, LVBoolean *err_f, uInt16 *err_c);

MgErr	vmCmd_Context_Close(int32 *vch,	LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_Context_Test(	int32 *vch,	LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_VarMan_Init(	LVBoolean *err_f, uInt16 *err_c);

MgErr	vmCmd_vcNames_Reset(	LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_vcNames_Assign(	int32 *vch, LStrHandle vcName, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_vcNames_GetName(	int32 *vch, LStrHandle vcName, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_vcNames_FindName(	int32 *vch, LStrHandle vcName, LVBoolean *err_f, uInt16 *err_c);

MgErr	vmCmd_MiscFieldAccess(	int32 *vch, VM_MiscAccessPars *par, LVBoolean *err_f, uInt16 *err_c);

// -----------------------------
MgErr	vmCmd_Var_Create(	int32 vch, LVBoolean *gbl, LStrHandle name, _VarInfo *par, uInt32 writerID, _VM_VI_WriteStats *wrStats, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_Var_CreateEx(	int32 vch, LVBoolean *gbl, LStrHandle name, _VarInfo *par, uInt32 writerID, _VM_VI_WriteStats *wrStats, _VM_EXT_FUNC *efunc, void *edata, LVBoolean *err_f, uInt16 *err_c);
MgErr	__varCreate_CmdX (	int32 vch, LVBoolean *gbl, LStrHandle name, _VarInfo *par, uInt32 writerID, _VM_VI_WriteStats *wrStats, _VM_EXT_FUNC *efunc, void *edata, _VarInfo **vip, LVBoolean *err_f, uInt16 *err_c);

MgErr	vmCmd_Var_GetValue(	 int32 vch, LVBoolean *gbl, LStrHandle name, _VarInfo *par, _VM_VI_WriteStats *wrStats, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_Var_GetValueEx(int32 vch, LVBoolean *gbl, LStrHandle name, _VarInfo *par, _VM_VI_WriteStats *wrStats, _VM_EXT_FUNC *efunc, void *edata, LVBoolean *err_f, uInt16 *err_c);

MgErr	vmCmd_Var_SetValue(	 int32 vch, LVBoolean *gbl, LStrHandle name, _VarInfo *value, uInt32 writerID, _VM_VI_WriteStats *wrStats, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_Var_SetValueEx(int32 vch, LVBoolean *gbl, LStrHandle name, _VarInfo *value, uInt32 writerID, _VM_VI_WriteStats *wrStats, _VM_EXT_FUNC *efunc, void *edata, LVBoolean *err_f, uInt16 *err_c);
MgErr	__vmCmd_Var_SetValue_Pnt(	_VarInfo *vip, _VarInfo	*value, uInt32 writerID, LVBoolean *err_f, uInt16 *err_c );

MgErr	vmCmd_Var_InfoList(	int32 vch, LVBoolean *gbl, LStrHandle name, _VarInfo *par, LVBoolean *err_f, uInt16 *err_c);

MgErr	vmCmd_Var_Destroy(	int32 vch, LVBoolean *gbl, LStrHandle name, LVBoolean *err_f, uInt16 *err_c);

MgErr	vmCmd_Var_GetAttribute(	int32 vch, LVBoolean *gbl, LStrHandle name, LStrHandle attr, float64 *nval, uInt32 *type, uInt32 *writerID, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_Var_SetAttribute(	int32 vch, LVBoolean *gbl, LStrHandle name, LStrHandle attr, float64 *nval, uInt32 *type, LVBoolean *err_f, uInt16 *err_c);

MgErr	vmCmd_Var_Copy( _VM_2Var_Info *vars, uInt32 writerID, LVBoolean *err_f, uInt16 *err_c );

// ##########################################################
// >>> #CE_3.1.0-r6 2026-09-17
__forceinline void  __UpdateVarWritingTime(_VarRecord *vp, uInt32 millisec, float64 timestamp)
{
	vp->writeCnt += 1;
	vp->writeMS  = millisec;
	vp->writeTS  = timestamp;
}
__forceinline void  __UpdateVarWritingTimeNow(_VarRecord *vp)
{
//	return;
	static Bool32	ok = FALSE;
	static uInt32	ms = 0;
	static float64	ts = 0.0;

	vp->writeCnt += 1;
	vp->writeMS  = MillisecsTimer();
	if ((vp->writeMS == ms) && ok) {
		vp->writeTS = ts;
		return;
	}
	ok = TRUE;
	ms = vp->writeMS;
	ts = vp->writeTS = __GetTime_Labview_DBL();
}
__forceinline void  __ReadVarWritingStats(const _VarRecord *vp, _VM_VI_WriteStats *wrStats)
{
	wrStats->writerID	= vp->writerID;
	wrStats->writeMS	= vp->writeMS;
	wrStats->writeTS	= vp->writeTS;
	wrStats->writeCnt	= vp->writeCnt;
}
// <<< #CE_3.1.0-r6 2026-09-17

// ##########################################################

// >>> #CE_3.0.7 2026-07-23
__forceinline	_VarRecord * __fastcall	__VarInfoPnt_To_VarRecordPnt(_VarInfo *vip)
{
	return (_VarRecord*)((char*)vip - offsetof(_VarRecord, inf));
}
// <<< #CE_3.0.7 2026-07-23

_VarRecord * __fastcall __GetVarPointer( int32 vch, LVBoolean *glob, LStrHandle name, int32 *pVCIndex, uInt16 *err_c);
/*
__forceinline _VarInfo * __fastcall GetVarPointer( int32 *vch, LVBoolean *glob, LStrHandle name, uInt16 *err_c)
{
	_VarRecord	*vp;
	if (vp = __GetVarPointer(vch, glob, name, NULL, err_c))
		return &(vp->inf);
	return NULL;	// err_f и err_c уже установлены
}
*/
// -----------------------------------
__forceinline _VarInfo * __fastcall __vmCmd_VarPointer( int32 vch, LVBoolean *glob, LStrHandle name, uInt16 *err_c)
{
	_VarRecord	*vp;
//DbgPrintf(__FUNCTION__"()");
	if ( vp = __GetVarPointer(vch, glob, name, NULL, err_c) )
		return &(vp->inf);
	return NULL;	// err_f и err_c уже установлены
}
// -----------------------------------
__forceinline MgErr __fastcall Var_Copy( _VarInfo *dst, _VarInfo *src)
{
	dst->num = src->num;
	switch( dst->type = src->type) {
		case VM_TYPE_DBL :
		case VM_TYPE_IDXRANGE :	return noErr;
	}
	if ((dst->str) && (src->str)) {	// do not copy a string if either dst->str == NULL or src->str == NULL
//DbgPrintf(__FUNCTION__"(): copy str");
//		return  LStr_Copy(dst->str, src->str);
		return ce_CopyOrCreate_OperandString(&(dst->str), src->str, dst->type);		// >>> #CE_3.0.4 2026-07-06
	}
	return noErr;
}


// >>> #CE_3.0.2 2026-06-19
// this is used to signal CE (and CSE) that something (= a variable) was deleted in VarMan
// so, CE should renew pointers to variables it uses.
extern	uInt32	_DelVarCnt;
__forceinline	uInt32	VM_HardChangeCounter()	{ return _DelVarCnt; }
// <<< #CE_3.0.2 2026-06-19

// >>> #CE_3.0.2 2026-06-08 : deprecated
//void	__vmCmd_Lock		(void);
//void	__vmCmd_Unlock		(void);
//void	__vmCmd_LockWFlag	(int32 *flag);
//void	__vmCmd_UnlockWFlag	(int32 *flag);
//void	__vmCmd_LockWCnt	(int32 *counter);
//void	__vmCmd_UnlockWCnt	(int32 *counter);
// <<< #CE_3.0.2 2026-06-08
// -----------------------------

MgErr	vmCmd_Alias_Operation(	int32 *vch, LVBoolean *gbl, LStrHandle name, _VarInfo *par, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_Alias_InfoList(	_VarInfo *dout, LVBoolean *err_f, uInt16 *err_c);


MgErr	vmCmd_History_Create(	int32 vch, LStrHandle name, _VHistParams *histPars, Bool32 enaWriterIDs, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_History_Destroy(	int32 vch, LStrHandle name, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_History_Reset(	int32 vch, LStrHandle name, LVBoolean *err_f, uInt16 *err_c);
//MgErr	vmCmd_History_Save(		int32 vch, LStrHandle name, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_History_Config(	int32 vch, LStrHandle name, _VHistParams *hpars, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_History_Access(	int32 vch, LStrHandle name, LVBoolean *outHasWrID, VHistAcc_func extFunc, void *extData, LVBoolean *err_f, uInt16 *err_c);
MgErr	vmCmd_History_Preset(	int32 vch, LStrHandle name, uInt32 type, _hLVA1Dbl time, _hLVA1Poly data, _hLVA1U32 wrid, LVBoolean append, LVBoolean *err_f, uInt16 *err_c);

MgErr	_VHistory_Create(	_VarRecord *vp, _VHistParams *pars, Bool32 enaWriterIDs, LVBoolean *err_f, uInt16 *err_c);
MgErr	_VHistory_Destroy(	_VarRecord *vp);
MgErr	_VHistory_Reset(	_VarRecord *vp);
MgErr	_VHistory_Config(	_VarRecord *vp, _VHistParams *hpars);
MgErr	_VHistory_AddCurValue(_VarRecord *vp);

#ifdef  __cplusplus
}
#endif

#endif
