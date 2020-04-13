//#include <openssl/rand.h>
#include <string>
#include <iostream>


#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "Helper.h"


#define SEQ_LEN 4
#define SUCCESS 1

using std::string;
using std::cout;
using std::endl;


Party::Party(short myID,long input):_id(myID),_input(input){
	//expend the vector to contain all parties' sockets
	this->_sockets.resize(NUM_OF_PARTIES);
	this->_msgs.resize(NUM_OF_PARTIES);
	this->_keys.resize(NUM_OF_PARTIES);
	//this->_mtx.resize(NUM_OF_PARTIES);
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == _id)
			continue;
		_msgs[i] = new Message;
	}
	AutoSeededRandomPool rnd;
	_keys[_id] = new byte(KEY_LEN);
	rnd.GenerateBlock(_keys[_id],KEY_LEN);
}
void Party::connectToAllParties(string IPs[NUM_OF_PARTIES]) {
	bool isConnected = false;
	//compute the party's id this party needs to initiate communication and the party's id this party needs to wait for a connection
	unsigned short idToConnect = (_id + 1) % NUM_OF_PARTIES;
	unsigned short idFromConnect = (_id + 2) % NUM_OF_PARTIES;//-1 mod 3 = 2

	//as mentioned abobe only with IPs
	string toIP = Helper::IPCompare(IPs[1], IPs[2]) ^ (_id % 2 == 0) ? IPs[1] : IPs[2];
	string myIP = IPs[0];
	
	//toPort - 6200[id + 1] myPort - 62000 - [id]
	unsigned short toPort = BASE_PORT + idToConnect, myPort = BASE_PORT + _id;
	
	//setup a server socket 
	TcpServer* from =new TcpServer(myPort);
	this->_sockets[idFromConnect] = from;
	from->serve(_msgs[idFromConnect]);// , _mtx[idFromConnect]);
	TRACE("Waiting for clients..");

	//setup a client socket
	TcpClient* to = new TcpClient(myIP, myPort, toPort, toIP, _msgs[idToConnect],isConnected);// , _mtx[idToConnect]);
	this->_sockets[idToConnect] = to;

	//check that the sockect to the other parties were created succssesfully
	while (!from->isValid());
	while (!isConnected);

	TRACE("Succssesfully connected to all parties!\n");

}
void Party::broadcast(void* msg,unsigned short messageType)const {
	for (int i = 0; i < NUM_OF_PARTIES ; i++) {
		if (i == _id)
			continue;
		sendTo(i, messageType, msg);
	}
}
Party::~Party() {
	//delete all the sockets of the party
	while (_sockets.size()) {
		TcpSocket* toFree = _sockets.back();
		//safety check before using delete
		if (toFree) {
			delete toFree;
			toFree = nullptr;
		}
		_sockets.pop_back();
	}
	//delete all the sockets of the party
	while (_msgs.size()) {
		Message* toFree = _msgs.back();
		//safety check before using delete
		if (toFree) {
			delete toFree;
			toFree = nullptr;
		}
		_msgs.pop_back();
	}
	//delete all the sockets of the party
	while (_sockets.size()) {
		byte* toFree = _keys.back();
		//safety check before using delete
		if (toFree) {
			delete toFree;
			toFree = nullptr;
		}
		_keys.pop_back();
	}
}
bool Party::sendTo(unsigned short id, unsigned short messageType, void* msg)const {
	Message toSend(messageType);
	string data = (char*)msg;
	toSend.setData(data.c_str());
	_sockets[id]->writeBuffer(&toSend, HEADER_SIZE);
	_sockets[id]->writeBuffer(toSend.getData(), toSend.getSize());
	return true;
}
void Party::readFrom(unsigned short id,unsigned char* msg) {
	while (this->_msgs[id]->getIsRead());
	memcpy(msg,_msgs[id]->getData(),_msgs[id]->getSize());
	//update thread that the message was read
	this->_msgs[id]->setIsRead(true);
}
unsigned short Party::getID()const { return this->_id; }
void Party::fInput() {
	AutoSeededRandomPool rnd;
	byte finalSeq[AES_SIZE]{};
	byte alpha[NUM_OF_PARTIES - 1][AES_SIZE];//TODO: conver to Share!
	byte seqMy[SEQ_LEN];
	byte seqTo[SEQ_LEN];
	byte seqFrom[SEQ_LEN];
	byte fromKey[KEY_LEN];
	
	//genereting random seq
	rnd.GenerateBlock(seqMy, SEQ_LEN);

	//broadcast seq to other parties
	broadcast(seqMy,SEQ);
	readFrom((_id + 1) % NUM_OF_PARTIES, seqTo);
	readFrom((_id + 2) % NUM_OF_PARTIES, seqFrom);
	*finalSeq = *(unsigned int*)seqFrom + *seqMy + *(unsigned int*)seqTo;

	TRACE("SEQ = %u", finalSeq);

	//send this party key to the next party
	sendTo((_id + 1) % NUM_OF_PARTIES,KEY, _keys[_id]);
	readFrom((_id + 2) % NUM_OF_PARTIES, fromKey);
	TRACE("Key of id - 1:%s", fromKey);
	TRACE("My key:%s", fromKey);
	_keys[(_id + 2) % NUM_OF_PARTIES] = new byte(*fromKey);
	for (int i = 0; i < NUM_OF_PARTIES - 1; i++) {
		memcpy_s(alpha[i], sizeof(int), &finalSeq, sizeof(int));
		Helper::encryptAES(alpha[i], SEQ_LEN,(SecByteBlock) *_keys[(_id + 2 + i) % NUM_OF_PARTIES]);
		TRACE("Alpha %d:%s", (_id + 2 + i) % NUM_OF_PARTIES,alpha[i]);
	}
}
