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

#include <celib\ceMemLib.h>
#include <celib\FlattenData.h>
#include <lvsup\lvsup.h>


// *** Time range
typedef struct {
	float64		t1;
	float64		t2;
} _TimeRange;


// *** Modes to make history
enum {
	kModeTime = 0,		// get values within a time range
	kModeNVals,		// get last N values
	kModeNewVals		// get New Values
};

// --------------------------------------
// Структура, через которую параметры CIN
// передаются в функцию расширения VarMan
// --------------------------------------
typedef struct {
	uInt32		type;		// expected type of variable
#if defined(_M_X64)
	int32		pad_1;		// >>> #CE_3.0 2024-12-13 : padding bytes
#endif
	_hLVA1Dbl	time;		// array of time stamps
	_hLVA1Poly	data;		// array of history values
	_hLVA1U32	wrid;		// array of writer IDs	[#CE_3.1.0 2026-08-20]
	//
	uInt32		mode;		// Mode of getting history
	uInt32		NVals;		// number of requested values	(MODE: kModeNVals)
	_TimeRange	tRange;		// requested time range		(MODE: kModeTime)
	Bool32		reverse;	// reverse(T)/direct(F) order a history should be arranged
} extFuncPars;


typedef	MgErr	(*VMExtFunc)();

// ---------------------------------------------------------------------------------------
// Прототип функции расширения VarMan, Именно эта функция формирует History на выходе CIN
// указатель на эту функцию передается в VarMan, который вызывает ее из своего ядра
// Тело функции находится после CIN Entry Points
// ---------------------------------------------------------------------------------------
MgErr	extfunc_get_history(_VHistory *hist, extFuncPars *oPars, LVBoolean *err_f, uInt16 *err_c);

#include "HistoryGet_Subr.c"


// **********************************************************************************
// ***********************  Acquire Variable History VI  ****************************
// **********************************************************************************

MgErr __cdecl	viHistGet(
			int32		vch,
			LStrHandle	name,
			uInt32		type,
			uInt32		mode,
			int32		nvals,
			_TimeRange	*tRange,
			LVBoolean	*reverseOrder,
			_hLVA1Dbl	time,
			_hLVA1Poly	data,
			_hLVA1U32	wrid,	// >>> #CE_3.1.0 2026-08-20
			//LVBoolean	*err_f,
			uInt16		*err_c
			)
{
	extFuncPars		outInfo;
	LVBoolean		errf = LVFALSE;
	MgErr			err;

	outInfo.type	= type;
	outInfo.time	= time;
	outInfo.data	= data;
	outInfo.wrid	= wrid;
	outInfo.mode	= mode;
	outInfo.NVals	= (uInt32)nvals;
	outInfo.tRange	= *tRange;
	outInfo.reverse	= *reverseOrder;

	if (err = vmCmd_History_Access(vch, name, NULL, extfunc_get_history, &outInfo, &errf, err_c))
		vm_LVError(err, &errf, err_c);

	return err;
}


// *********************************************************************
// ********************  VarMan Extension Function  ********************
// *********************************************************************

//#define	DecrementHistIdx(hist, idx)	if (--idx < 0) idx = hist->bufSize -1;

