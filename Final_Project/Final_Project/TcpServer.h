#pragma once

#include "TcpSocket.h"


	class TcpServer :public TcpSocket
	{
	public:
		//! Default constructor creates a local socket server on port 2006 (or the specified port number).
		TcpServer(int port);
		void bind() const;
		void listen()const;

		//! Extract the first pending connection request from the queue and create a new connection, returning the descriptor for the accepted socket.
		/*!
		  If no connection requests are pending and the socket has not
		  been set non-blocking, this function will block until a connection
		  is present.  If an error occurs, -1 is returned.
		*/
		void accept(void);


	protected:

	};

