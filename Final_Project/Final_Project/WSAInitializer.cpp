#include "WSAInitializer.h"
#include <exception>


WSAInitializer::WSAInitializer()
{
	WSADATA wsaData = {};
	WORD wVersionRequested = WSAStartup(MAKEWORD(2, 2),&wsaData);
	if (wVersionRequested)
		throw std::exception("WSAStartup Failed");
	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
		throw std::exception("TcpServer: Incompatible Windows socket library version!");
}


WSAInitializer::~WSAInitializer()
{
	try
	{
		WSACleanup();
	}
	catch (...) {}
}
