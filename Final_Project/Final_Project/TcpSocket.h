#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

	class TcpSocket 
	{
	public:

		enum ProtocolType {
			PROTO_TCP,
			PROTO_UDP
		};

		//! Class constructor
		TcpSocket();

		//! Class destructor.
		~TcpSocket();

		//! Return the socket descriptor.
		int id(void) const { return _socket; };

		//! Return the socket port number.
		int port(void) const { return _port; };

		//! Write a buffer over the socket connection.  Returns the number of bytes written or -1 if an error occurs.
		int writeBuffer(const void* buffer, long bufferSize, int flags = 0);

		//! Read an input buffer, up to length \e bufferSize.  Returns the number of bytes read or -1 if an error occurs.
		int readBuffer(void* buffer, long bufferSize, int flags = 0);

		//! Returns true if the socket descriptor is valid.
		bool isValid() { return _socket != -1; };

		void setBlocking(int socket, bool enable);

		void close();

	protected:

		int _socket;
		int _port;

	};
