
#include "stdafx.h"
#include "trapz.h"
#include <iostream>

using std::endl;

double trapz(double * x, double * y)
{
	double sum = 0.0;
	for (int i = 1; i < 2000; i++)
	{	
		//cout << "x(i-1) = " << x[i-1] << " x(i) = " << x[i] << endl;
		//cout << "y(i-1) = " << y[i-1] << " y(i) = " << y[i] << endl;
		// cout << endl;
		sum += (x[i] - x[i-1]) * (y[i] + y[i-1]);
	}
	return sum * 0.5;
 }