
// *** comparison function for QSort()
int32	__qsort_comp1_dbl(register float64 *x, float64 *y)
{
	if (*x < *y)	return -1;
	return (*x != *y);
}

// ===============================
// 	Median
// ===============================
float64	median(float64 *x, register int32 num)
{
	if (num <= 0)	return _NaN_;
	if (num == 1)	return *x;

	QSort( (void*)x, num, sizeof(float64), (CompareProcPtr)__qsort_comp1_dbl);

	x += num/2;

	if (num & 1)	return *x;
	else		return (*x + *(x-1))/2;
}

// ===============================
// 	Mode(x, i)
// ===============================
float64	mode(register float64 *x, int32 len, int32 intervals)
{
register int32	i;		// x index
	int32	curI, maxI;	// interval indexes for current and max interval
	int32	curV, maxV;	// histogram values for current and max interval
	float64	min, max, dx, imax;

	if (len <= 0)		return _NaN_;
	if (len == 1)		return *x;
	if (intervals <= 0)	intervals = 1;

	QSort( (void*)x, len, sizeof(float64), (CompareProcPtr)__qsort_comp1_dbl);

	maxI	= maxV = 0;
	min	= *x;
	max	= *(x + len-1);
	dx	= (max - min)/(float64)intervals;
	imax	= min + dx;

	for (i=curI=0; curI < intervals; curI++) {
		curV = 0;
		while (x[i] < imax) {
			curV += 1;
			if (++i >= len) break;
		}
		if (curI == intervals-1)
			curV += len - i;
		if (curV > maxV) {
			maxI = curI; maxV = curV;
		}
		if (i >= len) break;
		imax += dx;
	}
	return  min + 0.5*dx + dx*(float64)maxI;
}

// ===============================
// Histogram(what, *x, *hist)
// ===============================
int32 histogram(int32 what, register float64 *x, int32 xlen, float64 *hist, int32 hlen)
{
register int32	i;			// x index
register int32	curI;			// interval index
	int32	curV;			// histogram value
	float64	min, max, dx, imax;

	if (hlen <= 0)		return 0;
	for (i=0; i<hlen; i++)	hist[i] = 0.0;
	if (xlen <= 0)		return 0;

	QSort( (void*)x, xlen, sizeof(float64), (CompareProcPtr)__qsort_comp1_dbl);

	min	= *x;
	max	= *(x + xlen-1);
	dx	= (max - min)/(float64)hlen;

	if (!what) {
		// Compute Histogram values
		// ------------------------
		imax = min + dx;
		for (i=curI=0; curI < hlen; curI++) {
			curV = 0;
			while (x[i] < imax) {
				curV += 1;
				if (++i >= xlen) break;
			}
			if (curI == hlen-1)
				curV += xlen - i;
			hist[curI] = (float64)curV;
			if (i >= xlen) break;
			imax += dx;
		}
	}
	else {
		// Compute X values for Histogram
		// ------------------------------
		min += 0.5*dx;
		for (curI=0; curI < hlen; curI++) {

	hist[curI] = min + dx*(float64)curI;

		//	hist[curI] = min;
		//	min += dx;
		}
	}
	return 0;
}
