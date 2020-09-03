//#include <openssl/rand.h>
#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "Circuit.h"
#include <string>
#include <iostream>

#define SUCCESS 1

using std::string;
using std::cout;
using std::endl;


Party::Party(short myID,long input):_id(myID),_input(input),_arithmeticCircuit(nullptr){
	//for Dbug
	 srand(10);

	//expend the vector to contain all parties' sockets
	this->_sockets.resize(NUM_OF_PARTIES);
	this->_msgs.resize(NUM_OF_PARTIES);
	this->_keys.resize(NUM_OF_PARTIES);
	this->_shares.resize(NUM_OF_PARTIES);

	//this->_mtx.resize(NUM_OF_PARTIES);
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == _id)
			continue;
		_msgs[i] = new Message;
	}
	//AutoSeededRandomPool rnd;
	//_keys[_id] = new SecByteBlock(0x00,KEY_LEN);
	//rnd.GenerateBlock(*_keys[_id],_keys[_id]->size());
	//cout<<std::dec << endl;

}
void Party::printKey(unsigned short index)const {
	if (index < 0 || index > NUM_OF_PARTIES)
		return;
	string a((char*)_keys[index]->begin(), KEY_LEN);
	for (int i = 0; i < a.length(); i++)
		printf("%u ", (byte)a[i]);
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
void Party::broadcast(byte* msg, byte messageType)const {
	for (unsigned short i = 0; i < NUM_OF_PARTIES ; i++) {
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
			//delete toFree; HEAP CORRUPTION
			toFree = nullptr;
		}
		_msgs.pop_back();
	}
	//delete all the sockets of the party
	while (_keys.size()) {
		SecByteBlock* toFree = _keys.back();
		//safety check before using delete
		if (toFree) {
			delete toFree;
			toFree = nullptr;
		}
		_keys.pop_back();
	}
}
bool Party::sendTo(unsigned short id, byte messageType, byte* msg)const {
	Message toSend(messageType);
	toSend.setData(msg);
	_sockets[id]->writeBuffer(&toSend, HEADER_SIZE);
	_sockets[id]->writeBuffer(toSend.getData(), toSend.getSize());
	return true;
}
void Party::readFrom(unsigned short id,byte* msg) {
	while ( this->_msgs[id]->getIsRead());
	memcpy(msg,_msgs[id]->getData(),_msgs[id]->getSize());
	//update thread that the message was read
	this->_msgs[id]->setIsRead(true);
}
unsigned short Party::getID()const { return this->_id; }
void Party::calcSeq() {
	AutoSeededRandomPool rnd;
	byte seqMy[SEQ_LEN];
	byte seqTo[SEQ_LEN];
	byte seqFrom[SEQ_LEN];

	//genereting random seq
	rnd.GenerateBlock(seqMy, SEQ_LEN);

	//broadcast seq to other parties
	broadcast(seqMy, SEQ);

	readFrom((_id + 1) % NUM_OF_PARTIES, seqTo);
	readFrom((_id + 2) % NUM_OF_PARTIES, seqFrom);

	*(unsigned int*)_finalSeq = *(unsigned int*)seqFrom + *(unsigned int*)seqMy + *(unsigned int*)seqTo;

	//TRACE("SEQ = %u", *(unsigned int*)_finalSeq);
}
Share* Party::fRand() {
	static unsigned int calledThisFunc = 0;
	calledThisFunc++;
	
	byte alpha[NUM_OF_PARTIES][KEY_LEN]{};//TODO: conver to Share!
	byte fromKey[KEY_LEN]{};
	byte IV[KEY_LEN]{};
	AutoSeededRandomPool rnd;
	Share* ans = new Share((_id + 2) % NUM_OF_PARTIES, 'a' + calledThisFunc);
	
	_keys[_id] = new SecByteBlock(0x00,KEY_LEN);
	rnd.GenerateBlock(*_keys[_id],_keys[_id]->size());
	
	for (unsigned int i = 0; i < KEY_LEN / SEQ_LEN; i++)
		*(unsigned int*)(IV+i * SEQ_LEN) = *(unsigned int*)_finalSeq;
		//*(unsigned int*)(IV + i * SEQ_LEN) = *(unsigned int*)_finalSeq;

	//send this party key to the next party
	sendTo((_id + 1) % NUM_OF_PARTIES,KEY, _keys[_id]->data());
	readFrom((_id + 2) % NUM_OF_PARTIES, fromKey);

	_keys[(_id + 2) % NUM_OF_PARTIES] = new SecByteBlock(fromKey,KEY_LEN);

	for (unsigned short i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == (_id + 1)%NUM_OF_PARTIES)
			continue;
		memcpy_s(alpha[i], sizeof(int), &_finalSeq, sizeof(int));
		Helper::encryptAES(alpha[i], KEY_LEN,*_keys[i],IV); 
		(*ans)[i] = *(long*)alpha[i];
		//TRACE("Alpha %d:%u", i, *(unsigned int*)alpha[i]);
	}
	//free vector of keys
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (!_keys[i])
			continue;
		delete _keys[i];
		_keys[i] = nullptr;
	}
	return ans;
}
void Party::fInput() {
	vector<Share*> randomShares;
	long randomNum;
	byte partiesInputs[NUM_OF_PARTIES][ENC_INPUT_LEN];
	randomShares.resize(NUM_OF_PARTIES);
	calcSeq();

	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		randomShares[i] = fRand();//randomShares[i] - the random number for input #i
		//cout << "Share #" << i << " " << randomShares[i]->toString() << endl;
	}
	//reconstruct the random value of this party
	randomNum = reconstruct(randomShares);
	randomNum = _input - randomNum;
	//brodcast the salted input
	broadcast((byte*)&randomNum,ENC_INPUT);
	//reciecve other parties salted inputs
	readFrom((_id + 2) % NUM_OF_PARTIES, partiesInputs[(_id + 2) % NUM_OF_PARTIES]);
	readFrom((_id + 1) % NUM_OF_PARTIES, partiesInputs[(_id + 1) % NUM_OF_PARTIES]);
	memcpy_s(partiesInputs+_id, sizeof(long), &randomNum, sizeof(long));

	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		_shares[i] = new Share(*randomShares[i] + (*(long*)partiesInputs[i]));
	}
	//build circuit
	_arithmeticCircuit = new Circuit(_finalSeq, this);
	//release the memory which was allocated in fRand
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		delete randomShares[i];
	}
	////convert the share recieved from the other parties to Share and add it to the vector _shares
	//for (unsigned short i = 0; i < NUM_OF_PARTIES; i++) {
	//	Share* receivedShare = new Share((i+2)%NUM_OF_PARTIES, 'a' + i);
	//	(*receivedShare)[i].setValue(*(unsigned long*)partiesInputs[i]);
	//	(*receivedShare)[(i+2)%NUM_OF_PARTIES].setValue(*(unsigned long*)partiesInputs[i]);////fucked
	//	_shares[i] = receivedShare;
	//}
}
long Party::finalReconstruct(Share* myShare) {
	byte shareToSend[RECONSTRUCT_LEN]{};//index(4) value(4) name(1)
	return 0;

}
long Party::reconstruct(vector<Share*>& shares) {
	byte name = (*shares[_id])[_id].getName();
	byte rawData[NUM_OF_PARTIES][RECONSTRUCT_LEN];//the answers from the other parties
	byte sendShare[RECONSTRUCT_LEN];
	vector<Share*> otherShares;

	otherShares.resize(NUM_OF_PARTIES);
	//
	for (unsigned short i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == _id)
			continue;
		*(unsigned short*)sendShare = (*shares[i])[(_id+2)%NUM_OF_PARTIES].getIndex();
		*(long*)(sendShare + 2) = (*shares[i])[(_id + 2) % NUM_OF_PARTIES].getValue();
		sendShare[6] = (*shares[i])[(_id + 2) % NUM_OF_PARTIES].getName();
		
		*(unsigned short*)(sendShare+7) = (*shares[i])[_id].getIndex();
		*(long*)(sendShare + 9) = (*shares[i])[_id].getValue();
		sendShare[13] = (*shares[i])[_id].getName();
		sendTo(i,RECONSTRUCT,sendShare);
	}

	//read answers from other parties
	for (unsigned short i = 0; i <NUM_OF_PARTIES; i++) {
		if (i == _id) {
			otherShares[i] = nullptr;			
			continue;
		}
		readFrom(i, rawData[i]);//(index,value,name)
		otherShares[i] = new Share(*(unsigned short*)rawData[i], rawData[i][6]);
		(*otherShares[i])[(i+2)%NUM_OF_PARTIES] = *(long*)(rawData[i] + 2);//put the value recievied in the share
		(*otherShares[i])[i] = *(long*)(rawData[i] + 9);//put the value recievied in the share
	}
	bool isValid = false;
	switch (_id) {
	case 0:
		isValid = (*otherShares[1])[1].getValue() == (*otherShares[2])[1].getValue() && (*shares[_id])[0].getValue() == (*otherShares[1])[0].getValue() && (*shares[_id])[2].getValue() == (*otherShares[2])[2].getValue();
		break;
	case 1:
		isValid = (*otherShares[2])[2].getValue() == (*otherShares[0])[2].getValue() && (*shares[_id])[0].getValue() == (*otherShares[0])[0].getValue() && (*shares[_id])[1].getValue() == (*otherShares[2])[1].getValue();
		break;
	case 2:
		isValid = (*otherShares[1])[0].getValue() == (*otherShares[0])[0].getValue() && (*shares[_id])[1].getValue() == (*otherShares[1])[1].getValue() && (*shares[_id])[2].getValue() == (*otherShares[0])[2].getValue();
		break;
	}
	if (! isValid)
		throw std::exception(__FUNCTION__  "I'm surrounded by liers!");

	//perform validity check that the answers we got 
	/*for (int i = (_id+2)%NUM_OF_PARTIES, j = 0;j <=NUM_OF_PARTIES; j++,i++) {
		if (i  == (_id + 1) % NUM_OF_PARTIES) {
			if ((*otherShares[(_id + 2) % NUM_OF_PARTIES])[i].getValue() != (*otherShares[(_id + 1) % NUM_OF_PARTIES])[i].getValue())
				throw std::exception(__FUNCTION__ "I'm surrounded by liers!");
			continue;
		}
		if((*otherShares[_id])[i].getValue() != (*otherShares[(i+j)%NUM_OF_PARTIES])[i].getValue())
			throw std::exception(__FUNCTION__ "I'm surrounded by liers!");	
	}*/
	long ans = (*shares[_id])[(_id + 2) % NUM_OF_PARTIES].getValue() + (*shares[_id])[_id].getValue() + (*otherShares[(_id + 1) % NUM_OF_PARTIES])[(_id + 1) % NUM_OF_PARTIES].getValue();
	for (int i = 0; i < NUM_OF_PARTIES; i++)
		if (otherShares[i]) {
			delete otherShares[i];
			otherShares[i] = nullptr;
		}
	return ans;
}

Share* Party::getShare(int index) {
	if (index < NUM_OF_PARTIES) {
		return _shares[index];
	}
	throw std::exception(__FUNCTION__ "Index is invalid!");
}

void Party::setShare(Share* share, int index) {
	if (index < NUM_OF_PARTIES) {
		_shares[index] = share;
		return;
	}
	throw std::exception(__FUNCTION__ "Index is invalid!");

}
Share* Party::calcCircuit(){
	return _arithmeticCircuit->getOutput();
}
vector<Share*>& Party::getAllShares() { return _shares; }
