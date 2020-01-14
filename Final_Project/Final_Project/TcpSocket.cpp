#include "TcpSocket.h"
#include <iostream>

using std::cerr;

TcpSocket::TcpSocket(int socket, int port) :_port(_port)
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
		cerr << "TcpServer: Incompatible Windows socket library version!";

	if (_socket = ::socket(AF_INET, SOCK_STREAM, 0) < 0)
		cerr << "TcpClient: Couldn't create socket client!";

}

	TcpSocket :: ~TcpSocket()
	{
		if(isValid())
			this->close();

		WSACleanup();

	}

	int TcpSocket::writeBuffer(const void* buffer, long bufferSize, int flags)
	{
		if (!isValid()) return -1;
		return send(_socket, (const char*)buffer, sizeof(buffer), 0);
	}

	int TcpSocket::readBuffer(void* buffer, long bufferSize, int flags)
	{
		if (!isValid()) return -1;
		return recv(_socket, (char*)buffer, bufferSize, flags);
	}
	void TcpSocket::close()
	{
		if (!isValid()) return;

		::closesocket(_socket);

	}