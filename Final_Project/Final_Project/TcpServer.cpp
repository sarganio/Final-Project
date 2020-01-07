#include "TcpServer.h"

namespace stk {

	TcpServer::TcpServer(int port)
	{
		// Create a socket server.
	#if defined(__OS_WINDOWS__)  // windoze-only stuff
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(1, 1);

		WSAStartup(wVersionRequested, &wsaData);
		if (wsaData.wVersion != wVersionRequested) {
			oStream_ << "TcpServer: Incompatible Windows socket library version!";
			handleError(StkError::PROCESS_SOCKET);
		}
	#endif

		// Create the server-side socket
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket < 0) {
			oStream_ << "TcpServer: Couldn't create socket server!";
			handleError(StkError::PROCESS_SOCKET);
		}

		int flag = 1;
		int result = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
		if (result < 0) {
			oStream_ << "TcpServer: Error setting socket options!";
			handleError(StkError::PROCESS_SOCKET);
		}

		struct sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(port);

		// Bind socket to the appropriate port and interface (INADDR_ANY)
		if (bind(_socket, (struct sockaddr*) & address, sizeof(address)) < 0) {
			oStream_ << "TcpServer: Couldn't bind socket!";
			handleError(StkError::PROCESS_SOCKET);
		}

		// Listen for incoming connection(s)
		if (listen(_socket, 1) < 0) {
			oStream_ << "TcpServer: Couldn't start server listening!";
			handleError(StkError::PROCESS_SOCKET);
		}

		_port = port;
	}

	TcpServer :: ~TcpServer()
	{
	}

	int TcpServer::accept(void)
	{
		return ::accept(_socket, NULL, NULL);
	}

	int TcpServer::writeBuffer(const void* buffer, long bufferSize, int flags)
	{
		if (!isValid(_socket)) return -1;
		return send(_socket, (const char*)buffer, bufferSize, flags);
	}

	int TcpServer::readBuffer(void* buffer, long bufferSize, int flags)
	{
		if (!isValid(_socket)) return -1;
		return recv(_socket, (char*)buffer, bufferSize, flags);
	}

} // stk namespace