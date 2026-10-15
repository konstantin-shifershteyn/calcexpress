/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


size_t	_metrics_history_mem(_VHistory	*hist)
{
	size_t		size = 0;
	uInt32		i;
	_VHistData	*hBuf;

	if (!hist)	return 0;

	size	= sizeof(_VHistory);
	size	+= hist->bufSize * sizeof(_VHistData);

// >>> #CE_3.1.0 2026-08-20
	if (hist->writerIDs)
		size += hist->bufSize * sizeof(*(hist->writerIDs));
// <<< #CE_3.1.0 2026-08-20

	if ((hist->vType==VM_TYPE_DBL) || (hist->vType==VM_TYPE_IDXRANGE))
		goto out;
	
	if ( !(hBuf = hist->buf))
		goto out;

	// calculate allocated memory blocks for variable-sized values
	for(i=0; i < hist->bufSize; i++) {
		if (hBuf[i].hdl)
			size += DSGetHandleSize(hBuf[i].hdl);
	}
out:
	return size;	
}

// =============================================================================

void	_metrics_variable_mem(
				_VarRecord	*vp,
				size_t		*vSize,
				size_t		*hSize,
				Bool32		addMasterSize)
{
	size_t	varSize	= 0;
	size_t	histSize= 0;

	if (!vp)
		goto out;

	varSize	= addMasterSize ? sizeof(_VarRecord) : 0;

	// ***	name
	if (vp->name)
		varSize	+= DSGetHandleSize(vp->name);

	// ***	attribute
	if (vp->attrib)
		varSize += DSGetHandleSize(vp->attrib);

	// ***	string value
	if (vp->inf.str)
		varSize += DSGetHandleSize(vp->inf.str);

	// ***	alias
	if (vp->alias) {
		varSize	+= sizeof(_AliasRecord);
		if (vp->alias->name)
			varSize	+= DSGetHandleSize(vp->alias->name);
	}

	// ***	history
	if (vp->hist) {
		histSize = _metrics_history_mem(vp->hist);
		varSize	+= histSize;
	}
out:
	*vSize	= varSize;
	*hSize	= histSize;
	return;
}

// =============================================================================

void	_metrics_context_mem( int32 idx, size_t *cSize, size_t *hSize, Bool32 addMasterSize )
{
	int32			i;
	size_t			ctxSize	= 0, ctxSizeTmp;
	size_t			histSize= 0, histSizeTmp;
	_CTableEl		*cxp;
	_HashTableRec	*ht;
	_VarRecord		*vp, *vlist;

	if (idx < 0)	goto out;

	// pointer to Context Table
	cxp	= &((*ctbl)->data[idx]);

	// pointer to Variable Hash Table in the context
//	ht	= cxp->hktbl;
//	if (!ht)	goto out;	//stub
	ht	= &(cxp->hktbl[0]);	// >>> #CE_3.1.0 2026-08-18

	ctxSize = addMasterSize ? sizeof(_CTableEl) : 0;

// >>> #CE_3.1.0 2026-09-15 : DEPRECATED : HashTable is now a static member of the context
	// ***	size of HashTable
//	ctxSize	+= sizeof(_HashTableRec) * VAR_HASH_TABLE_SIZE;
// <<< #CE_3.1.0 2026-09-15

	// *** size of VC attributes
	if (cxp->Creator)	ctxSize	+= DSGetHandleSize(cxp->Creator);
	if (cxp->vcName)	ctxSize += DSGetHandleSize(cxp->vcName);

	// ***	size of all variables
	for (i=0; i < VAR_HASH_TABLE_SIZE; i++) {
// >>> #CE_3.1.0 2026-09-15
#if (VAR_ARRAY_SIZE > 0)
		int j;
		// ### scan the var array
		vp = &(ht[i].varray[0]);
		for (j=0; j < VAR_ARRAY_SIZE; j++, vp++) {
			//if (vp->name == NULL)		// if not used
			//	continue;
			_metrics_variable_mem(vp, &ctxSizeTmp, &histSizeTmp, FALSE);
			ctxSize	+= ctxSizeTmp;
			histSize+= histSizeTmp;
		}
#endif
// <<< #CE_3.1.0 2026-09-15
		vlist = ht[i].vlist;
		while (vlist) {
			vp	= vlist;
			vlist	= vlist->link;
			_metrics_variable_mem(vp, &ctxSizeTmp, &histSizeTmp, TRUE);
			ctxSize	+= ctxSizeTmp;
			histSize+= histSizeTmp;
		}
	}

out:
	*cSize	= ctxSize;
	*hSize	= histSize;
	return;
}

