#pragma once
#include <vector>
#include <string>
#include "Messages.h"
#include "TcpSocket.h"

using std::vector;
using std::pair;
using std::string;

class Party
{
private:
	short _id;//unique identifier, lowest ip receive id = 1 and so on. 
	long _input;//secret input 
	char _fromSeq[SEQ_LEN];
	vector<TcpSocket*> _sockets;//socket for all TCP connection. sockets[0] is a UDP soket
	vector<pair<long, long>> _shares;//index of vector is the id of input's party
public:
	Party(short myID,long input);
	void connectToAllParties(string IPs[NUM_OF_PARTIES]);
	void broadcast(char * msg)const;
	~Party();

};

