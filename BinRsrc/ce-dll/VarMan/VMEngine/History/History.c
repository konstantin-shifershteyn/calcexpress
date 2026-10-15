/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <math.h>
#include <float.h>
#include <VHistory.h>

MgErr	__hist_save( _VHistory *hist, _VarInfo *vip, Bool32 replace, uInt32 tick, float64 time, uInt32 writerID);

// =======================================================
//	Create History
// =======================================================
MgErr _VHistory_Create(	_VarRecord		*vp,
						_VHistParams	*pars,
						Bool32			enaWriterIDs,
						LVBoolean		*err_f,
						uInt16			*err_c )
{
	_pVHistory	hist = NULL;
	int32		BufSizeInBytes;
	MgErr		err = noErr;

	if (*err_f)
		goto out;

	if (vp->hist) {
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_ATTACHED;
		goto out;
	}
	if (pars->length <= 0) {
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_INVPARAMS;
		goto out;
	}
	BufSizeInBytes = pars->length * sizeof(_VHistData);
	if (BufSizeInBytes <= 0) {			// test for overflow
		*err_f = LVTRUE;
		*err_c = VM_ERR_HIST_INVPARAMS;
		goto out;
	}

	// *** Create history descriptor
	vp->hist = hist = (void*)ce_DSNewPClr(sizeof(_VHistory));
	if (!hist) {
		err = mFullErr;
		goto out;
	}
//DbgPrintf("VHistory data %s\n", (size_t)(vep->hist) % sizeof(float64) == 0 ? "aligned" : "not aligned");

	// *** Fill in history descriptor
	hist->vType			= vp->inf.type;
	hist->buf			= NULL;		// temporal
	hist->bufSize		= pars->length;
	// ---
	hist->tStep			= pars->tStep;
	hist->tGate			= pars->tGate;
	// ---
	hist->vMethod		= pars->vMethod;
	hist->vDelta		= fabs(pars->vDelta);
	hist->vRangeMin		= pars->vRangeMin;
	hist->vRangeMax		= pars->vRangeMax;
	// ---
	hist->elNum			= 0;	// number of elements in buffer
	hist->updateCnt		= 0;	// history update counter
	hist->lastIdx		= 0;
	hist->lastVal		= 0.0;
	hist->lastUpdateTime = 0.0;
	hist->lastUpdateMS	= 0;
	// ---
	hist->newVals		= 0;
	hist->lastReadTime	= 0.0;
	hist->lastReadMS	= 0;

	// *** Create history buffer and fill in it with zeroes
	hist->buf = ce_DSNewPClr(BufSizeInBytes);
	if (!(hist->buf)) {
		err = mFullErr;
		_VHistory_Destroy(vp);
		goto out;
	}
// >>> #CE_3.1.0 2026-08-20
	// *** Create buffer for writer IDs
	hist->writerIDs = NULL;
	if (enaWriterIDs) {
		hist->writerIDs = ce_DSNewPClr(pars->length * sizeof(*(hist->writerIDs)));
		if (!(hist->writerIDs)) {
			err = mFullErr;
			_VHistory_Destroy(vp);
			goto out;
		}
	}
// <<< #CE_3.1.0 2026-08-20
out:
	return err;
}

// =======================================================
//	Destroy History
// =======================================================
MgErr _VHistory_Destroy( _VarRecord	*vp)
{
	uInt32		i;
	_pVHistory	hist;
	_pVHistData	buf;
	MgErr		err = noErr;

	// ### get pointer to history descriptor
	if (!(hist = vp->hist))
		goto out;		// no history
//	if (DSCheckPtr(hist))		// ???
//		goto out;		// invalid pointer

// >>> #CE_3.1.0 2026-08-20
	// ### dispose the writer IDs buffer if present
	if (hist->writerIDs) {
		DSDisposePtr(hist->writerIDs);
		hist->writerIDs = NULL;
	}
// <<< #CE_3.1.0 2026-08-20

	// ### get pointer to history data
	if (!(buf = hist->buf))
		goto DestroyDescriptor;

	// *** dispose the history buffer elements if they are handles
	if ((hist->vType==VM_TYPE_DBL)||(hist->vType==VM_TYPE_IDXRANGE))
		goto DestroyBuffer;

	for(i=0; i < hist->bufSize; i++) {
		if (buf[i].hdl) {
			DSDisposeHandle(buf[i].hdl);
			buf[i].hdl = NULL;
		}
	}

	// ### dispose the history buffer
DestroyBuffer:
	DSDisposePtr(buf);
	hist->buf = NULL;

	// ### dispose the history descriptor
DestroyDescriptor:
	DSDisposePtr(hist);
	vp->hist = NULL;
out:
	return err;
}

