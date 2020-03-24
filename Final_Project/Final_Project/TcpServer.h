#pragma once

#include "TcpSocket.h"


class TcpServer :public TcpSocket
{
public:
	//open tcp socket on port number 'port'
	TcpServer(int port);
	//bind + listen + accept. throws an exception in case of failure.
	void serve(unsigned char messBuffer[MAX_MESSAGE_SIZE]);// , mutex& m);
protected:
	//accept a new connection
	void accept(unsigned char messBuffer[MAX_MESSAGE_SIZE]);// , mutex& m);
};

