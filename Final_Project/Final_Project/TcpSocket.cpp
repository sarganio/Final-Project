#include "TcpSocket.h"
#include "Helper.h"
#include <iostream>
#include <string>
#include <condition_variable>
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
void TcpSocket::messagesHandler(Message* mess)
{
	char type;
// Setup timeval variable
struct fd_set FDs;

unsigned short fromID = ((this->_port - BASE_PORT) + 2)%NUM_OF_PARTIES;////////////////////TODO:needs to be fit both client and server/////////////////
	std::mutex& dataMutex = mess->getDataMutex();
	std::condition_variable& mine = mess->getListenerCV();
	std::condition_variable& other = mess->getPartyCV();
	while (true) {
		// Setup fd_set structure
		FD_ZERO(&FDs);
		FD_SET(_socket, &FDs);
		//wait for messages from socket
		select(_socket + 1, &FDs, NULL, NULL, NULL);
		std::unique_lock<std::mutex> listenerUL(mess->getIsReadMutex());

		mine.wait(listenerUL, [&] {mess->getIsRead(); });

		//read message type - 1B
		//wait until the previous message is read 
		dataMutex.lock();
		this->readBuffer(&type, 1);
		dataMutex.unlock();

		//read the header: size of message - 2B
		mess->setSize(type);
		unsigned short expectedSize = mess->getSize();
		dataMutex.lock();
		this->readBuffer(mess + 1, HEADER_SIZE - 1);
		dataMutex.unlock();

		//check if the message hase a proper size field
		if (expectedSize != mess->getSize()) {
			std::string errorMsg(__FUNCTION__ + ("Received from:" + std::to_string(fromID) + "-Message's size is invalid!"));
			throw std::exception(errorMsg.c_str());
		}

		//read rest of message
		dataMutex.lock();
		this->readBuffer(mess->getData(), mess->getSize());
		dataMutex.unlock();
		//let the other thread know there is a new message to be read
		other.notify_one();
		//mark message as ready to be read
		mess->setIsRead(false);
		//put the message in the mutual variable of main thread and this thread

		//TRACE("Got a new message from %d.\nThe message is: %s", fromID, mess->getData());
	}
}