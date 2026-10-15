/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ==========================================
// Функция подготавливает данные к выполнению
// ==========================================
__inline MgErr ExecPrepare(_CE_Instance *ip)
{
	// Размер стека операндов = 0
	// ==========================
	ip->OpdNum = 0;

	// Временные операнды
	// ==================
	ip->LOpd.iip = ip->ROpd.iip = NULL;

	// Reset error information
	// =======================
	return CE_Error_Init(ip);
}
