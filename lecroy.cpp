
#include "StdAfx.h"
#include "lecroy.h"
#include "ni4882.h"
#include "visa.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>

using std::ofstream;

// class constructor
lecroy::lecroy(void)
{
}

// class destructor
lecroy::~lecroy(void)
{
	// cout << "Destructor called ...";

}

// get IDN? response

void lecroy::initialise()
{
	SendIFC(0); // clear the bus
	if (ibsta & ERR)
	{
		cout << "Could not send IFC" << endl;
	}

	m_device = ibdev(0,9,0,16,1,0);
	if (m_device == -1){
		cout << "Error connecting to Oscilloscope!" << endl;
	}
	else if (m_device < 0){
		cout << "Error connecting to Oscilloscope!" << endl;
	}

	// cout << "Device ID = " << m_device << endl; 
	queryTimeInterval();
	queryTimeOffset();
}

void lecroy::sendCommand(char *command)
{
	ibwrt(m_device, command, strlen(command));
}

void lecroy::get_idn()
{
	char command[] = "*IDN?";
	char response[100];
	sendCommand(command);
	ibrd(m_device,response,100);
	response[ibcnt] = '\0';
}

void lecroy::setTrigSlope(SLOPE  t_slope)
{
	char command[50];
	if (t_slope == Pos)
	{		
		sprintf_s(command, "C1:TRSL POS"); // Options: POS, NEG, WINDOW
	}
	else if (t_slope == Neg)
	{
		sprintf_s(command, "C1:TRSL NEG");
	}
	else if (t_slope == Window)
	{
		sprintf_s(command, "C1:TRSL WINDOW");
	}	
	else{}
	sendCommand(command);
}

void lecroy::queryTrigSlope()
{
	char command[] = "TRig_SLope?"; // Query trigger slope
	char response[100];
	sendCommand(command);
	ibrd(m_device,response,100);
	response[ibcnt] = '\0';
	// cout << response << endl; 
}

void lecroy::clearScope()
{
	char command[] = "*CLS";
	ibwrt(m_device,command,strlen(command));
}

void lecroy::queryVolt()
{
	char command[] = "C1:Volt_Div?";
	char xresponse[250];
	sendCommand(command);
	ibrd(m_device, xresponse, 250);
	xresponse[ibcnt] = '\0';
	// cout << xresponse << endl;
}

void lecroy::displayTrace(CHANNEL C)
{
	char command[] = "TA:TRA ON";
	sendCommand(command);
}

void lecroy::averageTraceA(int points, int sweeps)
{
	//char command1[] = "TA:TRA ON";
	//sendCommand(command1);
	char command[50];
	// char str1[] = "TA:DEF EQN,'AVGS(C1)',MAXPTS,5000,SWEEPS,10";
	sprintf_s(command, "%s%i%s%i", "TA:DEF EQN,'AVGS(C1)',MAXPTS,", points, ",SWEEPS, ", sweeps);
	// cout << command << endl; 
	sendCommand(command);
}

void lecroy::setVoltageDiv(double volt_div)
{
	char str1[] = "C1:Volt_Div ";
	char command[50];
	sprintf_s(command, "%s%e", str1, volt_div);
	sendCommand(command);
}

void lecroy::setTrigMode()
{
	char command[] = "TRMD SINGLE"; // Options AUTO, NORM, SINGLE, STOP
	sendCommand(command);
}

void lecroy::queryTrigMode()
{
	char command[] = "TRMD?";
	char response[50];
	sendCommand(command);
	ibrd(m_device, response, 100);
	response[ibcnt] = '\0';
	// cout << response << endl;
}

void lecroy::setTrigLev(double lev)
{
	char str1[] = "C1:TRLV ";
	char command[50];
	sprintf_s(command, "%s%e", str1, lev);
	sendCommand(command);
}

void lecroy::setTimeBase(double tdiv)
{
	char str1[] = "TDIV ";
	char command[50];
	sprintf_s(command, "%s%e", str1, tdiv);
	sendCommand(command);
}

void lecroy::queryTimeBase()
{
	char command[] = "Time_DIV?"; // This is the timebase setting on the scope
	char response[100];
	sendCommand(command);
	ibrd(m_device, response, 100);
	response[ibcnt] = '\0';
	// cout << "Time Div : " << response << endl;
}

void lecroy::queryTimeInterval()
{
	char command[] = "C1:INSPECT? \"HORIZ_INTERVAL\"";
	char response[100];
	sendCommand(command);
	ibrd(m_device, response, 100);
	response[ibcnt] = '\0';
	// cout << "Time Interval = " << response << endl;
	timeInt = rtnData(response);
	
}

