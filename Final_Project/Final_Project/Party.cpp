#include <string>
#include <iostream>
#include "Party.h"

using std::string;
using std::cout;
void Party::connectToAllParties() {
	short idToConnect = (_id + 1) % NUM_OF_PARTIES;
	string destIP = BASE_IP + std::to_string(idToConnect);
	string myIP = BASE_IP + std::to_string(_id);

	short destPort = BASE_PORT + idToConnect, myPort = BASE_PORT + _id;

}