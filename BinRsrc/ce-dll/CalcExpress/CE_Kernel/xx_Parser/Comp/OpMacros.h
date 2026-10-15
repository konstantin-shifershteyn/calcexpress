/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */
 

// ========================================
// Макрос проверяет операции типа '/' '/='
// ========================================
#define test2(op1,op2)					\
	if (*(pnt+off) == '=')	{ pop = op2; off++; }	\
	else					{ pop = op1; }		\

// ========================================
// Макрос проверяет операции типа '+' '-' где + и - могут быть unary/binary
// ========================================
#define test2a( op11, op12, op2)			\
	if (*(pnt+off) == '=')	{ pop = op2; off++; }	\
	else { pop = *Operand ? op12 : op11; }		\

// ============================================
// Макрос проверяет операции типа '&' '&&' '&='
// ============================================
#define test2_2(ch, op1, op2, op3)		\
	switch (*(pnt+off)) {				\
		case ch  : pop = op2; off++; break;	\
		case '=' : pop = op3; off++; break;	\
		default  : pop = op1; break;		\
	}						\

// ==================================================
// Макрос проверяет операции типа '<' '<<' '<=' '<<='
// ==================================================
#define test3(ch, op1,op2,op3,op4)			\
	switch (*(pnt+off)) {					\
		case ch : off++; test2(op2,op4);break;	\
		case '=': pop = op3; off++;		break;	\
		default : pop = op1;			break;	\
	}						\