double lecroy::rtnData(char * resp)
{
	int i = 0;
	char temp[20];

	while (!isdigit(resp[i]) || (i > 50)) // find first digit in char array (infinite loop - set i to max 50)
	{
		i++;
	}
	int j = 0;
	if (isdigit(resp[i])) // if resp is a digit then...
	{
		if (resp[i-1] == '-') // needed to catch minus sign before first digit
		{
			--i;
			while (!isspace(resp[i])) // write digits to temp (until a space is found)
			{
				temp[j] = resp[i]; 
				j++;
				i++;
			}
		}
		else
		{
			while (!isspace(resp[i])) // write digits to temp (until a space is found)
			{
				temp[j] = resp[i]; 
				j++;
				i++;
			}
		}
		temp[j] = '\0';
		// cout << "Temp = " << temp << endl;
		return atof(temp); // convert temp to double and return value
	}
	else
	{
		cout << "Error : No digit found!" << endl; 
		return -99999; // What to return if function fails!! 
	}
}

void lecroy::queryTimeOffset()
{
	char command[] = "C1:INSPECT? \"HORIZ_OFFSET\"";
	char response[255];
	sendCommand(command);
	ibrd(m_device, response, 255);
	response[ibcnt] = '\0';
	// cout << "Time Offset : " << response << endl;
	timeOffset = rtnData(response);
}

double lecroy::getTimeOffset()const
{
	// cout << "TimeOffset = " << timeOffset << endl;
	return timeOffset; 
}

void  lecroy::queryFormat()
{
	char command[] = "CFMT?";
	char response[100];
	sendCommand(command);
	ibrd(m_device, response, 100);
	response[ibcnt] = '\0';
	cout << "Format : " << response << endl;
}

void lecroy::setFormat()
{
	//char command[] = "CFMT DEF9,WORD,BIN"; // Indefinite length arbitrary block response
	char command[] = "CFMT IND0,BYTE,BIN";
	sendCommand(command);
}

void lecroy::scopeWait()
{
	char command[] = "WAIT"; // prevents scope from analysing new commands until the current acquisition is complete
	sendCommand(command);
	// cout << "Waiting ....." << endl;
}

void lecroy::scopeArm()
{
	char command[] = "ARM"; 
	sendCommand(command);
}

void lecroy::setMemorySize()
{
	char command[] = "MSIZ 1M"; // Allows the selection of the maximum memory length used for acquisition
	sendCommand(command); // Options 500, 1K, 2.5K, 5K, 10K, 25K, 50K, 100K, 250K, 500K, 1M, 2.5M, 5M, 10M
}

void lecroy::queryMemorySize()
{
	char command[] = "MSIZ?";
	char response[50];
	sendCommand(command);
	ibrd(m_device,response,50);
	response[ibcnt] = '\0';
	cout << "Memory Size : " << response << endl;
}

void lecroy::setCommHeader()
{
	char command[] = "CHDR OFF";
	sendCommand(command); // Turn off response header. Options: OFF, SHORT, LONG
}

void lecroy::getTrace(CHANNEL t_chan, char *pFile)
{
	char command[55];

	if (t_chan == C1)
	{
		sprintf_s(command, "C1:INSPECT? \"SIMPLE\"");
	}
	else if (t_chan == C2)
	{
		sprintf_s(command, "C2:INSPECT? \"SIMPLE\"");
	}
	else if (t_chan == C3)
	{
		sprintf_s(command, "C3:INSPECT? \"SIMPLE\"");
	}
	else if (t_chan == C4)
	{
		sprintf_s(command, "C4:INSPECT? \"SIMPLE\"");
	}
	else if (t_chan == TA)
	{
		sprintf_s(command, "TA:INSPECT? \"SIMPLE\"");
	}

	sendCommand(command);
	char * response = new char[500000];
	ibrd(m_device, response, 500000); 
	response[ibcnt] = '\0';

	genWaveform(response, pFile);
	delete [] response;
}

void lecroy::grabData(CHANNEL t_chan, int j)
{
	char command[21];
	char * response = new char[500000];

	if (t_chan == C1)
	{
		sprintf_s(command, "C1:INSPECT? \"SIMPLE\"");
	}
	else if (t_chan == C2)
	{
		sprintf_s(command, "C2:INSPECT? \"SIMPLE\"");
	}
	else if (t_chan == C3)
	{
		sprintf_s(command, "C3:INSPECT? \"SIMPLE\"");
	}
	else if (t_chan == C4)
	{
		sprintf_s(command, "C4:INSPECT? \"SIMPLE\"");
	}
	else if (t_chan == TA)
	{
		sprintf_s(command, "TA:INSPECT? \"SIMPLE\"");
	}

	// cout << "Command = " << command; 
	sendCommand(command);
	ibrd(m_device, response, 500000); // read response from oscilloscope
	response[ibcnt] = '\0'; 

	// saveData(response,j);
	genWaveform(response, j); // convert scope data into waveform array
	delete [] response; //  free memory on heap
}


