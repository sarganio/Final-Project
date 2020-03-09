#include <string>
#include <iostream>
#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include <openssl/rand.h>


#define SEQ_LEN 4

using std::string;
using std::cout;
using std::endl;

Party::Party(short myID,long input):_id(myID),_input(input){}
void Party::connectToAllParties(string IPs[NUM_OF_PARTIES]) {

	unsigned short idToConnect = (_id + 1) % NUM_OF_PARTIES;
	string toIP = max(IPs[1], IPs[2]);
	string myIP = IPs[0];
	
	//toPort - 6200[id + 1] myPort - 6200[id]
	unsigned short toPort = BASE_PORT + idToConnect, myPort = BASE_PORT + _id;
	
	//setup a server socket 
	TcpServer* from =new TcpServer(myPort);
	this->_sockets.push_back(from);
	from->serve();
	cout << "Waiting for clients.." << endl;
	//setup a client socket
	TcpClient* to = new TcpClient(myIP,myPort,toPort, toIP);
	this->_sockets.push_back(to);
	cout << "Sent a message forward" << endl;

	getchar();

}
void Party::broadcast(char* msg)const {
	for (int i = 0; i < NUM_OF_PARTIES - 1; i++)
		_sockets[i]->writeBuffer(msg, strlen(msg));
}
Party::~Party() {
	int i = 0;
	//delete all the sockets of the party
	while (_sockets.size()) {
		TcpSocket* toFree = _sockets.back();
		//safety check before using delete
		if(toFree)
			delete _sockets.back();
		_sockets.pop_back();
	}
}