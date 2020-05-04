#include <cstring>
#include <sstream>
#include "TcpClient.h"
#include <iostream>
#include "Messages.h"
#include "Helper.h"
#include <thread>
#define IP_INDEX 1
using std::thread;
TcpClient::TcpClient(string myIP,unsigned short myPort, unsigned short hostPort, std::string hostIp, Message* mess,bool& isConnected) :TcpSocket(-1, hostPort)//, mutex& m):TcpSocket(-1,myPort)
{
	_hostAddress = hostIp;

	//struct sockaddr_in myAddr;
	// //Explicitly assigning port number by 
	// //binding client with that port  
	//myAddr.sin_family = AF_INET;
	//myAddr.sin_addr.s_addr = INADDR_ANY;
	//myAddr.sin_port = htons(myPort);
	//myAddr.sin_addr.s_addr = inet_addr(myIP.c_str());

	//if (bind(_socket, (struct sockaddr*) & myAddr, sizeof(struct sockaddr_in)) != 0)
	//	throw std::exception("Client bind failed (port assighnment)");

	_t = new thread(&TcpClient::connect, this, hostPort, hostIp, mess,std::ref(isConnected));// , m);
	_t->detach();
}

int TcpClient::connect(unsigned short hostPort, std::string hostname, Message* mess,bool& isConnected)//, mutex& m)
{
	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(hostPort); // port that server will listen to
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = inet_addr(hostname.c_str());    // the IP of the server

	int status,i = 0;
	while (status = ::connect(_socket, (struct sockaddr*) & sa, sizeof(sa))) {
		TRACE("ID to connect %d: Attempt #%d", hostPort - BASE_PORT, ++i);
		Sleep(500);
	}

	if (status == INVALID_SOCKET)
		throw std::exception("Cant connect to server");
	cout << "Connected to server! id=" << ((hostPort-BASE_PORT) + 1) % NUM_OF_PARTIES << endl;
	isConnected = true;
	messagesHandler(mess);//,m);

	return _socket;
}