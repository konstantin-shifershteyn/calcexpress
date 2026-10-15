/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VarMan	VIs Interface
// -----------------------------------

#include <extcode.h>
#include <VarMan.h>
#include <VarMan_CIN.h>
#include <VHistory.h>
#include <VMEngine_Routines.h>

#include <lvsup\lvsup.h>

#define	CMD_NOP				0
#define	CMD_CREATE			1
#define	CMD_DESTROY			2
#define	CMD_RESET			3
#define	CMD_GETINFO			4
#define	CMD_RECONFIG		5
#define	CMD_CREATE_WRID		6	// >>> #CE_3.1.0 2026-08-20

typedef	MgErr	(*VMExtFunc)();

// -----------------------------------------------------
// Cluster in which CIN returns information on a history
// -----------------------------------------------------
typedef struct {
	uInt32		varType;
	uInt32		histLength;		// history length
	//
	uInt32		elNum;			// stored values
	uInt32		updateCnt;		// update counter
	//
	uInt32		lastUpdateMS;
#if defined(_M_X64)
	int32		pad_1;			// >>> #CE_3.0 2024-12-13
#endif
	float64		newestTimestamp;
	float64		oldestTimestamp;

	// *** 'NewVals mode' cluster
	struct {
		uInt32		newVals;	// number of values added to history after the last read
#if defined(_M_X64)
		int32		pad_1;		// >>> #CE_3.0 2024-12-13
#endif
		float64		lastReadTime;	// timestamp, in seconds, of the last read value
		uInt32		lastReadMS;		// timestamp, in millisecs, of the last read value
#if defined(_M_X64)
		int32		pad_2;			// >>> #CE_3.0 2024-12-13
#endif
	} NewValsMod;

	// *** 'time conditions' cluster
	struct {
		uInt32	tStep;
		uInt32	tGate;
	} tCond;
	// *** 'value conditions' cluster
	struct {
		uInt32	vMethod;
#if defined(_M_X64)
		int32		pad_1;			// >>> #CE_3.0 2024-12-13
#endif
		float64	vDelta;
		float64	vRangeMin;
		float64	vRangeMax;
	} vCond;
} _VHistInfo;

const  _VHistInfo	CleanHistInfo = {
// >>> #CE_3.0 2024-12-13
#if defined(_M_X64)
				0, 0, 0, 0	,
				0, 0, 0.0, 0.0	,
				{ 0, 0, 0.0, 0, 0 }	,
				{ 0, 0 }	,
				{ 0, 0, 0.0, 0.0, 0.0 }
#else
				0, 0, 0, 0	,
				0, 0.0, 0.0	,
				{ 0, 0.0, 0 }	,
				{ 0, 0 }	,
				{ 0, 0.0, 0.0, 0.0 }
#endif
};

// ------------------------------------
// VarMan extension function prototype
// body is at the end of file
// ------------------------------------
MgErr extfunc_getinfo_history(	_VHistory *hist, _VHistInfo*info,    LVBoolean *err_f, uInt16 *err_c);



// **********************************************************************
// ***************  Variable History Manager VI  ************************
// **********************************************************************
// >>> #CE_3.1.0 2026-08-23 : the function updated
MgErr  __cdecl	viHistCtrl(
			uInt32			action,
			int32			vch,
			LStrHandle		name,
			_VHistParams	*histPars,
			_VHistInfo		*histInfo,
			LVBoolean		*hasWrID_out,	// output-only to get status [#CE_3.1.0 2026-08-23]
			//LVBoolean		*err_f,
			uInt16			*err_c	)
{
	LVBoolean	glob;
	LVBoolean	errf = LVFALSE;
	MgErr		err = noErr;

//	if (*err_f) {
//		*err_c = VM_ERR_INPUTERROR;
//		return noErr;
//	}

	glob	= (vch == -1);

	switch(action) {
		default:
			errf	= LVTRUE;
			*err_c	= VM_ERR_INVALID_CMD;
		case CMD_NOP:
			*histInfo = CleanHistInfo;	// clear output
			return noErr;

		case CMD_CREATE :
			err = vmCmd_History_Create(vch, name, histPars, FALSE, &errf, err_c);
			break;
		case CMD_CREATE_WRID :
			err = vmCmd_History_Create(vch, name, histPars, TRUE, &errf, err_c);
			break;

		case CMD_DESTROY :
			err = vmCmd_History_Destroy(vch, name, &errf, err_c);
			*histInfo = CleanHistInfo;	// clear output
			goto out;

		case CMD_RESET :
			err = vmCmd_History_Reset(vch, name, &errf, err_c);
			break;

		case CMD_RECONFIG :
			err = vmCmd_History_Config(vch, name, histPars, &errf, err_c);
			break;

		case CMD_GETINFO :
			break;
	}

	if (err || errf)
		goto out;

	// *** get history information
	err = vmCmd_History_Access(vch, name, hasWrID_out, extfunc_getinfo_history, histInfo, &errf, err_c);

out:
	if (err || errf) {
		*histInfo = CleanHistInfo;	// clear output if an error occurred
		if (err)
			vm_LVError(err, &errf, err_c);
	}
	return err;
}



// **********************************************************************
// **********  History Get Info: VarMan Extension Function  *************
// **********************************************************************
MgErr	extfunc_getinfo_history(
			register	_VHistory	*hist,
			register	_VHistInfo	*hinfo,
						LVBoolean	*err_f,
						uInt16		*err_c)
{
	int32	idx;

	if (*err_f)
		return noErr;

	if (!hist) {	//no history
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_NOHISTORY;
		return noErr;
	}

	// ---	History configuration
	hinfo->varType		= hist->vType;
	hinfo->histLength	= hist->bufSize;

	// ---	History statistics
	hinfo->elNum		= hist->elNum;
	hinfo->updateCnt	= hist->updateCnt;
	hinfo->lastUpdateMS	= hist->lastUpdateMS;
	hinfo->newestTimestamp	= hist->lastUpdateTime;
	// *** VarMan doesn't directly maintain the oldest time stamps
	if (hist->elNum && hist->buf) {		// if (hist->buf) is stub
		if ( (idx = hist->lastIdx - hist->elNum + 1) < 0)
			idx += hist->bufSize;
		hinfo->oldestTimestamp	= hist->buf[idx].time;
	} else	hinfo->oldestTimestamp	= 0;

	// ---	NewVals Mode statistics
	hinfo->NewValsMod.newVals	= hist->newVals;
	hinfo->NewValsMod.lastReadMS	= hist->lastReadMS;
	hinfo->NewValsMod.lastReadTime	= hist->lastReadTime;

	// ---	Time Conditions params
	hinfo->tCond.tStep		= hist->tStep;
	hinfo->tCond.tGate		= hist->tGate;

	// ---	Value Condition params
	hinfo->vCond.vMethod	= hist->vMethod;
	hinfo->vCond.vDelta		= hist->vDelta;
	hinfo->vCond.vRangeMin	= hist->vRangeMin;
	hinfo->vCond.vRangeMax	= hist->vRangeMax;

	return noErr;
}
