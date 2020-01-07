#pragma once

#include "Socket.h"


	class TcpServer :public Socket
	{
	public:
		//! Default constructor creates a local socket server on port 2006 (or the specified port number).
		/*!
		  An StkError will be thrown if a socket error occurs during instantiation.
		*/
		TcpServer(int port);

		//! The class destructor closes the socket instance, breaking any existing connections.
		~TcpServer();

		//! Extract the first pending connection request from the queue and create a new connection, returning the descriptor for the accepted socket.
		/*!
		  If no connection requests are pending and the socket has not
		  been set non-blocking, this function will block until a connection
		  is present.  If an error occurs, -1 is returned.
		*/
		int accept(void);


	protected:

	};

