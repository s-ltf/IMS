#include "OCR.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

//constructor
OCR::OCR(string outputFile)
{
	output = outputFile;
}
/*
 *Function: Run
 *Runs the OCR operation
 */
void OCR::Run(string imageName)
{
	//check if the image name contains ".jpeg"
	size_t loc = imageName.find('.');
	if(loc == string::npos)
		image = imageName + ".jpeg";
	else
		image = imageName;

	string command = "./ocr_script.sh " + image+" "+output;
	system(command.c_str());
}

/*
 *Function: Result
 *Return the result of OCR operation as a string
 */
string OCR::Result()
{
	string data;
	string file = output + ".txt";
	//create ifstream to read data from file
	ifstream myfile;
	myfile.open(file.c_str());
	if(myfile.is_open())
	{
		while(getline(myfile,data))
		{
			cout << data << "\n";
		}
		myfile.close();
	}
	else
		cout<< "Unable to open the file";

	return data;
}
