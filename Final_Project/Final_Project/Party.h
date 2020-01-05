#pragma once
#include <vector>
#include <WinSock2.h>
#include <Windows.h>

using std::vector;
using std::pair;

class Party
{
private:
	short id;//unique identifier, lowest ip receive id = 1. 
	long input;//secret input 
	vector<SOCKET> sockets;//socket for all TCP connection. sockets[0] is a UDP soket
	vector<pair<long, long>> shares;//index of vector is the id of input's party
public:
	

};

