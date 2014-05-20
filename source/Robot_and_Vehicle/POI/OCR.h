#ifndef OCR_H
#define OCR_H

#include <iostream>
#include <string.h>

using namespace std;
class OCR
{
	private:
		string image;
		string output;
	public:
		OCR(string);
		void Run(string);
		string Result();
};

#endif
