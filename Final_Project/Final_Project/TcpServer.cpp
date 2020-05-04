#include "TcpServer.h"
#include <iostream>
#include <thread>
#include "Messages.h"
#include <string>

#define BLOCKING -1

using std::thread;
	TcpServer::TcpServer(int port):TcpSocket(-1,port),_welcomSocketFD(_socket)
	{
		_hostAddress = "";
		_port = port;

	}
	void TcpServer::serve(Message* mess)//,mutex& m) {
	{
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

		_t = new thread(&TcpServer::accept, this,mess);
		//thread a(&TcpServer::messagesHandler, this);
		_t->detach();
	}

	void TcpServer::accept(Message* mess)//,mutex& m)
	{
		//backup the welcome socket for later deletetion
		SOCKET s = _socket;
		//this accepts the client and create a specific socket from server to this client
		_socket = ::accept(_socket, NULL, NULL);

		if (_socket == INVALID_SOCKET)
			throw std::exception(__FUNCTION__);

		std::cout << "Client accepted. Server and client can speak." << std::endl;

		messagesHandler(mess);//,m);
	}
	bool TcpServer::isValid()const {
		if (_socket == -1 || _socket == _welcomSocketFD)
			return false;
		return true;
	}
	
