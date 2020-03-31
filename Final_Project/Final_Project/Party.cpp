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
	//this->_mtx.resize(NUM_OF_PARTIES);
	for (int i = 0; i < NUM_OF_PARTIES; i++)
		this->_msgs.push_back(new Message);
	//expend the vector to contain all parties' sockets
	this->_sockets.resize(NUM_OF_PARTIES);
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
	Message bc(messageType);
	string data =(char*) msg;
	bc.setData(data.c_str());
	for (int i = 0; i < NUM_OF_PARTIES ; i++) {
		if (i == _id)
			continue;
		_sockets[i]->writeBuffer(&bc, HEADER_SIZE);
		_sockets[i]->writeBuffer(bc.getData(), bc.getSize());
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
}
bool Party::sendTo(unsigned short id, unsigned short messageType, void* msg) {
	Message toSend(messageType);
	string data = (char*)msg;
	toSend.setData(data.c_str());
	_sockets[id]->writeBuffer(&toSend, HEADER_SIZE);
	_sockets[id]->writeBuffer(toSend.getData(), toSend.getSize());
	return true;
}
void Party::readFrom(unsigned short id,char* msg) {
	while (!this->_msgs[id]->getIsRead());
	memcpy(msg,_msgs[id]->getData(),_msgs[id]->getSize());
}
unsigned short Party::getID()const { return this->_id; }
void Party::fInput() {
	unsigned char Seq[SEQ_LEN];
	unsigned char seqMy[SEQ_LEN + 1] = {"HOMO"};
	unsigned char seqTo[SEQ_LEN];
	unsigned char seqFrom[SEQ_LEN];
	unsigned char myKey[SEQ_LEN];

	//generate random key and seq
	//if(rand_priv_bytes(seqmy, seq_len) != success)
	//	throw std::exception(__function__"generate random seq failed!");
	//if(rand_priv_bytes(mykey, key_len) != success)
	//	throw std::exception(__function__"generate random key failed!");
	//broadcast seq to other parties
	broadcast(seqMy,SEQ);
	while (true);
	//send this party key to the next party
	sendTo(_id + 1 % NUM_OF_PARTIES,KEY, myKey);
	/*
	while(_mess.at())
	*/
	


	/*
	AES examples:
	*http://c-cpp-notes-snippets.blogspot.com/2017/02/aes-encryptiondecryption-using-openssl.html
	*https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
	*/
	
	
	
}
