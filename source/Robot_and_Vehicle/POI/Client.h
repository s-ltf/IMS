#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

using namespace std;

class Client
{
	public:
		Client(const char*, int);
		void Send(const char*);
		string Receive();
};
#endif
