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
#include <VMEngine.h>
#include <VMEngine_Routines.h>

#include <celib\ceMemLib.h>
#include <celib\FlattenData.h>


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@  DBL SCALARS  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//	READ multiple variables of the DBL Scalar datatype
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
MgErr __cdecl viReadMultiVars_DBL(	int32				vch,
									_hLVA1Str			varNames,
									int32				lock_cycle,
									uInt16				*errCodes,
									//uInt32		*writerIDs,
									_VM_VI_WriteStats	*wrStats,
									float64				*dblVals)
{
	int32		i;
	int32		arrSize = (*varNames)->size;
	_VarRecord	*vp;
	LVBoolean	glob;
	uInt16		errc;
	Bool32		locked = FALSE;
//	MgErr		err = noErr;

	VARMAN_LOCK_SHARED();
	locked = TRUE;

	for (i=0; i < arrSize; i++) {
		// === read
		glob = (vch == -1 ? LVTRUE : LVFALSE);
		errc = 0;
		vp	= __GetVarPointer(vch, &glob, (*varNames)->val[i], NULL, &errc);
		if (vp) {
			if (vp->inf.type == VM_TYPE_DBL) {
				dblVals[i]	 = vp->inf.num;
				__ReadVarWritingStats(vp, &(wrStats[i]));	// >>> #CE_3.1.0-r6 2026-09-18
			}
			else
				errc = VM_ERR_VAR_INVTYPE;
		}
		errCodes[i] = errc;

		// === re-locking to give a chance for other threads
		if ((lock_cycle > 0) && (i % lock_cycle == 0)) {
			VARMAN_UNLOCK_SHARED();
			locked = FALSE;
			VARMAN_LOCK_SHARED();
			locked = TRUE;
		}
	}
	if (locked) {
		VARMAN_UNLOCK_SHARED();
		locked = FALSE;
	}	
	return 	noErr;
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//	WRITE multiple variables of the DBL Scalar datatype
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
MgErr __cdecl viWriteMultiVars_DBL(	int32				vch,
									_hLVA1Str			varNames,
									int32				lock_cycle,
									uInt16				*errCodes,
									uInt32				*writerIDs,
									_VM_VI_WriteStats	*wrStats,
									float64				*dblVals)
{
	int32		i;
	int32		arrSize = (*varNames)->size;
	_VarRecord	*vp;
	LVBoolean	glob;
	uInt16		errc;
	Bool32		locked = FALSE;
	MgErr		err = noErr;
	uInt32		ms;			// >>> #CE_3.1.0-r6 2026-09-17
	float64		ts;			// >>> #CE_3.1.0-r6 2026-09-17

	VARMAN_LOCK_EXCLUSIVE();
	locked = TRUE;

	ms = MillisecsTimer();
	ts = __GetTime_Labview_DBL();
	//do { ms = MillisecsTimer(); ts = __GetTime_Labview_DBL(); } while (ms != MillisecsTimer());

	for (i=0; i < arrSize; i++) {
		glob = (vch == -1 ? LVTRUE : LVFALSE);
		errc = 0;
		vp	= __GetVarPointer(vch, &glob, (*varNames)->val[i], NULL, &errc);
		if (vp) {
			if (vp->inf.type == VM_TYPE_DBL) {
				// ### write value to the variable
				vp->inf.num	 = dblVals[i];
				// ### complete writing to the variable
				vp->writerID = writerIDs[i];
				__UpdateVarWritingTime(vp, ms, ts);			// >>> #CE_3.1.0-r6 2026-09-17
				if (vp->hist) {
					err = _VHistory_AddCurValue(vp);	// ### add new value to the history
					if (err) {
						errCodes[i] = VM_ERR_MEMORY_FULL;
						goto out;
					}
				}
				__ReadVarWritingStats(vp, &(wrStats[i]));	// >>> #CE_3.1.0-r6 2026-09-18
			}
			else
				errc = VM_ERR_VAR_INVTYPE;
		}
		errCodes[i] = errc;

		// === re-locking to give a chance for other threads
		if ((lock_cycle > 0) && (i % lock_cycle == 0)) {
			VARMAN_UNLOCK_EXCLUSIVE();
			locked = FALSE;
			VARMAN_LOCK_EXCLUSIVE();
			locked = TRUE;
			ms = MillisecsTimer();
			ts = __GetTime_Labview_DBL();
			//do { ms = MillisecsTimer(); ts = __GetTime_Labview_DBL(); } while (ms != MillisecsTimer());
		}
	}
out:
	if (locked) {
		VARMAN_UNLOCK_EXCLUSIVE();
		locked = FALSE;
	}	
	return 	err;
}


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  STRINGS  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//	READ multiple variables of the STRING datatype
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
MgErr __cdecl viReadMultiVars_STR(	int32				vch,
									_hLVA1Str			varNames,
									int32				lock_cycle,
									uInt16				*errCodes,
									//uInt32		*writerIDs,
									_VM_VI_WriteStats	*wrStats,
									_hLVA1Str strVals)
{
	int32		i;
	int32		arrSize = (*varNames)->size;
	_VarRecord	*vp;
	LVBoolean	glob;
	uInt16		errc;
	Bool32		locked = FALSE;
	MgErr		err = noErr;

	VARMAN_LOCK_SHARED();
	locked = TRUE;

	for (i=0; i < arrSize; i++) {
		// === read
		glob = (vch == -1 ? LVTRUE : LVFALSE);
		errc = 0;
		vp	= __GetVarPointer(vch, &glob, (*varNames)->val[i], NULL, &errc);
		if (vp) {
			if (vp->inf.type == VM_TYPE_STR) {
				err = LStr_Copy((*strVals)->val[i], vp->inf.str);
				__ReadVarWritingStats(vp, &(wrStats[i]));	// >>> #CE_3.1.0-r6 2026-09-18
			}
			else
				errc = VM_ERR_VAR_INVTYPE;
		}
		errCodes[i] = errc;
		if (err) {
			errCodes[i] = VM_ERR_MEMORY_FULL;
			goto out;
		}
		// === re-locking to give a chance for other threads
		if ((lock_cycle > 0) && (i % lock_cycle == 0)) {
			VARMAN_UNLOCK_SHARED();
			locked = FALSE;
			VARMAN_LOCK_SHARED();
			locked = TRUE;
		}
	}
out:
	if (locked) {
		VARMAN_UNLOCK_SHARED();
		locked = FALSE;
	}	
	return 	err;
}


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//	WRITE multiple variables of the STRING datatype
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
MgErr __cdecl viWriteMultiVars_STR(	int32				vch,
									_hLVA1Str			varNames,
									int32				lock_cycle,
									uInt16				*errCodes,
									uInt32				*writerIDs,
									_VM_VI_WriteStats	*wrStats,
									_hLVA1Str			strVals)
{
	int32		i;
	int32		arrSize = (*varNames)->size;
	_VarRecord	*vp;
	LVBoolean	glob;
	uInt16		errc;
	Bool32		locked = FALSE;
	MgErr		err = noErr;
	uInt32		ms;			// >>> #CE_3.1.0-r6 2026-09-17
	float64		ts;			// >>> #CE_3.1.0-r6 2026-09-17

	VARMAN_LOCK_EXCLUSIVE();
	locked = TRUE;
	ms = MillisecsTimer();
	ts = __GetTime_Labview_DBL();
	//do { ms = MillisecsTimer(); ts = __GetTime_Labview_DBL(); } while (ms != MillisecsTimer());

	for (i=0; i < arrSize; i++) {

		//glob = LVFALSE;
		glob = (vch == -1 ? LVTRUE : LVFALSE);	// >>> #CE_3.1.0 2026-08-22
		errc = 0;
		vp	= __GetVarPointer(vch, &glob, (*varNames)->val[i], NULL, &errc);
		if (vp) {
			if (vp->inf.type == VM_TYPE_STR) {
				// ### write value to the variable
				if (!(err = LStr_Copy(vp->inf.str, (*strVals)->val[i]))) {
					// ### complete writing to the variable
					vp->writerID = writerIDs[i];
					__UpdateVarWritingTime(vp, ms, ts);			// >>> #CE_3.1.0-r6 2026-09-17
					if (vp->hist) {
						err = _VHistory_AddCurValue(vp);	// ### add new value to the history
					}
					__ReadVarWritingStats(vp, &(wrStats[i]));	// >>> #CE_3.1.0-r6 2026-09-18
				}
			}
			else
				errc = VM_ERR_VAR_INVTYPE;
		}
		errCodes[i] = errc;
		if (err) {
			errCodes[i] = VM_ERR_MEMORY_FULL;
			goto out;
		}
		// === re-locking to give a chance for other threads
		if ((lock_cycle > 0) && (i % lock_cycle == 0)) {
			VARMAN_UNLOCK_EXCLUSIVE();
			locked = FALSE;
			VARMAN_LOCK_EXCLUSIVE();
			locked = TRUE;
			ms = MillisecsTimer();
			ts = __GetTime_Labview_DBL();
			//do { ms = MillisecsTimer(); ts = __GetTime_Labview_DBL(); } while (ms != MillisecsTimer());
		}
	}
out:
	if (locked) {
		VARMAN_UNLOCK_EXCLUSIVE();
		locked = FALSE;
	}	
	return 	err;
}


// @@@@@@@@@@@@@@@@@@@@@@@  ARBITRARY DATA TYPES  @@@@@@@@@@@@@@@@@@@@@@@@@@@


// -------------------------------------------------------
// Typedef describes an array of values passed from/to VIs
// -------------------------------------------------------
typedef struct {
	int32		dimSize;
#if defined(_M_X64)
	int32		pad_1;
#endif
	_VarInfo	data[1];
} _VarValueArray, ** _VarValueArrayHandle;


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//	READ multiple variables of arbitrary datatype
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
MgErr __cdecl viReadMultiVars_ANY(	int32					vch,
									_hLVA1Str				varNames,
									int32					lock_cycle,
									uInt16					*errCodes,
									//uInt32		*writerIDs,
									_VM_VI_WriteStats		*wrStats,
									_VarValueArrayHandle	viah )
{
	int32		i;
	int32		arrSize = (*varNames)->size;
	_VarInfo	*vip;
	_VarRecord	*vp;
	LVBoolean	glob;
	LVBoolean	errf;
	uInt16		errc;
	Bool32		locked = FALSE;
	MgErr		err = noErr;

	VARMAN_LOCK_SHARED();
	locked = TRUE;

	for (i=0; i < arrSize; i++) {

		vip = &((*viah)->data[i]);

		// === read
		glob = (vch == -1 ? LVTRUE : LVFALSE);
		errc = 0;
		vp	= __GetVarPointer(vch, &glob, (*varNames)->val[i], NULL, &errc);
		if (vp != NULL) {
			switch (vip->type = vp->inf.type) {
				case VM_TYPE_DBL :
				case VM_TYPE_IDXRANGE :
					vip->num = vp->inf.num;
					break;
				default:
				case VM_TYPE_STR:
					err = LStr_Copy(vip->str, vp->inf.str);
					break;
				case VM_TYPE_A1DBL :
				case VM_TYPE_A2DBL :
				case VM_TYPE_CDB :
				case VM_TYPE_A1CDB :
				case VM_TYPE_A2CDB :
				case VM_TYPE_A1STR :
				case VM_TYPE_A2STR :
					if (!(err = LStr_Copy(vip->str, vp->inf.str))) {
						errf = LVFALSE;
						_flat_data(vip->type, vip->str, &errf);		//flats only predefined types
						if (errf) {
							errc = VM_ERR_BADDATA;
						}
					}
					break;
			}
			__ReadVarWritingStats(vp, &(wrStats[i]));	// >>> #CE_3.1.0-r6 2026-09-18
		}
		errCodes[i] = errc;
		if (err) {
			errCodes[i] = VM_ERR_MEMORY_FULL;
			goto out;
		}
		// === re-locking to give a chance for other threads
		if ((lock_cycle > 0) && (i % lock_cycle == 0)) {
			VARMAN_UNLOCK_SHARED();
			locked = FALSE;
			VARMAN_LOCK_SHARED();
			locked = TRUE;
		}
	}
out:
	if (locked) {
		VARMAN_UNLOCK_SHARED();
		locked = FALSE;
	}	
	return 	err;
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//	WRITE multiple variables of arbitrary datatype
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
MgErr __cdecl viWriteMultiVars_ANY(	int32					vch, 
									_hLVA1Str				varNames,
									int32					lock_cycle,
									uInt16					*errCodes,
									uInt32					*writerIDs,
									_VM_VI_WriteStats		*wrStats,
									_VarValueArrayHandle	viah )
{
	int32		i;
	int32		arrSize = (*varNames)->size;
	_VarInfo	*vip;
	_VarRecord	*vp;
	LVBoolean	glob;
	LVBoolean	errf;
	uInt16		errc;
	Bool32		locked = FALSE;
	MgErr		err = noErr;
	uInt32		ms;			// >>> #CE_3.1.0-r6 2026-09-17
	float64		ts;			// >>> #CE_3.1.0-r6 2026-09-17

	VARMAN_LOCK_SHARED();
	locked = TRUE;
	ms = MillisecsTimer();
	ts = __GetTime_Labview_DBL();
	//do { ms = MillisecsTimer(); ts = __GetTime_Labview_DBL(); } while (ms != MillisecsTimer());

	for (i=0; i < arrSize; i++) {

		vip = &((*viah)->data[i]);

		//glob = LVFALSE;
		glob = (vch == -1 ? LVTRUE : LVFALSE);	// >>> #CE_3.1.0 2026-08-22
		errc = 0;
		vp	= __GetVarPointer(vch, &glob, (*varNames)->val[i], NULL, &errc);
		if (vp != NULL) {
			// ### write value to the variable
			switch (vip->type = vp->inf.type) {
				case VM_TYPE_DBL :
				case VM_TYPE_IDXRANGE :
					vp->inf.num = vip->num;
					break;
				default:
				case VM_TYPE_STR:
					err = LStr_Copy(vp->inf.str, vip->str);
					break;
				case VM_TYPE_A1DBL :
				case VM_TYPE_A2DBL :
				case VM_TYPE_CDB :
				case VM_TYPE_A1CDB :
				case VM_TYPE_A2CDB :
				case VM_TYPE_A1STR :
				case VM_TYPE_A2STR :
					errf = LVFALSE;
					_unflat_data(vip->type, vip->str, &errf);		// unflat juct in source
					if (errf) {
						errc = VM_ERR_BADDATA;
					}
					else {
						err = LStr_Copy(vp->inf.str, vip->str);
					}
					break;
			}
			if (!err) {
				// ### complete writing to the variable
				vp->writerID = writerIDs[i];
				__UpdateVarWritingTime(vp, ms, ts);			// >>> #CE_3.1.0-r6 2026-09-17
				if (vp->hist) {
					err = _VHistory_AddCurValue(vp);
				}
				__ReadVarWritingStats(vp, &(wrStats[i]));	// >>> #CE_3.1.0-r6 2026-09-18
			}
		}
		errCodes[i] = errc;
		if (err) {
			errCodes[i] = VM_ERR_MEMORY_FULL;
			goto out;
		}
		// === re-locking to give a chance for other threads
		if ((lock_cycle > 0) && (i % lock_cycle == 0)) {
			VARMAN_UNLOCK_SHARED();
			locked = FALSE;
			VARMAN_LOCK_SHARED();
			locked = TRUE;
			ms = MillisecsTimer();
			ts = __GetTime_Labview_DBL();
			//do { ms = MillisecsTimer(); ts = __GetTime_Labview_DBL(); } while (ms != MillisecsTimer());
		}
	}
out:
	if (locked) {
		VARMAN_UNLOCK_SHARED();
		locked = FALSE;
	}	
	return 	err;
}

