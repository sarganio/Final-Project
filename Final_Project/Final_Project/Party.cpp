#include <string>
#include <iostream>
#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"
//#include <ws2tcpip.h>

using std::string;
using std::cout;
using std::endl;

Party::Party(short id,long input):_id(id),_input(input){}
void Party::connectToAllParties() {
	//WSAInitializer wsainit;

	short idToConnect = (_id + 1) % NUM_OF_PARTIES;
	//string toIP = BASE_IP + std::to_string(idToConnect);
	//string myIP = BASE_IP + std::to_string(_id);
	//myIP = "192.168.43.41";
	string IP = "127.0.0.1";
	string toIP = "192.168.43.241";
	string myIP = "192.168.43.41";
	
	//toPort - 62001 myPort - 62000
	unsigned short toPort = BASE_PORT + idToConnect, myPort = BASE_PORT + _id;
	
	////setup a server socket 
	TcpServer from = TcpServer(myPort);
	from.serve();
	cout << "Waiting for clients.." << endl;
	//setup a client socket
	//TcpClient to = TcpClient(65000,"192.168.43.241");
	TcpClient to = TcpClient(myIP,myPort,toPort, toIP);
	cout << "Sent a message forward" << endl;
	getchar();

}