#include <string>
#include <iostream>
#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"

using std::string;
using std::cout;
using std::endl;
using stk::TcpClient;
using stk::TcpServer;
Party::Party(short id,long input):_id(id),_input(input){}
void Party::connectToAllParties() {
	short idToConnect = (_id + 1) % NUM_OF_PARTIES;
	string toIP = BASE_IP + std::to_string(idToConnect);
	string myIP = BASE_IP + std::to_string(_id);
	myIP = "192.168.43.41";
	
	short toPort = BASE_PORT + idToConnect, myPort = BASE_PORT + _id;
	
	TcpServer from = TcpServer(65000);
	from.accept();
	char buffer[20];
	from.readBuffer(buffer, 20);
	cout << buffer << endl;
	TcpClient to = TcpClient(65010);
	to.connect(65001, "192.168.43.241");
	


}