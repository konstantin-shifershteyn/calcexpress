/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__VM_ERRORS__
#define	__VM_ERRORS__

// =============================================
//		VarMan Error Codes
// =============================================

#if defined __VarMan_20_Errors__
#define	VM_ERR_NOERROR		VM_ERR_OK
#define	VM_ERR_VAR_INVTYPE	VM_ERR_INVTYPE
#define	VM_ERR_VAR_NOTFOUND	VM_ERR_NOTDEF
#define	VM_ERR_VAR_NONUNIQUE	VM_ERR_ALREADYDEF
#define	VM_ERR_CTX_INVHANDLE	VM_ERR_INVHANDLE
#else
#define	VM_ERR_NOERROR		0
#define	VM_ERR_VAR_INVTYPE	1
#define	VM_ERR_VAR_NOTFOUND	2
#define	VM_ERR_VAR_NONUNIQUE	3
#define	VM_ERR_CTX_INVHANDLE	4
#define	VM_ERR_INPUTERROR	5
#define	VM_ERR_BADDATA		6
#endif

// New error codes in VarMan 2.2
// -----------------------------

// History specific error codes
#define	VM_ERR_HIST_NOHISTORY	7
#define	VM_ERR_HIST_ATTACHED	8
#define	VM_ERR_HIST_INVPARAMS	9

// Variable name specific error code
//#define	VM_ERR_VAR_INVNAME	VM_ERR_INVTYPE	// !!!
#define	VM_ERR_VAR_INVNAME	10	// при создании

// Aliase specific error codes
#define	VM_ERR_ALIAS_INVNAME	11	// при создании или переименовании
#define	VM_ERR_ALIAS_NONUNIQUE	12	// при создании или переименовании
#define	VM_ERR_ALIAS_NOTFOUND	13	// при обращении по алиасу
#define	VM_ERR_ALIAS_NOALIAS	14	// при удалении (переменная не имеет алиас)
#define	VM_ERR_ALIAS_ATTACHED	15	// при создании (переменная уже имеет алиас)

// Context Name specific error codes
#define	VM_ERR_VCNAME_INVNAME	16	// VM Engine returns VM_ERR_INVTYPE
#define	VM_ERR_VCNAME_NONUNIQUE	17	// VM Engine returns VM_ERR_ALREADYDEF
#define	VM_ERR_VCNAME_NOTFOUND	18	// VM Engine returns VM_ERR_NOTDEF

// Generic error codes
#define	VM_ERR_INVALID_PARAMS	19	// General 'invalid parameters' error
#define	VM_ERR_GENERIC_ERROR	20	// Generic error

// New error codes in VarMan 2.4
// -----------------------------

// LabVIEW errors
#define VM_ERR_MEMORY_FULL		21	// Memory Full: mFullErr
#define	VM_ERR_LABVIEW_ERROR	22	// Unrecognized LabVIEW error


#define	VM_ERR_INVALID_CMD	VM_ERR_INVALID_PARAMS	// Changed in VarMan v2.4
//#define	VM_ERR_INVALID_CMD	255


// ========================================================
//	LabVIEW Manager's error handler
// ========================================================

#ifdef	__cplusplus
extern "C" {
#endif

__inline MgErr	vm_LVError(MgErr err, LVBoolean *err_f, uInt16 *err_c ) {
	if (err) {
		*err_f	= LVTRUE;
		if (err == mFullErr)	*err_c = VM_ERR_MEMORY_FULL;
		else					*err_c = VM_ERR_LABVIEW_ERROR;
	}
	return err;
}

#ifdef	__cplusplus
}
#endif


#endif
