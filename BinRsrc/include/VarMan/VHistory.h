/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__VAR_HISTORY_H__
#define	__VAR_HISTORY_H__

#include <extcode.h>

//#include <VarMan.h>
//#include <VMEngine.h>

// --------------------------------------
// Defines an element of variable history
// --------------------------------------
typedef struct {
	float64		time;
	union {
		float64		dbl;
		LStrHandle	hdl;
	};
} _VHistData, *_pVHistData;
STATIC_ASSERT_8_BYTES_OFFSET(_VHistData,dbl);
STATIC_ASSERT_8_BYTES_SIZE(_VHistData);


// ----------------------------
// History condition parameters
// ----------------------------
typedef struct {
	// *** Maximum History Length
	int32		length;
	// *** Time condition parameters
	uInt32		tStep;
	uInt32		tGate;
	// *** Value condition parameters
	uInt32		vMethod;
	float64		vDelta;
	float64		vRangeMin;
	float64		vRangeMax;
} _VHistParams;
STATIC_ASSERT_8_BYTES_OFFSET(_VHistParams,vDelta);
STATIC_ASSERT_8_BYTES_SIZE(_VHistParams);

enum {
	kHistVCM_nop = 0,
	kHistVCM_Changed,
	kHistVCM_AbsDev,
	kHistVCM_RelDev,
	kHistVCM_RangeOut,
	kHistVCM_RangeIn
};

// =============================================
// General structure describing variable history
// =============================================
typedef struct {
	// *** Constant parameters
	uInt32		vType;			// Type of variable
	uInt32		bufSize;		// lenght of history buffer
	_VHistData	*buf;			// pointer to buffer
//#if !defined(_M_X64)
//	int32		__pad_1__;		// to allign the next memeber to 8-bytes boundary in 32-bits targets
//#endif
	uInt32		*writerIDs;		// optional array of writer IDs [#CE_3.1.0 2026-08-20]

	// *** Time condition parameters
	uInt32		tStep;
	uInt32		tGate;

	// *** Value condition parameters
	uInt32		vMethod;
	int32		__pad_2__;		// to allign the next memeber to 8-bytes boundary
	float64		vDelta;
	float64		vRangeMin;
	float64		vRangeMax;

	// *** Operative parameters
	int32		lastIdx;		// index of the last stored value
	uInt32		elNum;			// actual number of values stored in buffer
	uInt32		updateCnt;		// incremental history update counter
	uInt32		lastUpdateMS;	// millisecs of the last stored value
	float64		lastVal;		// the last stored value
	float64		lastUpdateTime;	// time in seconds of the last stored value

	// *** NewVals mode parameters
	uInt32		newVals;		// number of values added to history after the last read
	uInt32		lastReadMS;		// timestamp, in millisecs, of the last read value
	float64		lastReadTime;	// timestamp, in seconds, of the last read value
} _VHistory, *_pVHistory, **_hVHistory;
STATIC_ASSERT_8_BYTES_OFFSET(_VHistory,vDelta);
STATIC_ASSERT_8_BYTES_OFFSET(_VHistory,lastVal);
STATIC_ASSERT_8_BYTES_OFFSET(_VHistory,lastUpdateTime);
STATIC_ASSERT_8_BYTES_OFFSET(_VHistory,lastReadTime);
STATIC_ASSERT_8_BYTES_SIZE(_VHistory);


// Prototype of a function passed to VarMan for access a variable history
// ---------
typedef	MgErr (*VHistAcc_func)(_pVHistory hist, void *extdata, LVBoolean *err_f, uInt16 *err_c);


#endif
