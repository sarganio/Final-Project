#include "TcpServer.h"
#include <iostream>

	TcpServer::TcpServer(int port):TcpSocket(-1,port)
	{
		_hostAddress = "";
		_port = port;
	}
	void TcpServer::bind()const
	{
		struct sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(_port);

		// Bind socket to the appropriate port and interface (INADDR_ANY)
		if (::bind(_socket, (struct sockaddr*) & address, sizeof(address)) < 0)
			cerr << "TcpServer: Couldn't bind socket!";
	}
	void TcpServer::listen()const {
		// Listen for incoming connection(s)
		if (::listen(_socket, 1) < 0)
			cerr << "TcpServer: Couldn't start server listening!";
	}

	void TcpServer::accept(void)
	{
		SOCKADDR_IN clientAddress;
		int clientAddrSize = sizeof(clientAddress);
		if (::accept(_socket, (SOCKADDR*)&clientAddress, &clientAddrSize) != INVALID_SOCKET)
			cout << "Client connected!" << endl;
	}
