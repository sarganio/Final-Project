#pragma once
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")

class WSAInitializer
{
public:
	WSAInitializer();
	~WSAInitializer();
};

