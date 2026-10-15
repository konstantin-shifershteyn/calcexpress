/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */


// ############################################################
MgErr	CE_EOSToError(	_CE_Instance	*ip)
{
	return CE_Error( ip, " [EndOfScript]", NULL,NULL);
}

// ############################################################
MgErr	CE_SrcToError(	_CE_Instance	*ip,
						char			*pnt,
						int32			actLen,
						int32			maxLen)
{
	int32	len;
	char	*p, *fs;
	MgErr	err = noErr;

	if (actLen <= maxLen)
	{	len = actLen;	fs = "%s";	}
	else {	len = maxLen;	fs = "%s...";	}

	if (p = ce_DSNewPClr( len + 1)) {
		StrNCpy(p, pnt, len);

		err = CE_Error(ip, fs, (LStrHandle)p, NULL);
		DSDisposePtr(p);
	}
	return	err;
}


// ############################################################
// this function is only used in "cekrnl_Parser.c" to compare 
// input expression with source of already parsed expression
Bool32	CmpStrings_X(	LStrHandle	oldStr,
						LStrHandle	newStr)
{
	int32	len;

	len = LStrLen(*newStr);
	if ( LStrLen(*oldStr) != (len+1) )	// stored expression (oldStr) has the zero byte appended
		return FALSE;
	return !memcmp( LStrBuf(*oldStr), LStrBuf(*newStr), len);
//	return !BlockCmp( LStrBuf(*oldStr), LStrBuf(*newStr), len);
}

// ############################################################
MgErr LStr_copy_n(	uChar		*src,
					LStrHandle	dst,
					int32		off,
					int32		len)
{
	MgErr	err;

	// скопировать строку
	if ( !(err = LStr_SetSize(dst, len)) )
		_CopyBlock( src+off, LStrBuf(*dst), len);
	return err;
}


// ####################################################################
// Сохранить входную строку
// ####################################################################

MgErr InString_Init( _CE_Instance *ip, LStrHandle InputString )
{
	int32	datlen;
	MgErr	err;

	datlen = LStrLen(*InputString);

//	if ( ! (err = LStr_Resize( ip->str, datlen + 1)) ) {
//		MoveBlock( LStrBuf(*InputString), LStrBuf(*(ip->str)), datlen);
//		*(LStrBuf(*(ip->str)) + datlen) = '\0';
//	}
	if ( ! (err = LStr_SetSize( ip->str, datlen + 1)) )
		*((char*)_CopyBlock(LStrBuf(*InputString), LStrBuf(*(ip->str)), datlen)) = '\0';
	return err;
}

// ####################################################################
MgErr TransInit( _CE_Instance *ip, LStrHandle InString)
{
	MgErr	err = noErr;

	ip->procPars.ErrorOfParsing = BitFALSE;

	ExAr_FreeAllocatedMemory(ip, 0);			// >>> #CE_3.0.4 2026-06-22

	if (err = InString_Init(ip, InString))
		goto out;

	if (err = CE_Error_Init(ip))
		goto out;

	OpdStack_Init(ip);
	OpsStack_Init(ip);
	FIOStack_Init(ip);
out:
	return err;
}



// ##############################################################
// Return:
// 1:	'A'-'Z' , 'a'-'z' , '_'
// 2:	'0'-'9'
// 3:	'.'
// 4:	'\"'
// 5:	'\''

//#pragma warning (disable: 4035)
__inline int32	__fastcall	CharType(uChar Sym)
{
	const static  uChar	CTTbl[256] = 
	{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 4, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 3, 0 ,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0 ,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1 ,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 ,
	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
	};
/*
	__asm
	{
		xor	edx, edx
		mov	dl, Sym
		xor	eax, eax
		mov	al, BYTE PTR CTTbl[edx]
	}
*/
//	if (Sym >= 0x80) return 0;		// >>> #CE_3.0.5 2025-07-18
	return	(int32)CTTbl[Sym];		// >>> 2024-12-06 : CHECK 64 BIT
}
#pragma warning (default: 4035)