void lecroy::genWaveform(char * wf, char * pFile)
{
	timebase.resize(1000);
	amplitude.resize(1000); 

	ofstream fout (pFile);

	if (!fout.is_open())
	{
		cout << "Failed to open file ...\n";
	}
	else
	{
		convertWaveform(wf);
	}

	for (int b = 0; b < timebase.size(); b++)
	{
		fout << timebase[b] << '\t' << amplitude[b] << '\n'; // write data to text file
	}

	fout.close();
}

void lecroy::genWaveform(char * wf, int k)
{
	char * char_data = new char[150000]; // new char array for saving data

	timebase.resize(1000);
	amplitude.resize(1000);

	// char filename[] = "C:/Users/astewart/Python Scripts/scope_plot/LuAG_test.txt";
	// char filename[] = "\\\\afs\\phas.gla.ac.uk\\user\\a\\astewart\\Scope_Pulse\\scope_pulse_shape.txt";
	// char filename[] = "C:/Users/astewart/Documents/Data/IEEE Paper Data"
	char fstart[] = "C:/Users/astewart/Documents/Data/SCINT2015_Data/decay_5oC_";
	// char fstart[] = "\\\\afs\\phas.gla.ac.uk\\user\\a\\astewart\\Scope_Pulse\\test_Cs137_";
	//char fstart[] = "U:/Scope_Pulse/test_Cs137_";
	char fend[] = ".txt";
	char filename[255];
	sprintf_s(filename, "%s%i%s", fstart, k, fend);
	
	ofstream fout (filename); //openFile(filename);

	if (!fout.is_open()){
		cout << "Failed to open file...";
	}
	else
	{
		convertWaveform(wf);
	}

	
	for (int b = 0; b < timebase.size(); b++)
	{
		fout << timebase[b] << '\t' << amplitude[b] << '\n'; // write data to text file
	}
	
	// sigInt = trapz(x_data, y_data); // Integrate scope signal
	// sigMin = findmin(y_data); // Find minimum (peak) of scope signal

	fout.close();
	
	delete [] char_data; // 
}

void lecroy::convertWaveform(char * waveform)
{
	double t_interval = getTimeInt();
	double t_offset = getTimeOffset();

	int m = 1;
	int n = 0;
	int i = 0;
	char temp[13];

	while (waveform[m] != '\"')
		{
			int h = 0;
			if (!isspace(waveform[m]) && waveform[m] != '\"')
			{
				while (isspace(waveform[m])== 0)
				{
					temp[h] = waveform[m];
					m++;
					n++;
					h++;
				}
			temp[h] = '\0';
			if (i < 1000)
			{
				amplitude[i] = atof(temp);
				timebase[i] = t_interval * i + t_offset; 
			}
			else
			{
				amplitude.push_back(atof(temp));
				timebase.push_back(t_interval * i + t_offset);
			}
			
			i++;
			n++;
			}
			else
			{
				m++;
			}
		}
}

double lecroy::getSigInt() const
{
	return sigInt; 
}

void lecroy::setNrows(int numrows)
{
	nrows = numrows;
}

double lecroy::getNrows()
{
	return nrows; 
}

void lecroy::queryVerticalOffset()
{
	char command[] = "C1:OFST?";
	char response[100];
	sendCommand(command);
	ibrd(m_device, response, 100);
	response[ibcnt] = '\0';
	// cout << "Vertical Offset = " << response << endl;
	verticalOffset = atof(response);
}

void lecroy::setVerticalOffset(double v_offset)
{
	char str[] = "C1:OFST ";
	char command[50];
	sprintf_s(command, "%s%e", str, v_offset);
	sendCommand(command);
	queryVerticalOffset(); 
}

double lecroy::getVerticalOffset() const
{
	return verticalOffset; 
}

double lecroy::getSigMin() const
{
	return sigMin;
}

void lecroy::queryWaveform()
{
	char command[] = "C1:WF? DAT1";
	char * response = new char[100000];
	sendCommand(command);
	ibrd(m_device, response, 100000);
	response[ibcnt] = '\0';
	for (int i = 0; i < 20; i++){
		if (i == 0){
			cout << "WAVEFORM: " << response[i] << endl;
		}
		else {
			cout << response[i] << endl;
		}
	}
}

