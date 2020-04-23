#pragma once
#include <vector>
#include <string>
#include <mutex>


#include "Messages.h"
#include "TcpSocket.h"
#include "Helper.h"
#include "Share.h"

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
	byte _finalSeq[SEQ_LEN]{};
	vector<TcpSocket*> _sockets;//socket for all TCP connection. sockets[0] is a UDP soket
	vector<Message*>_msgs;
	vector<SecByteBlock*>_keys;
	vector<Share*> _shares;//index of vector is the id of input's party
	void printKey(unsigned short index)const;
public:
	Party(short myID, long input);//C'tor takes an ID and a secret input as parameters
	bool sendTo(unsigned short id, unsigned short messageType, byte* msg)const;//send message to party[id]
	void readFrom(unsigned short id,unsigned char* msg);//read message from party number id. this is a blocking function.
	//bool sendTo(unsigned short id, void* msg);//send a message to a party number id
	void connectToAllParties(string IPs[NUM_OF_PARTIES]);//connect to all 3 parties
	void broadcast(byte * msg, unsigned short messageType)const;//send message to all parties connected to this party
	unsigned short getID()const;
	void calcSeq();
	Share* fRand();
	void fInput();
	long reconstruct(vector<Share*>& myShare);
	~Party();

};

