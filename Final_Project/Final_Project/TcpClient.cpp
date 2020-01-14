#include <cstring>
#include <sstream>
#include "TcpClient.h"
#include <iostream>

	TcpClient::TcpClient(int port, std::string hostIp):TcpSocket(-1,port)
	{
		_hostAddress = hostIp;
		// Create a socket client connection.
		connect(port, hostIp);
	}

	int TcpClient::connect(int port, std::string hostname)
	{
		SOCKADDR_IN addr;
		addr.sin_addr.s_addr = inet_addr(_hostAddress.c_str()); // replace the ip with your futur server ip address. If server AND client are running on the same computer, you can use the local ip 127.0.0.1
		addr.sin_family = AF_INET;
		addr.sin_port = htons(5555);

		if(::connect(_socket, (SOCKADDR*)&addr, sizeof(addr)) < 0)
			cout << "Connected to server!" << endl;
		// Close any existing connections.
		this->close();

		// Fill in the address structure
		struct sockaddr_in server_address;
		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = inet_addr(hostname.c_str());
		server_address.sin_port = htons(_port);

		return _socket;
	}