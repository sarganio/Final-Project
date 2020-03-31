#pragma once

#include "TcpSocket.h"

using std::string;

	class TcpClient :public TcpSocket
	{
	public:
		//! Default class constructor creates a socket client connection to the specified host and port.
		TcpClient(string myIP, unsigned short myPort, unsigned short toPort, string hostname, Message* mess,bool& isConnected);//,mutex& m);

		//! Connect the socket client to the specified host and port and returns the resulting socket descriptor.
		/*!
		  If the socket client is already connected, that connection is
		  terminated and a new connection is attempted.
		*/
		int connect(unsigned short port, std::string hostname, Message* mess,bool& isConnected);//, mutex& m);
	};
