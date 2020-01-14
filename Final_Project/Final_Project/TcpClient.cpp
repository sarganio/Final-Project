#include <cstring>
#include <sstream>
#include "TcpClient.h"
#include <iostream>


using std::cerr;
	TcpClient::TcpClient(int port, std::string hostname)
	{
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(1, 1);

		WSAStartup(wVersionRequested, &wsaData);
		if (wsaData.wVersion != wVersionRequested) 
			cerr << "TcpClient: Incompatible Windows socket library version!";


		// Create a socket client connection.
		connect(port, hostname);
	}

	TcpClient :: ~TcpClient(void)
	{
	}

	int TcpClient::connect(int port, std::string hostname)
	{
		// Close any existing connections.
		this->close();

		// Create the client-side socket
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket < 0) 
			cerr << "TcpClient: Couldn't create socket client!";


		int flag = 1;
		int result = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
		if (result < 0) 
			cerr << "TcpClient: Error setting socket options!";


		// Fill in the address structure
		struct sockaddr_in server_address;
		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = inet_addr(hostname.c_str());
		server_address.sin_port = htons(port);

		// Connect to the server
		if (::connect(_socket, (struct sockaddr*) & server_address, sizeof(server_address)) < 0) 
			cerr << "TcpClient: Couldn't connect to socket server!";


		return _socket;
	}