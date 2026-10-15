/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <extcode.h>
#include <ceKernel.h>
#include <CSE.h>
#include <celib\ceMemLib.h>


int32	CSE_Diag_Convert_ItemID(int32	extItemID)
{
	static const int32	Table[] = {
					CEK_MOD_Diag_Vars,
					CEK_MOD_Diag_Functions,
					CEK_MOD_Diag_FuncVI,
					CEK_MOD_Diag_FastFunc,
					CEK_MOD_Diag_Script			/// >>> #CE_3.0.7 2026-07-25
				};
	if ((extItemID >= 0) && (extItemID < sizeof(Table)/sizeof(Table[0])) )
		return Table[extItemID];
	return extItemID;
}


// =============================
MgErr	CSE_List_Of_Item_Names(
				_CE_Instance	*ip,
				LStrHandle		outString,
				int32			ItemID )
{
	_CEKernel_Par_Diag	ParDiag;
	LVBoolean			errf;

	if (!ip)
		return LStr_Clear(outString);

	ParDiag.in_Mode	= ItemID;	// CEK_MOD_Diag_FuncVI;
	ParDiag.io_List	= outString;
	errf = LVFALSE;

	return	CEKernel_Diag( ip, &ParDiag, &errf);
}

// =============================
MgErr	CSE_Array_Of_Lists_Of_Item_Names(
						iPScrInstance	*iCSE,
						LStr1DArrayHdl	outArray,
						int32			ItemID )
{
	iPScrArrayHdl		ipsa;
	iPScrElement		*ipe;
	LStrHandle		*listArr;
	int32			size;
	int32			i;
	_CEKernel_Par_Diag	ParDiag;
	LVBoolean		errf;
	MgErr			err = noErr;

	ipsa = iCSE->iPScr;
	if (!ipsa)	return mZoneErr;	//protection ???
	size = iPScrSize(*ipsa);


	// ***	если массив не пустой, то разрушить его элементы (строки) ???
	ce_DisposeHandles( (UHandle*)&((*outArray)->data[0]), (*outArray)->size);


	// Изменить размер массива
	// =======================
	if (err = ce_1DArrayResize((UHandle*)&outArray, size, sizeof(LStr1DArray)) )
		return err;	//goto out;

	listArr = &((*outArray)->data[0]);

	// подготовить LStrHandle элементов массива (empty strings)
	// ========================================
	for (i=0; i<size; i++) {
		if (err = LStr_Create( &(listArr[i]), 0) )
			goto out;
	}

	// получить списки FuncVIs
	// =======================
	for (i=0; i<size; i++) {

		ipe = iPScrEPnt(*ipsa, i);
		if (ipe->ip == NULL)		// if empty statement
			continue;		// skip it

		ParDiag.in_Mode	= ItemID;	// CEK_MOD_Diag_FuncVI;
		ParDiag.io_List	= listArr[i];
		errf = LVFALSE;
//		if (err = cekrnl( CEK_CMD_Diag_Script, &(ipe->ip), (void*)&ParDiag, NULL, &errf) )
//			goto out;

		if (err = CEKernel_Diag( ipe->ip, &ParDiag, &errf) )
			goto out;
	}
out:
	if (err) {
		ce_DisposeHandles( (UHandle*)&((*outArray)->data[0]), (*outArray)->size);
		ce_1DArrayResize(  (UHandle*)&outArray, 0, sizeof(LStr1DArray));
	}
	return err;
}
