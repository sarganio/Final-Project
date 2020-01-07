#include <cstring>
#include <sstream>
#include "TcpClient.h"


namespace stk {

	TcpClient::TcpClient(int port, std::string hostname)
	{
	#if defined(__OS_WINDOWS__)  // windoze-only stuff
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(1, 1);

		WSAStartup(wVersionRequested, &wsaData);
		if (wsaData.wVersion != wVersionRequested) {
			oStream_ << "TcpClient: Incompatible Windows socket library version!";
			handleError(StkError::PROCESS_SOCKET);
		}
	#endif

		// Create a socket client connection.
		connect(port, hostname);
	}

	TcpClient :: ~TcpClient(void)
	{
	}

	int TcpClient::connect(int port, std::string hostname)
	{
		// Close any existing connections.
		this->close(_socket);

		// Create the client-side socket
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket < 0) {
			oStream_ << "TcpClient: Couldn't create socket client!";
			handleError(StkError::PROCESS_SOCKET);
		}

		int flag = 1;
		int result = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
		if (result < 0) {
			oStream_ << "TcpClient: Error setting socket options!";
			handleError(StkError::PROCESS_SOCKET);
		}

		struct hostent* hostp;
		if ((hostp = gethostbyname(hostname.c_str())) == 0) {
			oStream_ << "TcpClient: unknown host (" << hostname << ")!";
			handleError(StkError::PROCESS_SOCKET_IPADDR);
		}

		// Fill in the address structure
		struct sockaddr_in server_address;
		server_address.sin_family = AF_INET;
		memcpy((void*)&server_address.sin_addr, hostp->h_addr, hostp->h_length);
		server_address.sin_port = htons(port);

		// Connect to the server
		if (::connect(_socket, (struct sockaddr*) & server_address, sizeof(server_address)) < 0) {
			oStream_ << "TcpClient: Couldn't connect to socket server!";
			handleError(StkError::PROCESS_SOCKET);
		}

		return _socket;
	}

	int TcpClient::writeBuffer(const void* buffer, long bufferSize, int flags)
	{
		if (!isValid(_socket)) return -1;
		return send(_socket, (const char*)buffer, bufferSize, flags);
	}

	int TcpClient::readBuffer(void* buffer, long bufferSize, int flags)
	{
		if (!isValid(_socket)) return -1;
		return recv(_socket, (char*)buffer, bufferSize, flags);
	}

} // stk namespace