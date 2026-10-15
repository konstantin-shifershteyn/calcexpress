
/*
// ======================
// 	Sum
// ======================
float64 _sum_(register float64 *x, register int32 num)
{
	float64	sum = 0.0L;

	while(--num >= 0)
		sum += *x++;
	return sum;
}
*/
// ======================
// 	Mean
// ======================
float64	mean(register float64 *x, register int32 num)
{
	float64	sum	= 0.0;
	float64	N	= (float64)num;

	while(--num >= 0)
		sum += *x++;
	return sum/N;
}

// ================================
// 	Root Mean Square (RMS)
// ================================
float64	rms(register float64 *x, register int32 num)
{
	float64	sum	= 0.0;
	float64	N	= (float64)num;

	while(--num >= 0)
		sum += (*x) * (*x++);
	return sqrt(sum/N);
}

// ==================================
//	Mean Square Error (MSE)
// ==================================
float64 mse(register float64 *x, int32 xn, register float64 *y, int32 yn)
{
register int32	num;
	float64	N, tmp;
	float64	sum	= 0.0;

	num = xn <= yn ? xn : yn;  //__min(xn, yn);
	N   = (float64)num;

	while(--num >= 0) {
		tmp  = *x++ - *y++;
		sum += tmp*tmp;
	}
	return sum/N;
}

// ********************************************************************************

// ======================
// 	Variance
// ======================
float64	var(register float64 *x, register int32 num)
{
	float64	avg, tmp;
	float64	sum	= 0.0;
	float64	N	= (float64)num;

	if (num <= 0)	return _NaN_;

	avg = mean(x, num);

	while(--num >= 0) {
		tmp = *x++ - avg;
		sum += tmp*tmp;
	}
	return	sum/N;
}

// =============================
// 	Standard Deviation
// =============================
float64	stdev(register float64 *x, register int32 num)
{
	return sqrt(var(x, num));
}

// ======================
// 	Sample Variance
// ======================
float64	vars(register float64 *x, register int32 num)
{
	float64	avg, tmp;
	float64	sum	= 0.0;
	float64	N	= (float64)num;

	if (num < 2)	return _NaN_;

	avg = mean(x, num);

	while(--num >= 0) {
		tmp = *x++ - avg;
		sum += tmp*tmp;
	}
	return	sum/(N-1);
}

// ==================================
// 	Sample Standard Deviation
// ==================================
float64	stdevs(register float64 *x, register int32 num)
{
	return sqrt(vars(x, num));
}

// ********************************************************************************

// =================================
// 	Mean Absolute Deviation
// =================================
float64	madev(register float64 *x, register int32 num)
{
	float64	avg;
	float64	sum	= 0.0;
	float64	N	= (float64)num;

	if (num <= 0)	return _NaN_;

	avg = mean(x, num);

	while(--num >= 0)
		sum += fabs(*x++ - avg);

	return	sum/N;
}

// ===========================
// 	Moment About Mean
// ===========================
float64	moment(register float64 *x, register int32 num, int32 order)
{
	float64	avg;
	float64	sum	= 0.0;
	float64	ord	= (float64)order;
	float64	N	= (float64)num;

	if (num <= 0 || order < 0)	return _NaN_;

	avg = mean(x, num);

	while(--num >= 0)
		sum += pow(*x++ - avg, ord);

	return	sum/N;
}
/*
// ===============================
// 	Median
// ===============================

int32	_median_compare(float64 *x, float64 *y)
{
	if (*x < *y)	return -1;
	return (*x != *y);
}

float64	median(float64 *x, register int32 num)
{
	if (num <= 0)	return 0;
	if (num == 1)	return *x;

	QSort( (UPtr)x, num, sizeof(float64), _median_compare);

	x += num/2;

	if (num & 1)	return *x;
	else		return (*x + *(x-1))/2;
}
*/
