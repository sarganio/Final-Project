#include "Socket.h"

	Socket::Socket()
	{
		_socket = -1;
		_port = -1;
	}

	Socket :: ~Socket()
	{
		if(isValid())
			this->close();

		WSACleanup();

	}

	void Socket::setBlocking(int socket, bool enable)
	{
		if (!isValid()) return;

		unsigned long non_block = !enable;
		ioctlsocket(socket, FIONBIO, &non_block);

	}


	int Socket::writeBuffer(const void* buffer, long bufferSize, int flags)
	{
		if (!isValid()) return -1;
		return send(_socket, (const char*)buffer, bufferSize, flags);
	}

	int Socket::readBuffer(void* buffer, long bufferSize, int flags)
	{
		if (!isValid()) return -1;
		return recv(_socket, (char*)buffer, bufferSize, flags);
	}
	void Socket::close()
	{
		if (!isValid()) return;

		::closesocket(_socket);

	}