#include <string>
#include <iostream>
#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"
//#include <openssl/rand.h>
#include "Helper.h"


#define SEQ_LEN 4

using std::string;
using std::cout;
using std::endl;

Party::Party(short myID,long input):_id(myID),_input(input){}
void Party::connectToAllParties(string IPs[NUM_OF_PARTIES]) {

	unsigned short idToConnect = (_id + 1) % NUM_OF_PARTIES;
	unsigned short idFromConnect = (_id + 2) % NUM_OF_PARTIES;
	string toIP = Helper::IPCompare(IPs[1], IPs[2]) ^ (_id % 2 == 0) ? IPs[1] : IPs[2];
	string myIP = IPs[0];
	_sockets.resize(NUM_OF_PARTIES);
	
	//toPort - 6200[id + 1] myPort - 6200[id]
	unsigned short toPort = BASE_PORT + idToConnect, myPort = BASE_PORT + _id;
	
	//setup a server socket 
	TcpServer* from =new TcpServer(myPort);
	this->_sockets[idFromConnect] = from;
	from->serve();
	cout << "Waiting for clients.." << endl;

	//setup a client socket
	TcpClient* to = new TcpClient(myIP,myPort,toPort, toIP);
	this->_sockets[idToConnect] = to;
	while (!from->isValid());
	int IDtoSend,typeToSend;
	while (true) {
		cout << "ID to send:";
		std::cin >> IDtoSend;
		//IDtoSend--;
		cout << "Type to send:";
		std::cin >> typeToSend;
		string data;
		Message toSend(typeToSend);
		switch (typeToSend) {
		case SEQ:
			data = "987" + std::to_string(_id);
			break;
		case KEY:
			data = "1011121314151617181920212223242" + std::to_string(_id);
			break;
		case RECONSTRUCT:
			data = "10111213141516171819202122232421011121314151617181920212223242" + std::to_string(_id);
			break;
		default:
			break;
		}
		std::reverse(data.begin(), data.end());
		toSend.setData(data.c_str());
		_sockets[IDtoSend]->writeBuffer(&toSend, HEADER_SIZE);
		_sockets[IDtoSend]->writeBuffer(toSend.getData(), toSend.getSize());
	}
	cout << "Sent all messages" << endl;
	//for (int i = 0; i < NUM_OF_PARTIES - 1; i++) {
	//	char buff[100];
	//	if (_sockets[i]->readBuffer(buff, 24) == -1)
	//		i--;
	//}
	//cout << "Sent a messages forward" << endl;

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
unsigned short Party::getID()const { return this->_id; }