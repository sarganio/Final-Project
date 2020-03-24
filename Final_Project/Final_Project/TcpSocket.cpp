#include <iostream>
#include <string>
#include "TcpSocket.h"
#include "Helper.h"


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
void TcpSocket::messagesHandler(unsigned char type[RECONSTRUCT_LEN + 1])// , mutex& m_type)
{

// Setup timeval variable
struct fd_set FDs;

unsigned short fromID = (this->_port - BASE_PORT + 2)%NUM_OF_PARTIES;////////////////////TODO:needs to be fit both client and server/////////////////
//std::unique_lock<std::mutex> lk(m_type);
//std::condition_variable cv;
	while (true) {
		// Setup fd_set structure
		FD_ZERO(&FDs);
		FD_SET(_socket, &FDs);
		//wait for messages from socket
		select(_socket + 1, &FDs, NULL, NULL, NULL);

		//read message type - 1B
		//lk.lock();
		this->readBuffer(&type, 1);

		//read the header: size of message - 2B
		Message rcv(type[0]);
		unsigned short expectedSize = rcv.getSize();
		this->readBuffer(&rcv + 1, HEADER_SIZE - 1);

		//check if the message hase a proper size field
		if (expectedSize != rcv.getSize()) {
			std::string errorMsg(__FUNCTION__ + ("Received from:" + std::to_string(fromID) + "-Message's size is invalid!"));
			throw std::exception(errorMsg.c_str());
		}

		//read rest of message
		this->readBuffer(rcv.getData(), rcv.getSize());
		//put the message in the mutual variable of main thread and this thread
		memcpy_s(type, RECONSTRUCT_LEN + 1,rcv.getData(),rcv.getSize());
		//lk.unlock();
		//cv.notify_one();
		TRACE("Got a new message from %d.\nThe message is: %s", fromID, rcv.getData());

		//reset buffer
		memset(&rcv, 0, sizeof(rcv));

		cout << "Got message from client!" << endl;
	}
}