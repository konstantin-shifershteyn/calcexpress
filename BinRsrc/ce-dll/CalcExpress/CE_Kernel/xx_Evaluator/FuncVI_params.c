/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
 
// Последняя модификация:	17:15 20-09-01

// =======================================================================
// Эта функция подготавливает операнды в стеке для вызова FuncVI
// =======================================================================
// 1)	Эта функция копирует реальные строки в _Operand.str если это необходимо
// Старая версия не поддерживает получение хендла имени идентификатора через
// _Operand.hdl. В ней все handles должны быть в _Operand.str и указывать на
// реальную строку, что приводило к необходимости копировать строки целиком при
// копировании структуры _Operand
// 2)	Если в качестве аргументов функции имеются индексированные переменные,
// то они заменяются на их значения. Внешние CINs и особенно VIs не имеют доступа
// к структуре индексов.
// =======================================================================
MgErr FuncVI_Params(
				_CE_Instance	*ip,
				int32			n_operands,
			 	LVBoolean		*err_f)
{
	MgErr		err = noErr;
	_Operand	*opd;

//	if (*err_f) goto out;

	opd = &((*(ip->OpdStk))->data[ip->OpdNum - n_operands + 1]);

	while (n_operands--) {
		if (opd->type == OPTYPE_IDENT) {
			if (!(opd->iip)) {
				err = CE_InternalError(ip, "opd->iip == NULL",NULL,NULL);
				*err_f = LVTRUE;
				goto out;
			}
			if (opd->iip->qty) {
				// если индексированный операнд
				// то получить значение
				err = GetVariable(ip, opd, (_OperandBase*)opd,err_f);
			}
			else {	// Сформировать реальную строку идентификатора
				if (err = LStr_Copy(opd->str, getIdentString(opd)))
					goto out;
				opd->num = 0;
			}
			if (err || *err_f)
				goto out;
		}
		opd++;
	}
out:
	return err;
}
