#include <string>
#include <iostream>
#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include <openssl/rand.h>
#include "Helper.h"


#define SEQ_LEN 4

using std::string;
using std::cout;
using std::endl;

Party::Party(short myID,long input):_id(myID),_input(input){}
void Party::connectToAllParties(string IPs[NUM_OF_PARTIES]) {

	unsigned short idToConnect = (_id + 1) % NUM_OF_PARTIES;
	string toIP = Helper::IPCompare(IPs[1], IPs[2]) ^ (_id % 2 == 0) ? IPs[1] : IPs[2];
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
	for (int i = 0; i < NUM_OF_PARTIES - 1; i++) {
		string messge = "This is a message from" + _id;
		_sockets[i]->writeBuffer(messge.c_str(),messge.size());
	}
	cout << "Sent a messages forward" << endl;

	getchar();

}
void Party::broadcast(char* msg)const {
	for (int i = 0; i < NUM_OF_PARTIES - 1; i++)
		_sockets[i]->writeBuffer(msg, strlen(msg));
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