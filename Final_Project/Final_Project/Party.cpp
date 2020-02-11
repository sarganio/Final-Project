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

Party::Party(int myID,long input):_id(myID),_input(input){}
void Party::connectToAllParties(string IPs[NUM_OF_PARTIES]) {

	short idToConnect = (_id + 1) % NUM_OF_PARTIES;
	string toIP = max(IPs[1], IPs[2]);
	string myIP = IPs[0];
	
	//toPort - 6200[id + 1] myPort - 6200[id]
	unsigned short toPort = BASE_PORT + idToConnect, myPort = BASE_PORT + _id;
	
	//setup a server socket 
	TcpServer from = TcpServer(myPort);
	this->_sockets.push_back(from.socketFd());
	from.serve();
	cout << "Waiting for clients.." << endl;
	//setup a client socket
	TcpClient to = TcpClient(myIP,myPort,toPort, toIP);
	this->_sockets.push_back(to.socketFd());
	cout << "Sent a message forward" << endl;

	getchar();

}