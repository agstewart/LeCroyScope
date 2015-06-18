
#include "stdafx.h"
#include <string>
#include "string.h"
#include "pop_string.h"

//using std::cout;
//using sdt::endl;

string pop_string(string &s, char delim)
{
	int pos_delim = s.find_first_of(delim); // get position of delimiter
	
	if (pos_delim == string::npos) // check if delim appeared
	{
		return s;
	}

	string popped = s.substr(0, pos_delim);

	s = s.substr(pos_delim + 1); // remove the first part of the string

	return popped; // return the fist piece
}