// ===================================
MgErr	extfunc_get_history(
				_VHistory		*hist,
				extFuncPars		*oPars,
				LVBoolean		*err_f,
				uInt16			*err_c
				)
{
	_pVHistData		hBuf;
	_hLVA1Dbl		*time;
	_hLVA1Poly		*data;
	_hLVA1U32		*wrid;				// >>> #CE_3.1.0 2026-08-20
	int32			idx, idx0;
	uInt32			ui, histSize, size;
	int32			lvType;
	float64			tMin, tMax;

	float64			*tBuf	= NULL;
	void			*dBuf	= NULL;
	uInt32			*idBuf	= NULL;				// >>> #CE_3.1.0 2026-08-20

	enum {
			kTCatDblScalar,
			kTCatCdbScalar,
			kTCatNumArray,
			kTCatString,
			kTCatDblFlat,
			kTCatAnyFlat
	} typeCat;

	MgErr		err = noErr;

//------
	if (*err_f)
		goto out;
	if (!hist) {		//no history
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_NOHISTORY;
		goto out;
	}
	if (!(hBuf = hist->buf))
		goto out;	//stub


	// ===============================================
	// ===   DETERMINE VARIABLE TYPE CONDITIONS    ===
	// ===============================================
	if (hist->vType == oPars->type) {
		// *** if a variable has an expected type ***
		switch (hist->vType) {
			case VM_TYPE_IDXRANGE :
			case VM_TYPE_DBL :   typeCat= kTCatDblScalar;	lvType = fD;	break;
			case VM_TYPE_CDB :   typeCat= kTCatCdbScalar;	lvType = cD;	break;
			case VM_TYPE_A1DBL :	
			case VM_TYPE_A2DBL :
			case VM_TYPE_A1CDB :	
			case VM_TYPE_A2CDB : typeCat= kTCatNumArray;	lvType = uL;	break;
			default:			 typeCat= kTCatString;		lvType = uL;	break;
		}
	}
	else {	// *** a variable type differs from an expected type ***
		if (oPars->type != OPTYPE_ANY_FLATTENED) {
			*err_f = LVTRUE;
			*err_c = VM_ERR_VAR_INVTYPE;
			goto out;
		}
		// *** if any type is expected in flattened-to-string format ***
		switch (hist->vType) {
			case VM_TYPE_IDXRANGE :
			case VM_TYPE_DBL :	typeCat = kTCatDblFlat;	break;
			default :			typeCat = kTCatAnyFlat;	break;
		}
		lvType = uL;
		oPars->type = hist->vType;	// !!! save an actual type of the variable
	}
// >>> #CE_3.0 2024-12-15
#if defined(_M_X64)
	// lvType is used in call of NumericArrayResize() to specify size (datatype) of Var History array element
	// Var History element is UHandle for LV Strings and Arrays, so its size depends on LV bitness (size_t datatype): uInt32 for 32-bits, uInt64 for 64-bits
	if (lvType == uL) lvType = uQ;
#endif
// <<< #CE_3.0 2024-12-15

	// ======================================
	// ===  Initialize common parameters  ===
	// ======================================
	idx	 		= hist->lastIdx;	// index within history buffer
	histSize 	= hist->elNum;		// returned history size

	// ============================================
	// ===  Determine Mode specific parameters  ===
	// ============================================
	switch(oPars->mode) {

		case kModeNewVals :	// *** MODE: Get New Values
			if (hist->newVals < histSize)
				histSize = hist->newVals;
			goto MakeHistory;

		case kModeNVals :	// *** MODE: Get last N Values
			if (oPars->NVals < histSize)
				histSize = oPars->NVals;
			goto MakeHistory;

		case kModeTime :	// *** MODE: Get history within time range
			break;		// implementation is after the switch structure

		default :		// *** STUB
			histSize = 0;
			goto MakeHistory;
	}

	// =============================================
	// ===   TEST TIME RANGE
	// ===   MODE: Get history within time range
	// =============================================
	if ((oPars->tRange.t1 == 0.0) && (oPars->tRange.t2 == 0.0))
		goto MakeHistory;

	if (oPars->tRange.t1 >= oPars->tRange.t2)
	{	tMax = oPars->tRange.t1; tMin = oPars->tRange.t2;	}
	else {	tMax = oPars->tRange.t2; tMin = oPars->tRange.t1;	}

	// *** search for elements meeting the time range
	idx0 = idx;
	size = 0;
	for(ui=0; ui < histSize; ui++) {
		if (hBuf[idx].time <= tMax) {			// the first (newest) element is found
			idx0 = idx;
			for ( ; ui < histSize; ui++) {
				if (hBuf[idx].time < tMin)	// the last (oldest) element is found
					break;
				size++;
				//DecrementHistIdx(hist, idx);
				if (--idx < 0) idx = hist->bufSize -1;
			}
			break;
		}
		//DecrementHistIdx(hist, idx);
		if (--idx < 0) idx = hist->bufSize -1;
	}
	idx	= idx0;
	histSize = size;


	// ===============================
	// ===   MAKE OUTPUT HISTORY   ===
	// ===============================
MakeHistory:

	// !!! here !!!
	time	= &(oPars->time);
	data	= &(oPars->data);
	wrid	= &(oPars->wrid);		// >>> #CE_3.1.0 2026-08-20

	// ### Allocate Timestamp Array
//	if (err = ce_NumericArrayResize(fD, 1, (UHandle*)time, histSize) )		goto	MemErr;		//return err;
// >>> #CE_3.0 2024-12-13
	if (err = ce_NumericArraySetSize(fD, 1, (UHandle*)time, histSize) )
		goto	MemErr;
	(**time)->size = histSize;
	tBuf = &((**time)->val[0]);

	// ### Allocate Historic Values Array
//	if (err = ce_NumericArrayResize(lvType, 1, (UHandle*)data, histSize) )	goto	MemErr;		//return err;
// >>> #CE_3.0 2024-12-13
	if (err = ce_NumericArraySetSize(lvType, 1, (UHandle*)data, histSize) )
		goto	MemErr;
	(**data)->size = histSize;
	dBuf = &((**data)->val[0]);

// >>> #CE_3.1.0 2026-08-20
	// ### Allocate writer IDs array
	if (hist->writerIDs) {
		if (err = ce_NumericArraySetSize(uL, 1, (UHandle*)wrid, histSize))
			goto MemErr;
		(**wrid)->size = histSize;
		idBuf = &((**wrid)->val[0]);
	}
// <<< #CE_3.1.0 2026-08-20

	// ===================================
	// ===   FILL OUT HISTORY VALUES   ===
	// ===================================

	switch (typeCat) {
	case kTCatDblScalar :
		err =	_fill_dbl( tBuf, dBuf, idBuf, hist, histSize, idx, oPars->reverse);		// >>> #CE_3.1.0 2026-08-20
		break;
	case kTCatCdbScalar :
		err =	_fill_cdb( tBuf, dBuf, idBuf, hist, histSize, idx, oPars->reverse);
		break;
	case kTCatNumArray :
//		err =	_fill_arr( tBuf, dBuf, hist, histSize, idx, oPars->reverse);
		err =	_fill_arr( tBuf, dBuf, idBuf, hist, histSize, idx, oPars->reverse, oPars->type);	// #CE_3.0 2024-12-14
		break;
	case kTCatString :
	case kTCatAnyFlat :
		err =	_fill_str( tBuf, dBuf, idBuf, hist, histSize, idx, oPars->reverse, oPars->type, err_f, err_c);
		break;

	case kTCatDblFlat :
		err =	_fill_dFS( tBuf, dBuf, idBuf, hist, histSize, idx, oPars->reverse);
		break;
	}

	// ==================================================================
	// Update NewVals Mode parameters if all the conditions below are met
	// 1) NewVals mode
	// 2) returned history is not empty
	// 3) no error happened
	// ================================
	if ((oPars->mode == kModeNewVals) && histSize && !err) {
		hist->newVals		= 0;
		hist->lastReadMS	= hist->lastUpdateMS;
		hist->lastReadTime	= hist->lastUpdateTime;
	}

	if (err) {
		// ===	If history data array contains memory block handles,
		// ===	they are disposed in the respective subroute (_fill_xxx)
MemErr:
//		ce_NumericArrayResize(fD, 1, (UHandle*)time, 0);
// >>> #CE_3.0 2024-12-16
		ce_NumericArraySetSize(fD, 1, (UHandle*)time, 0);
		(**time)->size = 0;
//		ce_NumericArrayResize(lvType, 1, (UHandle*)data, 0);
// >>> #CE_3.0 2024-12-16
		ce_NumericArraySetSize(lvType, 1, (UHandle*)data, 0);
		(**data)->size = 0;
	}
out:
	return err;
}
