#include "TcpServer.h"
#include <iostream>
#include <thread>
#include "Messages.h"
#include <string>

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

		std::cout << "Client accepted. Server and client can speak" << std::endl;
		_t = new thread(&TcpServer::messagesHandler, this);
		//thread a(&TcpServer::messagesHandler, this);
		_t->detach();
	}
	void TcpServer::messagesHandler() {

		// Setup timeval variable
		struct fd_set FDs;

		unsigned short fromID = this->_port - BASE_PORT;

		while (TRUE) {
			// Setup fd_set structure
			FD_ZERO(&FDs);
			FD_SET(_socket, &FDs);
			//wait for messages from socket
			select(_socket + 1, &FDs, NULL, NULL, NULL);

			//read message type - 1B
			char type;
			this->readBuffer(&type, 1);

			//read the header: size of message - 2B
			Message rcv(type);
			unsigned short expectedSize = rcv.getSize();
			this->readBuffer(&rcv + 1, HEADER_SIZE - 1);
			if (expectedSize != rcv.getSize()) {
				std::string errorMsg(__FUNCTION__ + ("Received from:" + std::to_string(fromID) + "-Message's size is invalid!"));
				throw std::exception(errorMsg.c_str());
			}

			//read rest of message
			this->readBuffer(rcv.getData(), rcv.getSize());

			cout << "Got a new message from "<<fromID<<".\nThe message is: " << rcv.getData() << endl;

			//reset buffer
			memset(&rcv, 0, sizeof(rcv));


			cout << "Got message from client!" << endl;
		}
	}
