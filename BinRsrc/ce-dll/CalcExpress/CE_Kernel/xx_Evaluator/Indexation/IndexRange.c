/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


MgErr IdxRangeOperator(	_CE_Instance	*ip,
						_Operand		*Lopd,
						_Operand		*Ropd,
						LVBoolean		*err_f)
{
	int32	lowBound = 0, topBound = 0;
	MgErr	err = noErr;

	if (err = OpdStack_OpdValue(ip, Lopd, err_f))	goto out;
	if (err = OpdStack_OpdValue(ip, Ropd, err_f))	goto out;
	if (*err_f)
		goto out;

	switch(Lopd->type) {
		case OPTYPE_DBL :		lowBound = _DblToI32(Lopd->num);
								break;
		case OPTYPE_IDXRANGE :	lowBound = _UnpackIdxRangeFromFloat(&(Lopd->num), 0);
								break;
		default:				err = CE_Error(ip, "Invalid type of left-hand operand",NULL,NULL);
								*err_f = LVTRUE;
								goto out;
	}
	switch(Ropd->type) {
		case OPTYPE_DBL :		topBound = _DblToI32(Ropd->num);
								break;
		case OPTYPE_IDXRANGE :	topBound = _UnpackIdxRangeFromFloat(&(Ropd->num), 1);
								break;
		default:				err = CE_Error(ip, "Invalid type of right-hand operand",NULL,NULL);
								*err_f = LVTRUE;
								goto out;
	}
out:
	_PackIdxRangeToFloat(&(Lopd->num), lowBound, topBound);
	Lopd->type = OPTYPE_IDXRANGE;
	return err;
}
