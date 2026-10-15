/* *********************************************************************
 * CalcExpress Engine (CalcExpress Toolkit for LabVIEW(TM))
 * Copyright (C) 2000-2026 Konstantin Shifershteyn. All rights reserved.
 * Distributed under the terms of the MIT License.
 * (See accompanying LICENSE file in the root directory for full terms)
 * ********************************************************************* */

#include <stdlib.h>

#include <celib\ceMemLib.h>


// ============================================
// Reverses order of elements in a memory block
// ============================================
void ReverseBlock(void *pnt, int32 elNums, int32 elSize)
{
	char	*p1,*p2;
	int32	i;

	p1 = pnt;
	p2 = p1 + elSize * (elNums-1);
	for(i = elNums/2; i > 0; i--) {
		SwapBlock(p1, p2, elSize);
		p1 += elSize;
		p2 -= elSize;
	}
}


// ==============================================================
// Transposes elements in a memory block that represents 2D array
// ==============================================================
MgErr	Transpose2D(void *pnt, int32 size1, int32 size2, int32 elSize)
{
	int32	i,j, elNums;
	char	*p1,*p2;

	elNums	= size1 * size2;

	if (size1 == size2) {
		for (i=0; i<size1; i++)
			for (j=i; j<size2; j++)
				if (i!=j) {
					p1 = (char*)pnt + elSize*(i*size2 +j);
					p2 = (char*)pnt + elSize*(j*size1 +i);
					SwapBlock(p1,p2,elSize);
				}

	}
	else {	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// Bad algorithm because of need to allocate memory
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		void	*tmp;
		tmp = ce_DSNewPtr(elNums * elSize);
		if (!tmp) return mFullErr;

		_MoveBlock(pnt,tmp,elNums * elSize);
		for (i=0; i<size1; i++)
			for (j=0; j<size2; j++) {
				p1 = (char*)tmp + elSize*(i*size2 +j);
				p2 = (char*)pnt + elSize*(j*size1 +i);
				_MoveBlock(p1,p2,elSize);
			}
		DSDisposePtr(tmp);
	}
	return noErr;
}

// ============================================
// Shifts elements in a memory block
// ============================================
void ShiftBlock(void *pnt, int32 elNums, int32 elSize, int32 n)
{
	char	*p1,*p2;
	int32	ni;

	if (!n)	return;

	if (n > 0) { p1 = pnt; p2 = p1 + n * elSize; }
	else	   { p2 = pnt; p1 = p2 - n * elSize; }

	ni = elNums - abs(n);
	if (ni <= 0) {
		p1 = pnt; n = elNums;
	}
	else {
		_MoveBlock(p1, p2, ni * elSize);
		if (n < 0) {
			n  = abs(n);
			p1 = p2 + (elNums-n) * elSize;
		}
	}
	_ClearMem(p1, n * elSize);
}


// ============================================
// Rotates elements in a memory block
// ============================================
MgErr RotateBlock(void *pnt, int32 elNums, int32 elSize, int32 n)
{

	char	*buf, buffer[32];
	char	*small_p, *small_dst;
	char	*large_p, *large_dst;
	int32	small_size, large_size, total_size;

	if (elNums <= 1)	return noErr;
	if (!(n %= elNums))	return noErr;

	if (abs(n) > elNums/2) {
		if (n>0)	n -= elNums;
		else		n += elNums;
	}

	total_size = elNums * elSize;
	small_size = elSize * abs(n);
	large_size = total_size - small_size;

	if (n>0) {
		large_p		= pnt;
		small_p		= large_p + large_size;
		large_dst	= large_p + small_size; // elSize * n;
		small_dst	= large_p;
	}
	else {
		small_p		= pnt;
		large_p		= small_p + small_size;
		large_dst	= small_p;
		small_dst	= large_dst + large_size;
	}

	if (small_size > sizeof(buffer)) {
		if (!(buf = ce_DSNewPtr(small_size))) return mFullErr;
	}
	else	buf = buffer;

	_MoveBlock(small_p, buf,       small_size);
	_MoveBlock(large_p, large_dst, large_size);
	_MoveBlock(buf,		small_dst, small_size);

	if (buf != buffer)
		DSDisposePtr(buf);

	return noErr;
}
