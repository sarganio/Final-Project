#include <string>
#include <iostream>
//#include <openssl/rand.h>

#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "Helper.h"


#define SEQ_LEN 4

using std::string;
using std::cout;
using std::endl;

Party::Party(short myID,long input):_id(myID),_input(input){}
void Party::connectToAllParties(string IPs[NUM_OF_PARTIES]) {
	//compute the party's id this party needs to initiate communication and the party's id this party needs to wait for a connection
	unsigned short idToConnect = (_id + 1) % NUM_OF_PARTIES;
	unsigned short idFromConnect = (_id + 2) % NUM_OF_PARTIES;//-1 mod 3 = 2

	//as mentioned abobe only with IPs
	string toIP = Helper::IPCompare(IPs[1], IPs[2]) ^ (_id % 2 == 0) ? IPs[1] : IPs[2];
	string myIP = IPs[0];

	//expend the vector to contain all parties' sockets
	_sockets.resize(NUM_OF_PARTIES);
	
	//toPort - 6200[id + 1] myPort - 6200[id]
	unsigned short toPort = BASE_PORT + idToConnect, myPort = BASE_PORT + _id;
	
	//setup a server socket 
	TcpServer* from =new TcpServer(myPort);
	this->_sockets[idFromConnect] = from;
	from->serve();
	TRACE("Waiting for clients..");

	//setup a client socket
	TcpClient* to = new TcpClient(myIP,myPort,toPort, toIP);
	this->_sockets[idToConnect] = to;

	//check that the sockect to the other parties were created succssesfully
	while (!from->isValid());
	while (!to->isValid());


	getchar();

}
void Party::broadcast(char* msg)const {
	Message bc(SEQ);
	string data = msg;
	bc.setData(data.c_str());
	for (int i = 0; i < NUM_OF_PARTIES - 1; i++) {
		_sockets[i]->writeBuffer(&bc, HEADER_SIZE);
		_sockets[i]->writeBuffer(bc.getData(), bc.getSize());
	}
}
Party::~Party() {
	//delete all the sockets of the party
	while (_sockets.size()) {
		TcpSocket* toFree = _sockets.back();
		//safety check before using delete
		if (toFree) {
			//delete toFree;
			toFree = nullptr;
		}
		_sockets.pop_back();
	}
}
unsigned short Party::getID()const { return this->_id; }