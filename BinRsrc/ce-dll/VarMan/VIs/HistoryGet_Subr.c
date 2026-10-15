/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ====================================================
// *** DBL Scalar: VarMan stores it in the native LV format
// ====================================================
MgErr _fill_dbl(float64 *tBuf, float64 *dBuf, uInt32 *idBuf, _VHistory *hist, int32 histSize, int32 idx, Bool32 reverse)
{
	_pVHistData	hBuf = hist->buf;
	uInt32		*writerIDs	= hist->writerIDs;		// >>> #CE_3.1.0 2026-08-20

	if (reverse) {	// *** Filling out history in reverse order
		while(histSize--) {
			*tBuf++ = hBuf[idx].time;
			*dBuf++ = hBuf[idx].dbl;
			if (writerIDs) *idBuf++ = writerIDs[idx];		// >>> #CE_3.1.0 2026-08-20
			if (--idx < 0)	idx = hist->bufSize -1;
		}
	}
	else {		// *** Filling out history in direct order
		tBuf += histSize;
		dBuf += histSize;
		idBuf+= histSize;		// >>> #CE_3.1.0 2026-08-20
		while(histSize--) {
			*(--tBuf) = hBuf[idx].time;
			*(--dBuf) = hBuf[idx].dbl;
			if (writerIDs)	*(--idBuf) = writerIDs[idx];	// >>> #CE_3.1.0 2026-08-20
			if (--idx < 0)	idx = hist->bufSize -1;
		}
	}
	return noErr;
}

// =====================================================
// *** CDB Scalar:
// VarMan stores it in native LV format within LV String
// =====================================================
MgErr _fill_cdb(float64 *tBuf, float64 *dBuf, uInt32 *idBuf, _VHistory *hist, int32 histSize, int32 idx, Bool32 reverse)
{
	_pVHistData	hBuf = hist->buf;
	uInt32		*writerIDs	= hist->writerIDs;		// >>> #CE_3.1.0 2026-08-20

	if (reverse) {	// *** Filling out history in reverse order
		while(histSize--) {
			if (writerIDs) *idBuf++ = writerIDs[idx];		// >>> #CE_3.1.0 2026-08-20
			*tBuf++	= hBuf[idx].time;
			_MoveBlock( LStrBuf(*(hBuf[idx].hdl)), dBuf, Min(2*sizeof(float64),LStrLen(*(hBuf[idx].hdl))) );
			dBuf += 2;
			if (--idx < 0)	idx = hist->bufSize -1;
		}
	}
	else {		// *** Filling out history in direct order
		tBuf += histSize;
		dBuf += histSize * 2;	// !!!
		idBuf+= histSize;		// >>> #CE_3.1.0 2026-08-20
		while(histSize--) {
			if (writerIDs)	*(--idBuf) = writerIDs[idx];	// >>> #CE_3.1.0 2026-08-20
			*(--tBuf) = hBuf[idx].time;
			dBuf -= 2;
			_MoveBlock( LStrBuf(*(hBuf[idx].hdl)), dBuf, Min(2*sizeof(float64),LStrLen(*(hBuf[idx].hdl))) );
			if (--idx < 0)	idx = hist->bufSize -1;
		}
	}
	return noErr;
}

