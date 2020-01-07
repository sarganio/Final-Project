#pragma once

#include "Socket.h"


	/***************************************************/
	/*! \class TcpClient
		\brief STK TCP socket client class.
		This class provides a uniform cross-platform TCP socket client
		interface.  Methods are provided for reading or writing data
		buffers to/from connections.
		TCP sockets are reliable and connection-oriented.  A TCP socket
		client must be connected to a TCP server before data can be sent
		or received.  Data delivery is guaranteed in order, without loss,
		error, or duplication.  That said, TCP transmissions tend to be
		slower than those using the UDP protocol and data sent with
		multiple \e write() calls can be arbitrarily combined by the
		underlying system.
		The user is responsible for checking the values
		returned by the read/write methods.  Values
		less than or equal to zero indicate a closed
		or lost connection or the occurence of an error.
		by Perry R. Cook and Gary P. Scavone, 1995--2019.
	*/
	/***************************************************/

	class TcpClient :public Socket
	{
	public:
		//! Default class constructor creates a socket client connection to the specified host and port.
		/*!
		  An StkError will be thrown if a socket error occurs during instantiation.
		*/
		TcpClient(int port, std::string hostname = "localhost");

		//! The class destructor closes the socket instance, breaking any existing connections.
		~TcpClient();

		//! Connect the socket client to the specified host and port and returns the resulting socket descriptor.
		/*!
		  If the socket client is already connected, that connection is
		  terminated and a new connection is attempted.  An StkError will be
		  thrown if a socket error occurs.
		*/
		int connect(int port, std::string hostname = "localhost");

	protected:

	};
