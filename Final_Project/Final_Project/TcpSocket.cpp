#include "TcpSocket.h"
#include <iostream>
#include "Messages.h"
#include <string>

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
	if (_t)
		delete _t;

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
void TcpSocket::messagesHandler() {

	// Setup timeval variable
	struct fd_set FDs;

	unsigned short fromID = (this->_port + 2 - BASE_PORT)%NUM_OF_PARTIES;

	while (TRUE) {
		// Setup fd_set structure
		FD_ZERO(&FDs);
		FD_SET(_socket, &FDs);
		//wait for messages from socket
		select(_socket + 1, &FDs, NULL, NULL, NULL);

		//read message type - 1B
		char type;
		this->readBuffer(&type, 1);

		//read the header: size of message - 2B
		Message rcv(type);
		unsigned short expectedSize = rcv.getSize();
		this->readBuffer(&rcv + 1, HEADER_SIZE - 1);
		if (expectedSize != rcv.getSize()) {
			std::string errorMsg(__FUNCTION__ + ("Received from:" + std::to_string(fromID) + "-Message's size is invalid!"));
			throw std::exception(errorMsg.c_str());
		}

		//read rest of message
		this->readBuffer(rcv.getData(), rcv.getSize());

		cout << "Got a new message from " << fromID << ".\nThe message is: " << rcv.getData() << endl;

		//reset buffer
		memset(&rcv, 0, sizeof(rcv));


		cout << "Got message from client!" << endl;
	}
}