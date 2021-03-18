#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TcpClient.h"
#include "Messages.h"
#include "Helper.h"
#include <cstring>
#include <sstream>
#include <iostream>
#include <thread>

#define IP_INDEX 1
using std::thread;

TcpClient::TcpClient(string myIP,unsigned short myPort, unsigned short hostPort, std::string hostIp, Message* mess,bool& isConnected) :TcpSocket(-1, hostPort)//, mutex& m):TcpSocket(-1,myPort)
{
	_hostAddress = hostIp;
	_t = new thread(&TcpClient::connect, this, hostPort, hostIp, mess,std::ref(isConnected));
	_t->detach();
}

//connect party to server and go to message hendler
int TcpClient::connect(unsigned short hostPort, std::string hostname, Message* mess,bool& isConnected)
{
	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(hostPort); // port that server will listen to
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = inet_addr(hostname.c_str());    // the IP of the server


	int status,i = 0;
	//connect loop -> wait for other parties.
	while (status = ::connect(_socket, (struct sockaddr*) & sa, sizeof(sa))) 
		TRACE("\rID to connect %d: Attempt #%d", hostPort - BASE_PORT, ++i);

	if (status == INVALID_SOCKET)
		throw std::exception("Cant connect to server");

	cout << "Connected to server! id=" << (hostPort-BASE_PORT) % NUM_OF_PARTIES << endl;
	isConnected = true;

	messagesHandler(mess);

	return _socket;
}