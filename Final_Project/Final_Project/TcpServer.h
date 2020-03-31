#pragma once

#include "TcpSocket.h"


class TcpServer :public TcpSocket
{
public:
	//open tcp socket on port number 'port'
	TcpServer(int port);
	//bind + listen + accept. throws an exception in case of failure.
	void serve(Message* mess);// , mutex& m);
	bool isValid()const;
protected:
	unsigned int _welcomSocketFD;
	//accept a new connection
	void accept(Message* mess);// , mutex& m);

};

