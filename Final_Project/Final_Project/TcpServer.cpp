#include "TcpServer.h"
#include <iostream>

using std::cerr;

	TcpServer::TcpServer(int port)
	{
		// Create a socket server.
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(1, 1);

		WSAStartup(wVersionRequested, &wsaData);
		if (wsaData.wVersion != wVersionRequested) 
			cerr << "TcpServer: Incompatible Windows socket library version!";


		// Create the server-side socket
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket < 0) 
			cerr << "TcpServer: Couldn't create socket server!";

		int flag = 1;
		int result = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
		if (result < 0) 
			cerr << "TcpServer: Error setting socket options!";


		struct sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(port);

		// Bind socket to the appropriate port and interface (INADDR_ANY)
		if (bind(_socket, (struct sockaddr*) & address, sizeof(address)) < 0) 
			cerr << "TcpServer: Couldn't bind socket!";

		// Listen for incoming connection(s)
		if (listen(_socket, 1) < 0) 
			cerr << "TcpServer: Couldn't start server listening!";

		_port = port;
	}

	TcpServer :: ~TcpServer()
	{
	}

	int TcpServer::accept(void)
	{
		return ::accept(_socket, NULL, NULL);
	}
