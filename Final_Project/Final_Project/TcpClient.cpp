#include <cstring>
#include <sstream>
#include "TcpClient.h"
#include <iostream>

	TcpClient::TcpClient(int myPort,int hostPort, std::string hostIp):TcpSocket(-1,myPort)
	{
		_hostAddress = hostIp;
		// Create a socket client connection.
		if ((_socket = ::socket(AF_INET, SOCK_STREAM, 0) )<= 0)
			cerr << "TcpClient: Couldn't create socket client!";

		struct sockaddr_in myAddr;
		// Explicitly assigning port number by 
		// binding client with that port  
		myAddr.sin_family = AF_INET;
		myAddr.sin_addr.s_addr = INADDR_ANY;
		myAddr.sin_port = htons(myPort);

		// This ip address will change according to the machine 
		myAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		if (bind(_socket, (struct sockaddr*) & myAddr, sizeof(struct sockaddr_in)) != 0)
			throw std::exception("Client bind failed (port assighnment)");
		connect(hostPort, hostIp);
	}

	int TcpClient::connect(int port, std::string hostname)
	{
		struct sockaddr_in sa = { 0 };

		sa.sin_port = htons(port); // port that server will listen to
		sa.sin_family = AF_INET;   // must be AF_INET
		sa.sin_addr.s_addr = inet_addr(hostname.c_str());    // the IP of the server

		// notice that we step out to the global namespace
		// for the resolution of the function socket
		int status = ::connect(_socket, (struct sockaddr*) & sa, sizeof(sa));

		if (status == INVALID_SOCKET)
			throw std::exception("Cant connect to server");

		return _socket;
	}