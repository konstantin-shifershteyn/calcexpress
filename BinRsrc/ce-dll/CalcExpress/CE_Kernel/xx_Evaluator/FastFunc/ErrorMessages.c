/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#ifndef	__FF_ERROR_MESSAGES_C__
#define	__FF_ERROR_MESSAGES_C__

static	char* err_InvArgType_cantArr	= "Invalid parameter type (it can't be an array)";
static	char* err_InvArgType1_cantArr	= "Invalid type of the 1st parameter (it can't be an array)";
static	char* err_InvArgType2_cantArr	= "Invalid type of the 2nd parameter (it can't be an array)";

static	char* err_InvArgType1_needArr	= "Invalid type of the 1st parameter (an array expected)";
static	char* err_InvArgType1_needArr2D	= "Invalid type of the 1st parameter (2D array expected)";

static	char* err_InvArgType1_needArr1D	= "Invalid type of the 1st parameter (1D array expected)";
static	char* err_InvArgType2_needArr1D	= "Invalid type of the 2nd parameter (1D array expected)";
static	char* err_InvArgType3_needArr1D	= "Invalid type of the 3rd parameter (1D array expected)";

static	char* err_InvArgType1_needScal	= "Invalid type of the 1st parameter (scalar expected)";
static	char* err_InvArgType2_needScal	= "Invalid type of the 2nd parameter (scalar expected)";
static	char* err_InvArgType3_needScal	= "Invalid type of the 3rd parameter (scalar expected)";
static	char* err_InvArgType4_needScal	= "Invalid type of the 4th parameter (scalar expected)";

static	char* err_InvArgType_needScalar = "Invalid parameter type (scalar expected)";
static	char* err_InvArgType_needArr	= "Invalid parameter type (an array expected)";
static	char* err_InvArgType_needArr1D	= "Invalid parameter type (1D array expected)";
static	char* err_InvArgType_needArr2D	= "Invalid parameter type (2D array expected)";

static	char* err_IncompatibleArgTypes	= "Incompatible argument types";

// ----------------------

static	char* err_1ArgType_A1DBL	= "Invalid type of the 1st parameter (1D array of DBL expected)";
static	char* err_2ArgType_A1DBL	= "Invalid type of the 2nd parameter (1D array of DBL expected)";
static	char* err_3ArgType_A1DBL	= "Invalid type of the 3rd parameter (1D array of DBL expected)";

static	char* err_1ArgType_DBL		= "Invalid type of the 1st parameter (DBL scalar expected)";
static	char* err_2ArgType_DBL		= "Invalid type of the 2nd parameter (DBL scalar expected)";
static	char* err_3ArgType_DBL		= "Invalid type of the 3rd parameter (DBL scalar expected)";
static	char* err_4ArgType_DBL		= "Invalid type of the 4th parameter (DBL scalar expected)";


#endif
