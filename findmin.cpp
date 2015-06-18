
#include "stdafx.h"
#include "findmin.h"
#include <iostream>

// using namespace std;

double findmin(double * value)
{
	double min_value = value[0];
	for (int i = 1; i < 2000; i++)
	{
		if (value[i] < min_value)
		{	
			min_value = value[i];
		}
	}
	return min_value; 
}