// =============================================================================

void	_metrics_total_mem(	size_t *tSize, size_t *hSize )
{
	int32		i;
	size_t		totSize	= 0, totSizeTmp;
	size_t		histSize= 0, histSizeTmp;

//DbgPrintf(__FUNCTION__ "(): sizeof(_CTableEl) = %u bytes\n", sizeof((*ctbl)->data));
//DbgPrintf(__FUNCTION__ "(): sizeof(_HashTable) = %u bytes\n", sizeof((*ctbl)->data[0].hktbl));
//DbgPrintf(__FUNCTION__ "(): sizeof(_HashTableRec) = %u bytes\n", sizeof(_HashTableRec));
//DbgPrintf(__FUNCTION__ "(): sizeof(_VarRecord) = %u bytes\n", sizeof(_VarRecord));

	// ### Context table
	totSize	 = DSGetHandleSize(ctbl);	// >>> 2024-12-06 : CHECK 64 BIT
	for (i=0; i < (*ctbl)->dimSize; i++) {
		_metrics_context_mem( i, &totSizeTmp, &histSizeTmp, FALSE );
		totSize	+= totSizeTmp;
		histSize+= histSizeTmp;
	}

	// ### Alias Hash Table
	totSize	+= sizeof(aht);		//sizeof(_AliasHashTable);
	for (i=0; i < ALIAS_HASH_TABLE_SIZE; i++) {
		_AliasRecord	*ap;
		for( ap = aht[i]; ap; ap = ap->lnkHashList ) {
			if (ap->name)
				totSize += DSGetHandleSize(aht[i]->name);
		}
	}

	// ### exit
	*tSize	= totSize;
	*hSize	= histSize;
	return;
}

// =============================================================================

MgErr	viVMMetrics(
			int32		vch,
			LStrHandle	varName,
			uInt32		object,
			size_t		*totSize,
			size_t		*histSize,
			uInt16		*err_c )
{
	_VarRecord	*vep;
	int32		idx;
	LVBoolean	glob;

//	*err_c = VM_ERR_NOERROR;

	if (!totSize || !histSize) {
		*err_c = VM_ERR_INVALID_PARAMS;
		return noErr;
	}
	*totSize	= 0;
	*histSize	= 0;

	VARMAN_LOCK_EXCLUSIVE();

	switch(object) {

		case 0:	// ***	calculate memory size used by the specified variable
			// ---------------------------------------------------------
			glob = (vch == -1);
			if ( vep = __GetVarPointer(vch, &glob, varName, NULL, err_c) ) {
				_metrics_variable_mem( vep, totSize, histSize, TRUE );
			}
			break;

		case 1:	// ***	calculate memory size used by the specified context
			// --------------------------------------------------------
			if (vch == -1) {
				idx = CTBL_GBLIDX;				// Global Context
			}
			else {
				if ( (idx = ctbl_find_index(vch)) < 0) {	// найти контекст
					*err_c = VM_ERR_CTX_INVHANDLE;
					goto out;
				}
			}
			_metrics_context_mem( idx, totSize, histSize, TRUE );
			break;

		case 2:	// ***	calculate total size of allocated memory
			// ---------------------------------------------
			_metrics_total_mem(totSize, histSize );
			break;

		default:
			*err_c = VM_ERR_INVALID_PARAMS;
			break;
	}
out:
	VARMAN_UNLOCK_EXCLUSIVE();

	return noErr;
}
