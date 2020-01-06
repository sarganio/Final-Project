#include <string>
#include <iostream>
//#include "WSAInitializer.h"
#include "Party.h"

//#pragma comment(lib, "Ws2_32.lib")

using std::string;
using std::cout;
void Party::connectToAllParties() {
	short idToConnect = (_id + 1) % NUM_OF_PARTIES;
	string destIP = BASE_IP + std::to_string(idToConnect);
	string myIP = BASE_IP + std::to_string(_id);

	short destPort = BASE_PORT + idToConnect, myPort = BASE_PORT + _id;

	//int highSock, lowSock;
	////Create socket
	//highSock = socket(AF_INET, SOCK_STREAM, 0);
	//if (highSock == -1)
	//{
	//	perror("Could not create socket");
	//}

	//cout << "Socket created\n";

	//struct sockaddr_in server;
	//server.sin_addr.s_addr = inet_addr(destIP.c_str());
	//if (server.sin_addr.s_addr == -1)
	//	perror("Could not find address to send request.");

	//server.sin_family = AF_INET;
	//server.sin_port = htons(destPort);

	////Connect to remote server
	//if (connect(highSock, (struct sockaddr*) & server, sizeof(server)) < 0)
	//{
	//	perror("connect failed. Error");
	//}

	//cout << "Connected to next id peer\n";











	//WSAInitializer wsa;

	//try {
	//	//std::cout << "Statring... " << std::endl;
	//	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//	if (serverSocket == INVALID_SOCKET)
	//		throw std::exception(__FUNCTION__ " - socket");

	//	struct sockaddr_in sa = { 0 };
	//	sa.sin_port = htons(8086);
	//	sa.sin_family = AF_INET;
	//	sa.sin_addr.s_addr = INADDR_ANY;

	//	//std::cout << "Binding..." << std::endl;
	//	if (::bind(serverSocket, (struct sockaddr*) & sa, sizeof(sa)) == SOCKET_ERROR)
	//		throw std::exception(__FUNCTION__ " - bind");

	//	//m_communicator.bindAndListen(serverSocket);
	//}
	//catch (std::exception & e)
	//{
	//	std::cerr << e.what() << std::endl;
	//}

}