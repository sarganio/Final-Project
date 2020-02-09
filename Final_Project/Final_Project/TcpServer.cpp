#include "TcpServer.h"
#include <iostream>
#include <thread>

#define BLOCKING -1
using std::thread;
	TcpServer::TcpServer(int port):TcpSocket(-1,port)
	{
		_hostAddress = "";
		_port = port;

	}
	void TcpServer::serve() {
		struct sockaddr_in sa = { 0 };
		sa.sin_port = htons(_port);
		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = INADDR_ANY;

		std::cout << "Binding..." << std::endl;
		if (::bind(_socket, (struct sockaddr*) & sa, sizeof(sa)) == SOCKET_ERROR)
			throw std::exception(__FUNCTION__ " - bind");

		std::cout << "Listening..." << std::endl;
		if (listen(_socket, SOMAXCONN) == SOCKET_ERROR)
			throw std::exception(__FUNCTION__ " - listen");

		std::cout << "Accepiting clients..." << std::endl;

			accept();
	}

	void TcpServer::accept(void)
	{
		//backup the welcome socket for later deletetion
		SOCKET s = _socket;
		// this accepts the client and create a specific socket from server to this client
		_socket = ::accept(_socket, NULL, NULL);

		//close welcome socket
		::closesocket(s);

		if (_socket == INVALID_SOCKET)
			throw std::exception(__FUNCTION__);
		
		thread a(&TcpServer::messagesHandler, this);
		a.detach();
		std::cout << "Client accepted. Server and client can speak" << std::endl;
	}
	void TcpServer::messagesHandler() {
		cout << "Got message from client!" << endl;
		WSAPOLLFD FDs = {};
		FDs.fd = _socket;
		FDs.events = POLLRDNORM;
		FDs.revents = 0;
		while (1) {
			WSAPoll(&FDs, 1, BLOCKING);
			cout << "Got message from client!" << endl;
		}
	}
