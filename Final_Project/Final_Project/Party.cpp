#include <string>
#include <iostream>
#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"

using std::string;
using std::cout;
using stk::TcpClient;
using stk::TcpServer;
Party::Party(short id,long input):_id(id),_input(input){}
void Party::connectToAllParties() {
	short idToConnect = (_id + 1) % NUM_OF_PARTIES;
	string toIP = BASE_IP + std::to_string(idToConnect);
	string myIP = BASE_IP + std::to_string(_id);

	short toPort = BASE_PORT + idToConnect, myPort = BASE_PORT + _id;
	
	TcpServer from = TcpServer(myPort);
	from.accept();
	TcpClient to = TcpClient(toPort);
	to.connect(toPort,toIP);
	


}