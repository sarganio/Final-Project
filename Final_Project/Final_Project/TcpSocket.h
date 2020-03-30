#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <mutex>

#include "WSAInitializer.h"
#include "Messages.h"

#pragma comment(lib, "Ws2_32.lib")
#define IP_LEN 15
using std::cerr;
using std::cout;
using std::endl;
using std::thread;
using std::mutex;
class TcpSocket 
{
protected:
	int _socket;
	unsigned short _port;
	std::string _hostAddress;
	static WSAInitializer _WSAinit;
	thread* _t = nullptr;//thread for receiving messages
public: 
	//! Class constructor
	TcpSocket(int socket, unsigned short port);

	//! Class destructor.
	~TcpSocket();

	//! Return the socket descriptor.
	int id(void) const { return _socket; };

	//! Return the socket port number.
	unsigned short port(void) const { return _port; };

	//! Write a buffer over the socket connection.  Returns the number of bytes written or -1 if an error occurs.
	int writeBuffer(const void* buffer, long bufferSize, int flags = NULL);

	//! Read an input buffer, up to length \e bufferSize.  Returns the number of bytes read or -1 if an error occurs.
	int readBuffer(void* buffer, long bufferSize, int flags = NULL);

	//! Returns true if the socket descriptor is valid.
	bool isValid()const;

	int socketFd()const;

	void close();

	//receive all messages comming via _socket
	void messagesHandler(Message* mess);// , mutex& m_type);

};
