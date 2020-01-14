#include "TcpSocket.h"

	TcpSocket::TcpSocket()
	{
		_socket = -1;
		_port = -1;
	}

	TcpSocket :: ~TcpSocket()
	{
		if(isValid())
			this->close();

		WSACleanup();

	}

	void TcpSocket::setBlocking(int socket, bool enable)
	{
		if (!isValid()) return;

		unsigned long non_block = !enable;
		ioctlsocket(socket, FIONBIO, &non_block);

	}


	int TcpSocket::writeBuffer(const void* buffer, long bufferSize, int flags)
	{
		if (!isValid()) return -1;
		return send(_socket, (const char*)buffer, bufferSize, flags);
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