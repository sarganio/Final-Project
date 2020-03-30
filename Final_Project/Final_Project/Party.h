#pragma once
#include <vector>
#include <string>
#include <mutex>


#include "Messages.h"
#include "TcpSocket.h"

using std::vector;
using std::pair;
using std::string;
using std::mutex;

class Party
{
private:
	unsigned short _id;//unique identifier, lowest ip receive id = 1 and so on. 
	long _input;//secret input 
	unsigned char _seq[SEQ_LEN];
	vector<TcpSocket*> _sockets;//socket for all TCP connection. sockets[0] is a UDP soket
	vector<pair<long, long>> _shares;//index of vector is the id of input's party
	vector<Message*>_msgs;
	//vector<mutex> _mtx;
public:
	Party(short myID, long input);//C'tor takes an ID and a secret input as parameters
	bool sendTo(unsigned short id, unsigned short messageType, void* msg);//send message to party[id]
	void readFrom(unsigned short id, char* msg);//read message from party number id. this is a blocking function.
	//bool sendTo(unsigned short id, void* msg);//send a message to a party number id
	void connectToAllParties(string IPs[NUM_OF_PARTIES]);//connect to all 3 parties
	void broadcast(void * msg, unsigned short messageType)const;//send message to all parties connected to this party
	unsigned short getID()const;
	void fInput();
	~Party();

};

