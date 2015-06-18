// LeCroyScope.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "lecroy.h"
#include <iostream>
#include <string.h>
#include <sstream>
#include <fstream>
#include "pop_string.h"
#include "trapz.h"
#include "findmin.h"
#include "histo_class.h"

using std::cout;
using std::endl;
using std::ofstream;

enum Exper {pulseHeightSpectrum, waveForm, waveFormAverage};

void main()
{
	// Initialise Scope
	lecroy myscope; // Create lecroy class object
	myscope.initialise(); // send ibdev commmand
	
	// Set up Trigger
	myscope.setTrigMode(); // Sets trigger mode to single
	double trig_lev = 0.05; // Scope trigger level
	myscope.setTrigLev(trig_lev); // Set trigger level

	// Scope slope for trigger
	SLOPE t_slope = Pos; // Positive going pulse
	myscope.setTrigSlope(t_slope); // Set trigger slope 

	// Set memory and header
	myscope.setMemorySize(); // sets memory length used for acquisition
	myscope.setCommHeader(); // set response header
	myscope.setFormat(); // set format
	
	// Set time interval
	double time_interval;
	time_interval = myscope.getTimeInt();

	// Get time offset
	double t_offset;
	t_offset = myscope.getTimeOffset();
	
	// Query time base
	myscope.queryTimeBase(); // query timebase setting
	
	// Set voltage per division
	double voltsetting = 0.05; // Scope volts per division
	myscope.setVoltageDiv(voltsetting); // Set volts per division

	// Set vertical offset 
	double v_offset = voltsetting*-4; //voltsetting*3;
	myscope.setVerticalOffset(v_offset); // Set vertical offset
	// double offset = myscope.getVerticalOffset();

	// Arm scope
	myscope.scopeArm();
	// Wait for trigger
	myscope.scopeWait();
	// Set scopr channel
	CHANNEL t_chan = C1;

	Exper my_exper = pulseHeightSpectrum; 

	// Set up scope captures
	int runs = 10000;

	if (my_exper == pulseHeightSpectrum)
	{

		// Array to store values
		double minimum[10000][2];
	
		// open file for data
		char f_name[] = "C:/Users/astewart/Documents/Data/SCINT2015_Data/"
			"data_28p7V_Cs137_m10_bias.txt";
		
		ofstream fout (f_name);
		if (!fout.is_open())
		{
			cout << "Failed to open file...\n";
			exit(1);
		}	
		else
		{
			for (int i = 0; i < runs; i++)
			{
				if (i % 1000 == 0)
				{
					cout << "i = " << i << '\n'; 
				}
			
				// Arm scope and wait for next trigger
				myscope.scopeArm();
				myscope.scopeWait();

				// get pulse amplitude
			
				if (t_slope == Pos)
				{
					minimum[i][0] = myscope.queryMaximum();
				}
				else
				{
					minimum[i][0] = myscope.queryMinimum();
				}
	
				// ignore large pulses			
				if (minimum[i][0] < 0.35)
				{
					minimum[i][1] = myscope.queryRisetime82();
					fout << minimum[i][0] << '\t' << minimum[i][1] << '\n';	
				}	
				else
				{
					i--; 
				}
			
			}
		}
	} 
	else if (my_exper == waveForm)
	{
		char fstart[] = "C:/Users/astewart/Documents/Data/SCINT2015_Data/test_";
		char fend[] = ".txt";
		char filename[255];
		
		int k = 0;

		for (int i =0; i < runs; i++)
		{
			myscope.scopeArm();
			myscope.scopeWait();

			// open file for data
			sprintf_s(filename, "%s%i%s", fstart, k, fend);
			char *pFile = filename;

			myscope.getTrace(t_chan, pFile);

			k++;
		}
	}
	else if (my_exper == waveFormAverage)
	{
		CHANNEL trace = TA; 
		myscope.displayTrace(trace);
		int points = 5000; 
		myscope.averageTraceA(points, runs);

		for (int i =0; i < runs; i++)
		{
			myscope.scopeArm();
			myscope.scopeWait();
		}

		char fname[] = "C:/Users/astewart/Documents/Data/SCINT2015_Data/"
			"pulse_shape_30oC_28p8V.txt";

		ofstream fout (fname);
		if (!fout.is_open())
		{
			cout << "Failed to open file...\n";
			exit(1);
		}	
		else
		{
			t_chan = TA;
			char *pFile = fname;

			myscope.getTrace(t_chan, pFile);
		}

	}
	// t_chan = TA;
	// int j = 1;
	// myscope.grabData(t_chan, j);
	//fout.close();
	
	/*
	char hist_file[] = "C:/Users/astewart/Documents/Data/GAGG_Linearity/hist_30p5V_Cs137_noamp.txt";

	ofstream fout2 (hist_file);
	if (!fout2.is_open())
	{
		cout << "Failed to open file...";
	}
	
	else
	{
		for (int j = 0; j < myhisto.getNumBins(); j++)
		{ 
			// cout << myhisto.binCentre(j) << " , " << myhisto.getCount(j) << endl; 
			if (j == 0)
			{
				fout2 << myhisto.binCentre(j) - myhisto.getWidth()<< "\t" << myhisto.countLowerOutliers() << '\n';
				fout2 << myhisto.binCentre(j) << "\t" << myhisto.getCount(j) << '\n';
			}
			else
			{
				fout2 << myhisto.binCentre(j) << "\t" << myhisto.getCount(j) << '\n';
			}
			if (j == myhisto.getNumBins() -1 )
			{
				fout2 << myhisto.binCentre(j) + myhisto.getWidth() << "\t" << myhisto.countUpperOutliers() << '\n';
			}
			else{}
		}
	}

	fout2.close();

	//char T = "A";
	// myscope.getTrace('A', 1);
	

	char variable_name[] = "C:/Users/astewart/Documents/Data/GAGG_Linearity/variables_29V_Cs137_noamp.txt";
	
	ofstream fout3 (variable_name);
	if (!fout3.is_open())
	{
		cout << "Failed to open file...";
	}
	else
	{
		fout3 << "Detector Bias = " << "\t" << bias << "keV" << '\n';
		fout3 << "Scope Volts/Div = " << "\t" << voltsetting << "V" << '\n';
		fout3 << "Scope Trigger Level = " << "\t" << trig_lev << "V" << '\n';			
		fout3 << "Histogram Minimum = " << "\t" << min << "V" << '\n';
		fout3 << "Histogram Maximum = "	<< "\t" << max << "V" << '\n';
		fout3 << "Number of Histogram Bins = " << "\t" << myhisto.getNumBins() << '\n'; 
		fout3 << "Histogram Bin Width = " << "\t" << myhisto.getWidth() << '\n';
		fout3 << "Number Runs = " << "\t" << runs << '\n';
		fout3 << "Centre First Bin = " << myhisto.binCentre(0) << '\n';
		fout3 << "Lower Centre = " << myhisto.getLowCentre() << '\n';
		fout3 << "Upper Outlier Bin Centre = " << myhisto.getUpperCentre() << '\n';
	}
	*/
}
