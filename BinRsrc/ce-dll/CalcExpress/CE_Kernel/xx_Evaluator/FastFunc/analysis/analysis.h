/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__FF_ANALYSIS_H__
#define	__FF_ANALYSIS_H__

// LabVIEW Analysis Error Codes

#ifndef	LV_NoErr
#define	LV_NoErr		0
#endif

#define	LV_OutOfMemErr		-20001
#define	LV_EqSamplesErr		-20002
#define	LV_SamplesGTZeroErr	-20003
#define	LV_SamplesGEZeroErr	-20004
#define	LV_SamplesGEOneErr	-20005
#define	LV_SamplesGETwoErr	-20006

#define	LV_DtGEZeroErr		-20015
#define	LV_DtGTZeroErr		-20016

#define	LV_DecFactErr		-20022

#endif
