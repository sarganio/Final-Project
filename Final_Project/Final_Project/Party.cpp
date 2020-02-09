#include <string>
#include <iostream>
#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"

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
	
	//toPort - 61002 myPort - 61001
	unsigned short toPort = BASE_PORT + idToConnect, myPort = BASE_PORT + _id;
	
	////setup a server socket 
	TcpServer from = TcpServer(myPort);
	from.serve();
	char buffer[5] = { 0 };
	int test = from.readBuffer(buffer, 4);
	while (TRUE) {
		from.readBuffer(buffer, 4);
		cout << buffer << endl;
	}
	getchar();
	//setup a client socket
	////TcpClient to = TcpClient(65000,"192.168.43.241");
	//TcpClient to = TcpClient(myPort,toPort, "127.0.0.1");
	//char buffer[5] = { '1','2','3','4',0 };
	//to.writeBuffer(buffer,5);


}