#include "TcpServer.h"
#include <iostream>
#include <thread>
#include "Messages.h"

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
		std::unique_ptr<thread>t(new thread(&TcpServer::messagesHandler, this));
		//thread a(&TcpServer::messagesHandler, this);
		t->detach();
	}
	void TcpServer::messagesHandler() {

		unsigned short messageSize = -1;
		uint8_t messageType = -1;
		// Setup timeval variable
		struct fd_set FDs;
		char buffer[MAX_MESSAGE_SIZE] = {};

		//backup the welcome socket for later deletetion
		SOCKET s = _socket;
		// this accepts the client and create a specific socket from server to this client
		_socket = ::accept(_socket, NULL, NULL);

		//close welcome socket
		::closesocket(s);

		if (_socket == INVALID_SOCKET)
			throw std::exception(__FUNCTION__);

		std::cout << "Client accepted. Server and client can speak" << std::endl;

		while (TRUE) {
			// Setup fd_set structure
			FD_ZERO(&FDs);
			FD_SET(_socket, &FDs);
			//wait for messages from socket
			select(_socket + 1, &FDs, NULL, NULL, NULL);
			//read the header: type(1) + size of message(2)
			this->readBuffer(buffer, HEADER_SIZE);

			messageType = buffer[0];
			messageSize  = (buffer[1] << 8) | buffer[2];

			//read rest of message
			this->readBuffer(buffer, messageSize);

			cout << "Got a new message from "<<this->_port - BASE_PORT<<".\nThe message is: " << buffer << endl;

			//reset buffer
			memset(buffer, 0, messageSize + HEADER_SIZE);


			cout << "Got message from client!" << endl;
		}
	}