double lecroy::queryAmplitude() 
{
	char command[] = "C1:PAVA? AMPL";
	char response[255];
	sendCommand(command);
	ibrd(m_device,response,255);
	response[ibcnt] = '\0';
	double amp = rtnData(response);
	return amp;
	// cout << "Amplitude = " << amp << endl;
}

double lecroy::queryMaximum()
{
	char command[] = "C1:PAVA? MAX";
	char response[255];
	sendCommand(command);
	ibrd(m_device,response,255);
	response[ibcnt] = '\0';
	double max = rtnData(response);
	return max;
}

double lecroy::queryMinimum()
{
	char command[] = "C1:PAVA? MIN";
	char response[255];
	sendCommand(command);
	ibrd(m_device,response,255);
	response[ibcnt] = '\0';
	double min = rtnData(response);
	// cout << "rtnData gives = " << response << endl;
	// cout << "Minimum is " << min << endl;
	return min;
}

double lecroy::queryFalltime()
{
	char command[] = "C1:PAVA? FALL";
	char response[255];
	sendCommand(command);
	ibrd(m_device,response,255); 
	response[ibcnt] = '\0';
	double fall = rtnData(response);
	return fall;
}

double lecroy::queryRisetime82()
{
	char command[] = "C1:PAVA? RISE28";
	char response[255];
	sendCommand(command);
	ibrd(m_device, response, 255);
	response[ibcnt] = '\0';
	int i = 0;
	char temp[20];
	while (response[i] != ',')
	{
		i++;
	}
	i++;
	// cout << "i = " << i << '\n';
	int j = 0;
	while (response[i] != ',')
	{
		temp[j] = response[i];
		j++;
		i++;
	}
	temp[j] = '\0';
	return atof(temp);
}

double lecroy::queryFalltime82()
{
	char command[] = "C1:PAVA? FALL82";
	char response[255];
	sendCommand(command);
	ibrd(m_device,response,255);
	response[ibcnt] = '\0';
	// cout << response;
	int i = 0;
	char temp[20];
	while (response[i] != ',')
	{
		i++;
	}
	i++;
	// cout << "i = " << i << '\n';
	int j = 0;
	while (response[i] != ',')
	{
		temp[j] = response[i];
		j++;
		i++;
	}
	temp[j] = '\0';
	// cout << "Temp = " << temp << '\n';
	return atof(temp);
}

void lecroy::saveData(char * data, int j)
{
	// char fstart[] = "\\\\afs\\phas.gla.ac.uk\\user\\a\\astewart\\Scope_Pulse\\raw_data_new_";
	// char fstart[] = "C:/Users/astewart/Python Scripts/scope_plot/pull_hex_data_";
	char fstart[] = "C:/Users/astewart/Documents/Data/invention_report_data/gagg_spm_";
	char fend[] = ".txt";
	char filename[255];
	sprintf_s(filename, "%s%i%s", fstart, j, fend);
	ofstream fout (filename);
	if (!fout.is_open()){
		cout << "Failed to open file...";
	}
	else{
		fout << data;
	}
	fout.close();
}

void lecroy::saveData(char * filename, char * data) // overload saveData function
{
	ofstream fout (filename);
	if (!fout.is_open()){
		cout << "Failed to open file...";
	}
	else{
		// cout << "Dereference pointer to data : " << &data << endl;
		fout << data;
	}
	fout.close();
}

void lecroy::queryMax()
{
	char command[] = "C1:PAVA? MAX";
	char response[50];
	sendCommand(command);
	ibrd(m_device, response, 50);
	response[ibcnt] = '\0';
	cout << "Max = " << response << endl;
}

void lecroy::queryTrigLev()
{
	char command[] = "TRLV?";
	char response[50];
	sendCommand(command);
	ibrd(m_device, response, 50);
	response[ibcnt] = '\0';
	// cout << "Trig Lev = " << response << endl;
}

void lecroy::queryDesc()
{
	char command[] = "C1:INSP? \"WAVEDESC\"";
	char * response = new char[10000];
	sendCommand(command);
	ibrd(m_device, response, 10000);
	response[ibcnt] = '\0';
	// cout << "Wave Description : " << response << endl;
	
	char * filename = "C:/Users/astewart/Python Scripts/scope_plot/wave_desc.txt";
	saveData(filename, response); 
}

double lecroy::getTimeInt() const
{
	return timeInt;
}

