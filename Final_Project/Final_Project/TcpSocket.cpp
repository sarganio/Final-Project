#include "TcpSocket.h"
#include <iostream>

using std::cerr;
WSAInitializer TcpSocket::_WSAinit;
TcpSocket::TcpSocket(int socket, unsigned short port):_port(port)
{
	_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");

}

TcpSocket :: ~TcpSocket()
{
	if(isValid())
		this->close();

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
bool TcpSocket::isValid()const {
	return this->_socket != -1;
}
int TcpSocket::socketFd()const { 
	return isValid() ? _socket : -1; 
}