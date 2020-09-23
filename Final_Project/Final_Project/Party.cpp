#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "Circuit.h"
#include <string>
#include <iostream>
#include <assert.h>

#define SUCCESS 1

using std::string;
using std::cout;
using std::endl;

Party::Party(short myID,long input):_id(myID),_input(input),_arithmeticCircuit(nullptr){
	//for Dbug
	 srand(10);
	 //make sure the input belong to the Ring Z_p 
	 _input %= ZP;
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
			//delete toFree; HEAP CORRUPTION-to check:maybe change to delete[]
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

	TRACE("SEQ = %u", *(unsigned int*)_finalSeq);
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
		(*(long*)alpha[i]) %= ZP;
		(*ans)[i] = (*(long*)alpha[i])>0?*(long*)alpha[i]: (*(long*)alpha[i])+ZP;
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
	randomNum = _input + (ZP - randomNum);
	randomNum %= ZP;
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

	this->_gGatesInputs.resize(_arithmeticCircuit->getNumOfMulGates()* INPUTS_PER_MUL_GATE);

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
long Party::finalReconstruct(Share& myShare) {
	vector<Share*> outputShares;
	long ans = 0;
	outputShares.resize(NUM_OF_PARTIES);

	//send the final output share to the other parties
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i != _id) {
			sendShareTo(i, myShare);
		}
	}
	//reciecve the shares of the final output
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i != _id)
			outputShares[i] = &RecieveShareFrom(i);
		else
			outputShares[i] = &myShare;
	}

	//sum all shares to get result
	for (int i = 0; i < NUM_OF_PARTIES; i++)
		ans += (*outputShares[i])[i].getValue();
	ans %= ZP;
	return ans;
}
void Party::sendShareTo(unsigned short id, Share& toSend)const {
	byte sendShare[RECONSTRUCT_LEN];
	*(unsigned short*)sendShare = toSend[(_id + 2) % NUM_OF_PARTIES].getIndex();
	*(long*)(sendShare + 2) = toSend[(_id + 2) % NUM_OF_PARTIES].getValue();
	sendShare[6] = toSend[(_id + 2) % NUM_OF_PARTIES].getName();

	*(unsigned short*)(sendShare + 7) = toSend[_id].getIndex();
	*(long*)(sendShare + 9) = toSend[_id].getValue();
	sendShare[13] = toSend[_id].getName();
	sendTo(id, RECONSTRUCT, sendShare);
}
Share& Party::RecieveShareFrom(unsigned short id) {
	byte rawData[RECONSTRUCT_LEN]{};//the answers from the other parties
	readFrom(id, rawData);//(index,value,name)
	Share& ans = *new Share(*(unsigned short*)rawData, rawData[6]);
	ans[(id + 2) % NUM_OF_PARTIES] = *(long*)(rawData + 2);//put the value recievied in the share
	ans[id] = *(long*)(rawData+ 9);//put the value recievied in the share
	return ans;
}
long Party::reconstruct(vector<Share*>& shares) {
	byte name = (*shares[_id])[_id].getName();
	vector<Share*> otherShares;

	otherShares.resize(NUM_OF_PARTIES);
	//
	for (unsigned short i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == _id)
			continue;
		sendShareTo(i, *shares[i]);
	}

	//read answers from other parties
	for (unsigned short i = 0; i <NUM_OF_PARTIES; i++) {
		if (i == _id) {
			otherShares[i] = nullptr;			
			continue;
		}
		otherShares[i] = &RecieveShareFrom(i);
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
	ans %= ZP;
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
void Party::setG_GateInput(unsigned short index, Part value) {
	assert(index >= 0 && index < _arithmeticCircuit->getNumOfMulGates()* INPUTS_PER_MUL_GATE);
	this->_gGatesInputs[index] = value;
}
Circuit* Party::getArithmeticCircuit()const {
	return _arithmeticCircuit;
}
void Party::fVerify() {
	//-----Round 1-----:
	verifyRound1();
	//-----Round 2-----:


	//----------------------------------------------------------------------------------
	//real_1d_array x = "[0,1,2,3,4,5,6,7,8,9]";
	//real_1d_array y = "[0,0,2,6,12,20,30,42,56,72]";
	//barycentricinterpolant p;
	//real_1d_array a;
	//double v;

	//// barycentric model is created
	//polynomialbuild(x, y, p);
	//v = barycentriccalc(p, 10);
	////
 // // a=[0,-1,+1] is decomposition of y=x^2-x in the power basis:
 // //
 // //     y = 0 - 1*x + 1*x^2
 // //
 // // We convert it to the barycentric form.
 // //
	//polynomialbar2pow(p, a);
	//for (int i = 0; i < 10; i++)
	//	if (i != 9)
	//		std::cout <<"("<< a[i] << ")x^" << i << "+";
	//	else
	//		std::cout << "(" << a[i] << ")x^" << i;



}
void Party::verifyRound1() {
	//(a)
	vector<int> thetas;
	thetas.resize(L);
	//generate L random numbers 
	for (int i = 0; i < L; i++) {
		Share* randomShare = fRand();
		thetas.push_back(finalReconstruct(*randomShare));
		delete randomShare;
	}
	//(b)
	AutoSeededRandomPool rnd;
	vector<SecByteBlock*> omegas;
	omegas.resize(6 * L);
	for (int i = 0; i < 6 * L; i++)
		omegas.push_back(new SecByteBlock(0x00, sizeof(int)));//////////TODO:free memory
	//(c)
	int* inputPolynomials[6 * L]{};
	unsigned int M = _arithmeticCircuit->getNumOfMulGates() / L;//as descussed in the pepare
	for (int i = 0; i < 6 * L; i++) {
		inputPolynomials[i] = new int[M + 1];
		inputPolynomials[i][0] = (int)omegas[i];
		for (int j = 0; j < M; j++)
			inputPolynomials[i][j+1] = this->_gGatesInputs[j * 6 * L].getValue();//t'th input ,j'th coefficient of the polynomial
	}
	//(d)interpulation reqauired
	int* pCoefficients = new int[2 * M]{};
	//(e)
	int* PI = new int[2 * M + 1 + 6 * L]{};
	int* nextPI = (int*)new SecByteBlock(0x00, (2 * M + 1 + 6 * L) * sizeof(int));//may be problematic
	int* beforePI = new int[2 * M + 1 + 6 * L]{};

	//add 6*L omegas to f
	for (int i = 0; i < 6 * L; i++) {
		PI[i] = (int)omegas[i];
	}
	//add 2*M + 1 coeficients to f
	for (int i = 0; i <= 2 * M; i++) {
		PI[i] = pCoefficients[6 * L + i];
	}
	for (int i = 0; i < 2 * M + 1 + 6 * L; i++) {
		beforePI[i] = PI[i] - nextPI[i];
	}
	sendTo((_id + 1) % NUM_OF_PARTIES, PROOF_MESSAGE, (byte*)nextPI);
	sendTo((_id + 2) % NUM_OF_PARTIES, PROOF_MESSAGE, (byte*)beforePI);

	//-------------------release memory section-------------------
	thetas.clear();
	thetas.shrink_to_fit();
	
	for (int i = 0; i < 6 * L; i++) {
		delete[] inputPolynomials[i];
		inputPolynomials[i] = nullptr;
	}
	delete[] pCoefficients;
	pCoefficients = nullptr;

	delete[] beforePI;
	beforePI = nullptr;

	delete[] PI;
	PI = nullptr;

	delete nextPI;
	nextPI = nullptr;
}