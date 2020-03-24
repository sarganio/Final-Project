#include <cstring>
#include <sstream>
#include "TcpClient.h"
#include <iostream>
#include "Messages.h"
#include "Helper.h"
#include <thread>
#define IP_INDEX 1
using std::thread;
TcpClient::TcpClient(string myIP,unsigned short myPort, unsigned short hostPort, std::string hostIp, unsigned char messBuffer[MAX_MESSAGE_SIZE]) :TcpSocket(-1, myPort)//, mutex& m):TcpSocket(-1,myPort)
{
	_hostAddress = hostIp;
	// Create a socket client connection.
	if ((_socket = ::socket(AF_INET, SOCK_STREAM, 0)) <= 0)
		//cerr << "TcpClient: Couldn't create socket client!";
		throw std::exception(__FUNCTION__"Couldn't create socket client!");

	struct sockaddr_in myAddr;
	// Explicitly assigning port number by 
	// binding client with that port  
	myAddr.sin_family = AF_INET;
	myAddr.sin_addr.s_addr = INADDR_ANY;
	myAddr.sin_port = htons(myPort);
	myAddr.sin_addr.s_addr = inet_addr(myIP.c_str());

	if (bind(_socket, (struct sockaddr*) & myAddr, sizeof(struct sockaddr_in)) != 0)
		throw std::exception("Client bind failed (port assighnment)");

	_t = new thread(&TcpClient::connect, this, hostPort, hostIp, messBuffer);// , m);
	_t->detach();
}

int TcpClient::connect(unsigned short port, std::string hostname, unsigned char messBuffer[MAX_MESSAGE_SIZE])//, mutex& m)
{
	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(port); // port that server will listen to
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = inet_addr(hostname.c_str());    // the IP of the server

	int status,i = 0;
	TRACE("Trying to connent the server..");
	while (status = ::connect(_socket, (struct sockaddr*) & sa, sizeof(sa))) {
		TRACE("ID to connect %d: Attempt #%d", port - BASE_PORT, ++i);
		Sleep(500);
	}

	if (status == INVALID_SOCKET)
		throw std::exception("Cant connect to server");
	messagesHandler(messBuffer);//,m);

	return _socket;
}