// =======================================================
//	Reset History
// =======================================================
MgErr	_VHistory_Reset( _VarRecord *vp)
{
	_pVHistory	hist;
	_pVHistData	hBuf;
	int32		i, size;

	// *** pointer to history descriptor
	if (!(hist = vp->hist))
		goto out;		// no history

	size = hist->bufSize;
	hist->lastIdx	= 0;

	// !!!!!!!!!!!!!!!!!!!!!!!!!
	hist->elNum			= hist->updateCnt	= hist->newVals	= 0;
	hist->lastUpdateMS	= hist->lastReadMS	= 0;
	hist->lastUpdateTime = hist->lastReadTime = 0.0;

	// if fixed size values
	if ((hist->vType==VM_TYPE_DBL)||(hist->vType==VM_TYPE_IDXRANGE))
		goto out;

	if (!(hBuf = hist->buf))
		goto out;		// stub

	// free allocated memory blocks if variable-size values
	for(i=0; i<size; i++) {
		if (hBuf[i].hdl) {
			DSDisposeHandle(hBuf[i].hdl);
			hBuf[i].hdl = NULL;
		}
	}
out:
	return noErr;
}

// =======================================================
//	Configure History
// =======================================================
MgErr	_VHistory_Config( _VarRecord *vp, _VHistParams *hpars )
{
	_pVHistory	hist;

	// *** pointer to history descriptor
	if (!(hist = vp->hist))
		goto out;		// no history

	hist->tStep	= hpars->tStep;
	hist->tGate	= hpars->tGate;
	// ---
	hist->vMethod	= hpars->vMethod;
	hist->vDelta	= hpars->vDelta;
	hist->vRangeMin	= hpars->vRangeMin;
	hist->vRangeMax	= hpars->vRangeMax;
out:
	return noErr;
}

