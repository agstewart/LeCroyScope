
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "trapz.h"
#include "findmin.h"
#include <vector>

// #pragma once

using std::cout;
using std::endl;

enum SLOPE {Pos, Neg, Window};
enum CHANNEL {C1, C2, C3, C4, TA, TB, TC, TD};

class lecroy
{
public:
	lecroy(void);
	~lecroy(void);

	void get_idn();
	void initialise();
	void clearScope();
	void sendCommand(char *command);
	void grabData(CHANNEL t_chan, int j); // get data from scope
	void getTrace(CHANNEL t_chan, char *pFile); // Get Trace waveform
	void saveData(char * data, int j); // save data to text file
	void saveData(char * filename, char * data); // save data to text file with filename as argument
	void scopeWait(); // prevents instrument analysing new commands until acquisition is complete
	void scopeArm(); // Start a new data acquisition 
	void setCommHeader(); // sets scope response format, i.e. turn off the header
	void setMemorySize();
	void setFormat();
	void genWaveform(char * wf, int k); // generate waveform
	void genWaveform(char * wf, char * pFile); // take scope data and generate waveform
	void setNrows(int numrows); // set the value of n rows
	double getNrows(); // get the number of rows (nrows) written to arrays
	void averageTraceA(int points, int sweeps);
	void displayTrace(CHANNEL C);


	void queryTrigSlope();
	void queryVolt();
	void queryTrigMode();
	void queryTimeBase();
	void queryMemorySize();
	void queryWaveform(); // query the waveform
	void queryTimeInterval(); // query the Time interval
	void queryTimeOffset(); // query Time Offset
	void queryFormat(); // query the format
	void queryMax(); // query the maximum recorded amplitude
	void queryTrigLev(); // query the trigger level
	void queryDesc(); // query the wave description
	void queryVerticalOffset(); // query vertical offset
	double queryAmplitude(); // query waveform amplitude
	double queryMinimum(); // query waveform minimum
	double queryMaximum(); // query waveform maximum
	double queryFalltime(); // query pulse falltime
	double queryFalltime82(); // query pulse fallltime 80% to 20%
	double queryRisetime82(); // query pulse risetime 20% to 80%
	
	// functions to set up oscilloscope
	void setTrigMode();
	void setTrigLev(double lev);
	void setTimeBase(double tdiv);
	void setVerticalOffset(double v_offset);
	void setVoltageDiv(double volt_div);
	void setTrigSlope(SLOPE t_slope);

	double getTimeInt() const; // get time interval member variable
	double getTimeOffset() const;
	double getSigInt() const;
	double getSigMin() const;
	double getVerticalOffset() const;
	double rtnData(char * resp); // return data value from scope response
	
	void openFile(char filename); // open text file for writing
	void convertWaveform(char * waveform);

protected : 
	int m_device; // instrument identifier
	int nrows; // number of rows written to data arrays
	double timeInt; // time interval
	double timeOffset; // time offset
	double verticalOffset; // Vertical offset
	double sigInt; // Integral of scope signal
	double sigMin; // Minimum of scope signal
	std::vector<double> timebase;
	std::vector<double> amplitude;

};
