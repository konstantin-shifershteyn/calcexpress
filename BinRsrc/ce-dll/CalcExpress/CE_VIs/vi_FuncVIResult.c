/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
// VI Interface:	Save result of FuncVI call
// -----------------------------------

#include <extcode.h>

#include <CalcExpress.h>
#include <CEError.h>
#include <ceAccessContext.h>
#include <Index_Routines.h>
#include <lviDataConversion.h>

#include <celib\ceMemLib.h>


// *** Body is at the end of file
MgErr	viFuncVIResult_SaveResult(_CE_Instance *ip, _OperandVI *res, LVBoolean *err_f);


// =========================================
MgErr	__cdecl	viFuncVIResult(
				LVRefNum		*pRefNum,	//_CE_InstancePnt	*InstanceRef,
				_OperandVI		*result,
				LVBoolean		*err_f)
{
	_CE_Instance		*ip;
	MgErr				err = noErr;

	if (*err_f)
		return noErr;

	// ***** Convert LVRefNum to Instance Reference (CE 3.0)
	if ( !(ip = (_CE_Instance*)ceref_GetRefData(*pRefNum, &err)) )
		goto err_InstanceReference;

	// *****  «ахватить контекст
	if (!_CEContext_Lock(ip)) {
err_InstanceReference:
		// *** IF INVALID INSTANCE REFERENCE ***
		*err_f = LVTRUE;	// error
		return noErr;
	}

	// *****  
	if (result->type == OPTYPE_ANY_FLATTENED) {
		*err_f = LVTRUE;
		err = CE_Error(ip, "Invalid datatype of a function return value", NULL, NULL);
		goto out;
	}

	// *****  —охранить результат
	err = viFuncVIResult_SaveResult(ip, result, err_f);

out:
	if (err) {
		CE_Error_LVMgErr(ip, err, err_f);
	}

	// *****  ќсвободить контекст
	_CEContext_UnlockReliable(ip);

	return err;
}

// ================================================================

MgErr	viFuncVIResult_SaveResult(_CE_Instance *ip, _OperandVI *res, LVBoolean *err_f)
{
	_Operand	*oep;
	MgErr		err = noErr;

	oep = OpdStack_EPnt(ip, ip->OpdNum);	// указатель на результат

	oep->num = res->num;
	switch(oep->type = res->type) {
		case OPTYPE_DBL :
		case OPTYPE_IDXRANGE :
			break;

		default :
			if (oep->type == OPTYPE_IDENT) {
				oep->num = 0;		// !!! ќЅя«ј“≈Ћ№Ќќ !!!
				oep->hdl = NULL;	// >>> #CE_3.0.4 2026-07-05
			}
			// *****  скопировать строку
//			if ( err = LStr_Copy( oep->str, res->str) )
			if ( err = ce_CopyOrCreate_OperandString( &(oep->str), res->str, oep->type) )		// >>> #CE_3.0.4 2026-07-06
				goto out;

			// *****  ¬нутри CalcExpress данные хран€тьс€ unflattened
			if ( err = _To_Unflat_Data( ip, (_OperandBase*)oep, err_f) )
				goto out;
			break;
	}

	if (!(oep->iip)) {
		err = CE_InternalError(ip, "Save_FuncResult: oep->iip == NULL",NULL,NULL);
		*err_f = LVTRUE;
		goto out;
	}
	//_IndexInfo_FreeValues(oep);			// отменить индексацию (если есть)
	_IndexInfo_ResetIndexation(oep);		// >>> #CE_3.0.4 2026-07-05
out:
	return err;

}
