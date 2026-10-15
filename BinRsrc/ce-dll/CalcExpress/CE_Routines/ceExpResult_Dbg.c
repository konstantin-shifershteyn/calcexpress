/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#if define(_TRACE_ME)

err = CE_Error(ip, "Type = %d; op->iip = %d\n", (LStrHandle)op->type, (LStrHandle)op->iip);
err = CE_Error(ip, "\n NumVal = %d", (LStrHandle)(uInt32)op->num,	NULL);
err = CE_Error(ip, "\n StrHdl = %d", (LStrHandle)op->str,		NULL);

err = CE_Error(ip, "\n Name via NumVal = %H", getIdentString(op),	NULL);
err = CE_Error(ip, "\n Name via StrHdl = %H", op->str,			NULL);

err = CE_Error(ip, "\n IdxQry = %d", (LStrHandle)op->iip->qty,		NULL);
err = CE_Error(ip, "\n idx1.nDims = %d", (LStrHandle)op->iip->idx1.nDims,		NULL);
err = CE_Error(ip, "\n idx1.size1 = %d", (LStrHandle)op->iip->idx1.size1,		NULL);
err = CE_Error(ip, "\n idx1.size2 = %d", (LStrHandle)op->iip->idx1.size2,		NULL);
err = CE_Error(ip, "\n idx1.scalar= %d", (LStrHandle)op->iip->idx1.scalar,		NULL);

err = CE_Error(ip, "\n &idx1.scalar = %d", (LStrHandle)&(op->iip->idx1.scalar),		NULL);
err = CE_Error(ip, "\n idx1.values  = %d", (LStrHandle)op->iip->idx1.values,		NULL);
err = CE_Error(ip, "\n idx1.alloc = %d",   (LStrHandle)op->iip->idx1.alloc,		NULL);

#undef	_TRACE_ME

#endif
