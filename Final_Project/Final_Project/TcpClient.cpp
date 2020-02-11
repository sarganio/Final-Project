#include <cstring>
#include <sstream>
#include "TcpClient.h"
#include <iostream>
#define IP_INDEX 1

	TcpClient::TcpClient(string myIP,int myPort,int hostPort, std::string hostIp):TcpSocket(-1,myPort)
	{
		_hostAddress = hostIp;
		// Create a socket client connection.
		if ((_socket = ::socket(AF_INET, SOCK_STREAM, 0)) <= 0)
			//cerr << "TcpClient: Couldn't create socket client!";
			throw std::exception("TcpClient: Couldn't create socket client!");

		struct sockaddr_in myAddr;
		// Explicitly assigning port number by 
		// binding client with that port  
		myAddr.sin_family = AF_INET;
		myAddr.sin_addr.s_addr = INADDR_ANY;
		myAddr.sin_port = htons(myPort);
		myAddr.sin_addr.s_addr = inet_addr(hostIp.c_str());
		///////////////////////////////////////////////////
		//char szBuffer[1024];
		//gethostname(szBuffer, sizeof(szBuffer));
		//struct hostent* host = gethostbyname(szBuffer);
		//////////////////////////////////////////////////
		//// This ip address will change according to the machine 
		//myAddr.sin_addr.s_addr = *(long*)(host->h_addr_list[IP_INDEX]);
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
		int status,i = 0;
		cout << "Trying to connent the server.." << endl;
		while (status = ::connect(_socket, (struct sockaddr*) & sa, sizeof(sa))) {
			cout << "Attempt #" << ++i << endl;
			cout << status << endl;
			Sleep(500);
		}

		if (status == INVALID_SOCKET)
			throw std::exception("Cant connect to server");

		return _socket;
	}