// =======================================================
//	Add Value to History
// =======================================================
MgErr _VHistory_AddCurValue( _VarRecord	*vp)
{
	_pVHistory	hist;
	_pVHistData	buf;
	uInt32		tick;
	Bool32		replace;

	if (!(hist = vp->hist))		return noErr;	//no history
	if (!(buf  = hist->buf))	return noErr;	//stub
	if (hist->bufSize <= 0)		return noErr;	//stub

	replace	= FALSE;
//	tick	= MillisecsTimer();
	tick	= vp->writeMS;		// writeMS is set together with the value and writerID  [#CE_3.1.0-r6 2026-09-17]

	if (!(hist->elNum))
		goto Save;	// if empty history

	// ========================
	// Test Time Step condition
	// ========================
	if ((tick - hist->lastUpdateMS) >= hist->tStep)
		goto Save;

	// =====================
	// Test value conditions
	//======================
	switch(hist->vMethod) {

		case kHistVCM_nop :		// *** no value conditons
			break;

		case kHistVCM_Changed :		// *** 'If Changed' condition
			if ((vp->inf.type == VM_TYPE_DBL) || (vp->inf.type == VM_TYPE_IDXRANGE)) {
				if (vp->inf.num != hist->lastVal)
					if (!(_isnan(vp->inf.num) && _isnan(hist->lastVal)))	// >>> #CE_3.0.2 2026-06-10: don't save consecutive NaNs
						goto Save;
			}
			else {
				int32		idx;
				_VHistData	*pnt;

				idx = hist->lastIdx;
				pnt = &(hist->buf[idx]);
				if (!LStr_Cmp(vp->inf.str, pnt->hdl ))
					goto Save;
			}
			break;

		case kHistVCM_AbsDev :		// *** 'Absolute Deviation' condition
			if (vp->inf.type == VM_TYPE_DBL)
				if (fabs(vp->inf.num - hist->lastVal) >= hist->vDelta)
					goto Save;
			break;

		case kHistVCM_RelDev :		// *** 'Relative Deviation' condition
			if (vp->inf.type == VM_TYPE_DBL)
				if (fabs(vp->inf.num - hist->lastVal) >= fabs(hist->lastVal * hist->vDelta))
					goto Save;
			break;

		case kHistVCM_RangeOut :	// *** 'Out Of Range' condition
			if (vp->inf.type == VM_TYPE_DBL)
				if ((vp->inf.num <= hist->vRangeMin) || (vp->inf.num >= hist->vRangeMax))
					goto Save;
			break;

		case kHistVCM_RangeIn :		// *** 'Within Range' condition
			if (vp->inf.type == VM_TYPE_DBL)
				if ((vp->inf.num >= hist->vRangeMin) && (vp->inf.num <= hist->vRangeMax))
					goto Save;
			break;
	}

	// ========================
	// Test Time Gate condition
	// ========================
	if ((tick - hist->lastUpdateMS) < hist->tGate) {
		replace = TRUE;
Save:		// *** save value and exit
//		return __hist_save(vp->hist, &(vp->inf), replace, tick, __GetTime_Labview_DBL(), vp->writerID);
// >>> #CE_3.1.0-r6 2026-09-17
		// writeID, writeMS, and writeTS are set together with the value
		return __hist_save(vp->hist, &(vp->inf), replace, vp->writeMS, vp->writeTS, vp->writerID);
// <<< #CE_3.1.0-r6 2026-09-17
	}
	return noErr;		// *** don't save if no conditions are met
}

// ===================================================
MgErr __hist_save(	_VHistory	*hist,
					_VarInfo	*vip,
					Bool32		replace,
					uInt32		tick,
					float64		time,
					uInt32		writerID		// >>> #CE_3.1.0 2026-08-20
					)
{
	int32		idx;
	_VHistData	*pnt;
	MgErr		err = noErr;

	// ### Add empty element to history (virtually)
	if (replace) {
		idx = hist->lastIdx;
	}
	else {
		idx = (hist->lastIdx + 1) % hist->bufSize;
		hist->lastIdx	= idx;
		//hist->elNum		= __min( hist->elNum +1, hist->bufSize);
		if (hist->elNum < hist->bufSize)			// >>> #CE_3.0.7 2026-07-29
			hist->elNum += 1;
	}

	// ### Get pointer to history element
	pnt = &(hist->buf[idx]);

	// ### Save element value
	if ((hist->vType == VM_TYPE_DBL) || (hist->vType == VM_TYPE_IDXRANGE)) {
		pnt->dbl = vip->num;				// stored as DBL scalar
	}
	else {
//		err = LStr_CopyOrCreate( &(pnt->hdl), vip->str);	// stored as LV string
		err = ce_CopyOrCreate_OperandString( &(pnt->hdl), vip->str, hist->vType);	// stored as LV string	// >>> #CE_3.0.4 2026-07-06
	}

// >>> #CE_3.1.0 2026-08-20
	// ### Save writer ID (if it's supported)
	if (hist->writerIDs) {
		hist->writerIDs[idx] = writerID;
	}
// <<< #CE_3.1.0 2026-08-20

	// ### Update history statistics if new value has been added
	if (!replace) {
		// *** this is only actual for DBL Scalar and IndexRange types
		hist->lastVal			= vip->num;
		// *** these are actual for all the types
		hist->lastUpdateMS		= tick;
		hist->lastUpdateTime	= pnt->time = time;
		hist->newVals	+= 1;
		hist->updateCnt	+= 1;
	}
	return err;
}