// =====================================================
// *** 1D or 2D Numeric (DBL,CDB) Array:
// VarMan stores it in native LV format within LV String
// =====================================================
MgErr _fill_arr(	float64		*tBuf,
					UHandle		*dBuf,
					uInt32		*idBuf,
					_VHistory	*hist,
					int32		histSize,
					int32		idx,
					Bool32		reverse,
					uInt32		type	)
{
	_pVHistData	hBuf				= hist->buf;
	uInt32		*writerIDs			= hist->writerIDs;		// >>> #CE_3.1.0 2026-08-20
	UHandle		*OriginalArrayPnt	= (UHandle *)dBuf;
	int32		OriginalArraySize	= histSize;
	int32		size;
	MgErr		err = noErr;
	int32		*pInt_in, *pInt_out;
	int32		nPaddingBytes = 0;

// >>> #CE_3.0 2024-12-14
#if defined(_M_X64)
	if ((type == VM_TYPE_A1DBL) || (type == VM_TYPE_A1CDB)) nPaddingBytes = sizeof(int32);
#endif
// <<< #CE_3.0 2024-12-14

	if (reverse) {	// *** Filling out history in reverse order
		while(histSize--) {
			if (writerIDs) *idBuf++ = writerIDs[idx];		// >>> #CE_3.1.0 2026-08-20
			*tBuf++	= hBuf[idx].time;

// #CE_3.0 2024-12-14
			if (nPaddingBytes) {
				size = LStrLen(*(hBuf[idx].hdl));
				if (!(*dBuf = DSNewHandle(size + nPaddingBytes)) ) {
					err = mFullErr;	goto out;
				}
				pInt_in		= (int32*)LStrBuf(*(hBuf[idx].hdl));
				pInt_out	= (int32*)(**dBuf);
				*pInt_out++	= *pInt_in++;
				*pInt_out++ = 0;
				_MoveBlock(pInt_in, pInt_out, size - sizeof(int32));
			}
			else {
				size = LStrLen(*(hBuf[idx].hdl));
				if (!(*dBuf = DSNewHandle(size)) ) {
					err = mFullErr;	goto out;
				}
				_MoveBlock(LStrBuf(*(hBuf[idx].hdl)), **dBuf, size);	//entire array w size(s)
			}
			dBuf++;
			if (--idx < 0)	idx = hist->bufSize -1;
		}
	}
	else {		// *** Filling out history in direct order
		tBuf += histSize;
		dBuf += histSize;
		idBuf+= histSize;		// >>> #CE_3.1.0 2026-08-20
		while(histSize--) {
			if (writerIDs)	*(--idBuf) = writerIDs[idx];	// >>> #CE_3.1.0 2026-08-20
			*(--tBuf) = hBuf[idx].time;
			--dBuf;
// #CE_3.0 2024-12-14
			if (nPaddingBytes) {
				size = LStrLen(*(hBuf[idx].hdl));
				if (!(*dBuf = DSNewHandle(size + nPaddingBytes)) ) {
					err = mFullErr;	goto out;
				}
				pInt_in		= (int32*)LStrBuf(*(hBuf[idx].hdl));
				pInt_out	= (int32*)(**dBuf);
				*pInt_out++	= *pInt_in++;
				*pInt_out++ = 0;
				_MoveBlock(pInt_in, pInt_out, size - sizeof(int32));
			}
			else {
				size = LStrLen(*(hBuf[idx].hdl));
				if (!(*dBuf = DSNewHandle(size)) ) {
					err = mFullErr;	goto out;
				}
				_MoveBlock(LStrBuf(*(hBuf[idx].hdl)), **dBuf, size);	//entire array w size(s)
			}
			if (--idx < 0)	idx = hist->bufSize -1;
		}
	}
out:
	if (err) {
		ce_DisposeHandles(OriginalArrayPnt, OriginalArraySize);	// free allocated memory if an error
	}
	return err;
}

