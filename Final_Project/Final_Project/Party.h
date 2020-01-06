#pragma once
#include <vector>
#include "Messages.h"

using std::vector;
using std::pair;

class Party
{
private:
	short _id;//unique identifier, lowest ip receive id = 1. 
	long _input;//secret input 
	//vector<SOCKET> _sockets;//socket for all TCP connection. sockets[0] is a UDP soket
	vector<pair<long, long>> _shares;//index of vector is the id of input's party
public:
	void connectToAllParties();

};

