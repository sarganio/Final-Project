#pragma once

#include "TcpSocket.h"


class TcpServer :public TcpSocket
{
public:
	//open tcp socket on port number 'port'
	TcpServer(unsigned short port);
	//bind + listen + accept. throws an exception in case of failure.
	void serve(Message* mess);
	bool isValid()const;
protected:
	unsigned int _welcomSocketFD;
	//accept a new connection
	void accept(Message* mess);

};