// ====================================
// Produces history as an array of strings (either strings or values flattened to strings)
// ###  DOES NOT SERVE DBL SCALARS  ###
// ====================================
// Strings:						VarMan stores it in native format (LV String)
// 1D, 2D Array of Strings:		VarMan stores them flattened to string except
//								string length that are stored in ix86 integer format
// Any numeric type except DBL:	VarMan stores them in native LV format
// User-defined data types:		VarMan stores them flattened to string
// =============================
// The function returns:
// strings:		native LV format
// other types:	flattened-to-string format
// =============================
MgErr _fill_str(	float64		*tBuf,
					LStrHandle	*dBuf,
					uInt32		*idBuf,
					_VHistory	*hist,
					int32		histSize,
					int32		idx,
					Bool32		reverse,
					uInt32		type,
					LVBoolean	*err_f,
					uInt16		*err_c	)
{
	_pVHistData		hBuf				= hist->buf;
	uInt32			*writerIDs			= hist->writerIDs;		// >>> #CE_3.1.0 2026-08-20
	UHandle			*OriginalArrayPnt	= (UHandle *)dBuf;
	int32			OriginalArraySize	= histSize;
	MgErr			err = noErr;

	if (reverse) {	// *** Filling out history in reverse order
		while(histSize--) {
			if (writerIDs) *idBuf++ = writerIDs[idx];		// >>> #CE_3.1.0 2026-08-20
			*tBuf++	= hBuf[idx].time;
			*dBuf	= NULL;
			if (err = LStr_CopyOrCreate( dBuf, hBuf[idx].hdl))
				goto out;
		// ---
			_flat_data(type, *dBuf, err_f);		// flats 1D,2D Array of Strings only
			if (*err_f)
				*err_c = VM_ERR_BADDATA;
			dBuf++;

			if (--idx < 0)	idx = hist->bufSize -1;
		}
	}
	else {		// *** Filling out history in direct order
		tBuf += histSize; 
		dBuf += histSize;
		idBuf+= histSize;		// >>> #CE_3.1.0 2026-08-20
		while(histSize--) {
			if (writerIDs)	*(--idBuf) = writerIDs[idx];	// >>> #CE_3.1.0 2026-08-20
			*(--tBuf) = hBuf[idx].time;
			*(--dBuf) = NULL;
			if (err = LStr_CopyOrCreate( dBuf, hBuf[idx].hdl))
				goto out;
		// ---
			_flat_data(type, *dBuf, err_f);		// flats 1D,2D Array of Strings only
			if (*err_f)
				*err_c = VM_ERR_BADDATA;

			if (--idx < 0)	idx = hist->bufSize -1;
		}
	}
out:
	if (err) {
		ce_DisposeHandles(OriginalArrayPnt, OriginalArraySize);	// free allocated memory if an error
	}
	return err;
}

// ====================================================
// *** DBL Scalar flattened to string
// ====================================================
MgErr _fill_dFS(	float64		*tBuf,
					LStrHandle	*dBuf,
					uInt32		*idBuf,
					_VHistory	*hist,
					int32		histSize,
					int32		idx,
					Bool32		reverse	)
{
	_pVHistData		hBuf				= hist->buf;
	uInt32			*writerIDs			= hist->writerIDs;		// >>> #CE_3.1.0 2026-08-20
	UHandle			*OriginalArrayPnt	= (UHandle *)dBuf;
	int32			OriginalArraySize	= histSize;
	MgErr			err = noErr;

	if (reverse) {	// *** Filling out history in reverse order
		while(histSize--) {
			if (writerIDs) *idBuf++ = writerIDs[idx];		// >>> #CE_3.1.0 2026-08-20
			*tBuf++	= hBuf[idx].time;
			*dBuf	= NULL;
			if (err = LStr_CreateFromMem(dBuf, (char*)&(hBuf[idx].dbl), sizeof(float64)) )
				goto out;
			reverse_dbl( LStrBuf(**dBuf), 1);	// FlattenData.c
			dBuf++;
			if (--idx < 0)	idx = hist->bufSize -1;
		}
	}
	else {		// *** Filling out history in direct order
		tBuf += histSize;
		dBuf += histSize;
		idBuf+= histSize;		// >>> #CE_3.1.0 2026-08-20
		while(histSize--) {
			if (writerIDs)	*(--idBuf) = writerIDs[idx];	// >>> #CE_3.1.0 2026-08-20
			*(--tBuf) = hBuf[idx].time;
			*(--dBuf) = NULL;
			if (err = LStr_CreateFromMem(dBuf, (char*)&(hBuf[idx].dbl), sizeof(float64)) )
				goto out;
			reverse_dbl( LStrBuf(**dBuf), 1);	// FlattenData.c
			if (--idx < 0)	idx = hist->bufSize -1;
		}
	}
out:
	if (err) {
		ce_DisposeHandles(OriginalArrayPnt, OriginalArraySize);	// free allocated memory if an error
	}
	return err;
}
