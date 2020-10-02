#include "Party.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "Circuit.h"
//NTL
#define NTL_NO_MIN_MAX
#include "NTL/ZZ_pX.h"
#include "NTL/vec_ZZ.h"
#include "NTL/lzz_p.h"
#include "NTL/tools.h"

#include <string>
#include <iostream>
#include <assert.h>
#include <condition_variable>

#define SUCCESS 1
#define POWER_TWO_MAX_RANGE 9
//std
using std::string;
using std::cout;
using std::endl;
//NTL
using NTL::ZZ_p;
using NTL::GenPrime;
using NTL::ZZ_pX;
using NTL::vec_ZZ_p;
using NTL::interpolate;
using NTL::RandomBnd;
using NTL::rep;
using NTL::vec_ZZ;
using NTL::VectorRandom;
using NTL::random;
using NTL::ZZ;
using NTL::vec_ZZ_pX;
using NTL::BytesFromZZ;
using NTL::vec_vec_ZZ_p;


Party::Party(short myID,long input):_id(myID),_input(input),_arithmeticCircuit(nullptr),_P(ZP){
	//for Dbug
	 srand(10);
	//GenPrime(p, POWER_TWO_MAX_RANGE);
	NTL::ZZ_p::init(_P);
	cout << "P="<<_P<<endl;
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
	if(messageType == F_VERIFY_ROUND1_MESSAGE)
		toSend.setSize(messageType, (_arithmeticCircuit->getNumOfMulGates()/L*2+6*L+1)*ELEMENT_SIZE);
	else
		toSend.setSize(messageType);
	toSend.setData(msg);
	_sockets[id]->writeBuffer(&toSend, HEADER_SIZE);
	_sockets[id]->writeBuffer(toSend.getData(), toSend.getSize());
	return true;
}
void Party::readFrom(unsigned short id,byte* msg) {
	std::condition_variable& other = _msgs[id]->getListenerCV();
	std::condition_variable& mine = _msgs[id]->getPartyCV();
	std::mutex& m = _msgs[id]->getIsReadMutex();
	std::unique_lock<std::mutex> partyUL(m);
	//wait until mutex is unlocked or for a new message to be received
	mine.wait(partyUL, [&] {return !this->_msgs[id]->getIsRead(); });
	memcpy(msg,_msgs[id]->getData(),_msgs[id]->getSize());
	other.notify_one();
	//mark message as read
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
	
	byte alpha[NUM_OF_PARTIES][KEY_LEN]{};
	byte fromKey[KEY_LEN]{};
	byte IV[KEY_LEN]{};
	AutoSeededRandomPool rnd;
	Share* ans = new Share((_id + 2) % NUM_OF_PARTIES, 'a' + calledThisFunc);
	//byte* test = new byte[ELEMENT_SIZE];
	_keys[_id] = new SecByteBlock(0x00,KEY_LEN);/////////////////////////////////TODO: change to byte*!
	//rnd.GenerateBlock(test,ELEMENT_SIZE);
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

	this->_gGatesInputs.resize(_arithmeticCircuit->getNumOfMulGates()* INPUTS_PER_G_GATE);

	//release the memory which was allocated in fRand
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		delete randomShares[i];
	}
	TRACE("Secret sharing completed.");
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
	assert(index >= 0 && index < _arithmeticCircuit->getNumOfMulGates()* INPUTS_PER_G_GATE);
	this->_gGatesInputs[index] = value;
}
Circuit* Party::getArithmeticCircuit()const {
	return _arithmeticCircuit;
}
void Party::fVerify() {
	TRACE("Start of verification stage.");
	unsigned int M = _arithmeticCircuit->getNumOfMulGates() / L;//as descussed in the pepare
	ZZ_pX p;
	vec_ZZ_p polynomialAtR;
	vec_vec_ZZ_p pointsToInterpolate;
	//-----Round 1-----:
	cout << "Round1:" << endl;
	verifyRound1(M, pointsToInterpolate,p);
	//-----Round 2-----:
	cout << "Round2:" << endl;
	verifyRound2(M, pointsToInterpolate,p, polynomialAtR);
	//-----Round 3-----:
	cout << "Round3:" << endl;
	verifyRound3(polynomialAtR);
	//----------------------------------------------------------------------------------
}
void Party::verifyRound1(unsigned int M, vec_vec_ZZ_p& pointsToInterpolate, ZZ_pX& p) {
	vec_ZZ_pX inputPolynomials;
	inputPolynomials.SetLength(INPUTS_PER_G_GATE * L);
	//(a)
	//generate L random elements from Zp and spread them to every party
	int numOfElements = L;
	vec_ZZ_p thetas;
	fCoin(thetas, numOfElements);
	//(b)
	//generate 6L random elements from Zp
	vec_ZZ_p omegas;
	omegas.SetLength(INPUTS_PER_G_GATE * L);
	for (int i = 0; i < INPUTS_PER_G_GATE * L; i++)
		random(omegas[i]);
	pointsToInterpolate.SetLength(INPUTS_PER_G_GATE * L);
	//(c)
	//prepare the vector of points to be interpolated
	for (int i = 0; i < INPUTS_PER_G_GATE*L; i++) {
		//set number of coeffients of every polynomial to be M+1
		pointsToInterpolate[i].SetLength( M + 1);
		//put the witness coeffient as the free coeffient
		pointsToInterpolate[i][0] = omegas[i];
		for (int j = 1; j < M+1 ; j++)
			pointsToInterpolate[i][j] = this->_gGatesInputs[(j-1) * INPUTS_PER_G_GATE * L + i].getValue();//t'th input ,j'th coefficient of the polynomial

		cout << "pointsToInterpolate(" << i << "):" << pointsToInterpolate[i] << endl;
	}
	interpolateInputPolynomials(M, INPUTS_PER_G_GATE * L, pointsToInterpolate,inputPolynomials);
	//inputPolynomials[4].SetLength(2*M + 1);///////////////TO BE DELETED

	for (int i = 0; i < INPUTS_PER_G_GATE * L; i++)
		std::cout << "f(" << i << ")" << inputPolynomials[i] << std::endl;
	//(d)
	p.SetLength(2 * M + 1);
	//for (int i = 0; i < INPUTS_PER_G_GATE * L; i++)
		p = inputPolynomials[0]* inputPolynomials[2]+ inputPolynomials[0]* inputPolynomials[3]+ inputPolynomials[1]* inputPolynomials[2]+ inputPolynomials[4]- inputPolynomials[5];
	std::cout << "p(x) = " << p << std::endl;
	//(e)
	Party* partyPtr = this;
	for(int currentNumOfMulGates =1; currentNumOfMulGates<M; currentNumOfMulGates++)
		cout<<" C is: "<< (partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 0].getValue() * partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 2].getValue() + partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 0].getValue() * partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 3].getValue() + partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 1].getValue() * partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 2].getValue() + partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 4].getValue() - partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 5].getValue()) % ZP;
	vec_ZZ_p PI;
	PI.SetLength(2 * M + 1 + INPUTS_PER_G_GATE * L);

	AutoSeededRandomPool rnd;
	byte* nextPI = new byte[(2 * M + 1 + 6 * L) * ELEMENT_SIZE]();
	rnd.GenerateBlock(nextPI, (2 * M + 1 + 6 * L) * ELEMENT_SIZE);
	
	//Sleep(2);
	sendTo((_id + 1) % NUM_OF_PARTIES, F_VERIFY_ROUND1_MESSAGE, nextPI);

	vec_ZZ_p beforePI;
	beforePI.SetLength(2 * M + 1 + INPUTS_PER_G_GATE * L);

	//add 6*L omegas to f
	for (int i = 0; i < INPUTS_PER_G_GATE * L; i++) {
		PI[i] = omegas[i];
	}
	//add 2*M + 1 coeficients to f
	for (int i = 0; i < 2 * M + 1; i++) 
		PI[i + INPUTS_PER_G_GATE * L] = p[i];

	for (int i = 0; i < 2 * M + 1 + INPUTS_PER_G_GATE * L; i++) {
		beforePI[i] = PI[i] - ZZ_p(nextPI[i]);
	}
	//send PI_+1
	byte* toSend = new byte[(2 * M + INPUTS_PER_G_GATE * L + 1) * ELEMENT_SIZE]{};
	ZZ bytesToZZ;
	for (int i = 0; i < 2 * M + INPUTS_PER_G_GATE * L + 1; i++) {
		byte rawZp[ELEMENT_SIZE]{};
		BytesFromZZ(rawZp, rep(beforePI[i]), ELEMENT_SIZE);
		*(unsigned long long*)& toSend[i * ELEMENT_SIZE] = *(unsigned long long*)rawZp;
	}
	sendTo((_id + 2) % NUM_OF_PARTIES, F_VERIFY_ROUND1_MESSAGE, toSend);

	//------------------- memory release section-------------------

	delete[] nextPI;
	nextPI = nullptr;

	delete[] toSend;
	toSend = nullptr;
}
void Party::verifyRound2(unsigned int M, vec_vec_ZZ_p& pointsToInterpolate, ZZ_pX& p, vec_ZZ_p& calculationForRound3) {
	byte* PIs[NUM_OF_PARTIES];
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == _id) {
			PIs[i] = nullptr;
		}
		else {//set Message's size to: 2*M+6*L+2 before receiving the data
			PIs[i] = new byte[ELEMENT_SIZE * (2 * M + 6 * L + 1)];
			std::condition_variable& other = _msgs[i]->getListenerIsSetSizeCV();
			std::condition_variable& mine = _msgs[i]->getPartyIsSetSizeCV();
			std::mutex& isSetSize = _msgs[i]->getIsSetSizeMutex();
			std::unique_lock<std::mutex> partyUL(isSetSize);
			mine.wait(partyUL, [&] {return !_msgs[i]->getIsSetSize(); });
			_msgs[i]->setSize(F_VERIFY_ROUND1_MESSAGE, (2 * M + 6 * L + 1) * ELEMENT_SIZE);
			other.notify_one();
			_msgs[i]->setIsSetSize(true);
		}
	}

	readFrom((_id + 1) % NUM_OF_PARTIES, PIs[(_id + 1) % NUM_OF_PARTIES]);
	readFrom((_id + 2) % NUM_OF_PARTIES, PIs[(_id + 2) % NUM_OF_PARTIES]);

	vector<vec_ZZ_p> parsedPIs;
	parsedPIs.resize(NUM_OF_PARTIES);
	//(a)
	vec_ZZ_p bettas;
	fCoin(bettas, M);
	vec_ZZ_p r;
	r.SetLength(1);
	do {
		fCoin(r, 1);
	} while (rep(r[0]) <= M);
	cout << "r = " << r << endl;
	//convert the message recevied to ZZ_p elements
	for (int i = 0; i < NUM_OF_PARTIES; i++)
		if (i == _id)
			continue;
		else {
			//parse the data received
			rawDataToVec(parsedPIs[i], (2 * M + INPUTS_PER_G_GATE * L + 1), PIs[i]);
			cout << i << ".received PI :" << parsedPIs[i] << endl;
			ZZ_pX func;
			func.SetLength(2 * M + 1);
			//conv(func, parsedPIs[i]);
			for (int j = 0; j < 2 * M + 1; j++)
				func[j] = parsedPIs[i][j + 6 * L];
			cout << "func: " << func << endl;;
			cout << "P(r) = " << NTL::eval(func, r) << endl;;
		}
	//(b)
	vector<ZZ_p> b;
	b.resize(NUM_OF_PARTIES);

	vector<vec_ZZ_pX> polynomialsRound2;
	polynomialsRound2.resize(NUM_OF_PARTIES);

	vector<vec_vec_ZZ_p> pointsToInterpolateRound2;
	pointsToInterpolateRound2.resize(NUM_OF_PARTIES);

	//set length for each polynomials and update omega according to the rellevant PI message
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i != _id) {
			polynomialsRound2[i].SetLength(INPUTS_PER_G_GATE*L);
			pointsToInterpolateRound2[i].SetLength(INPUTS_PER_G_GATE*L);
			for (int j = 0; j < INPUTS_PER_G_GATE * L; j++) {
				pointsToInterpolateRound2[i][j].SetLength( M + 1);
				pointsToInterpolateRound2[i][j][0] = parsedPIs[i][j];
				for (int k = 1; k <  M + 1; k++)
					pointsToInterpolateRound2[i][j][k] = pointsToInterpolate[j][k];
			}
			interpolateInputPolynomials(M, INPUTS_PER_G_GATE * L, pointsToInterpolateRound2[i], polynomialsRound2[i]);

		}
	}

	//store every polynomials result in f_r
	vector<vec_ZZ_p> f_r;
	f_r.resize(NUM_OF_PARTIES);
	vector<ZZ_p> p_r;
	p_r.resize(NUM_OF_PARTIES);
	//computes f(r) for every each polynomial from the 6L polynomials 
	for (int i = 0; i < NUM_OF_PARTIES; i++)
		for (int j = 0; j < M + 1; j++)
			if (i != _id) {//f_r
				f_r[i].SetLength(INPUTS_PER_G_GATE * L);
				for (int k = 0; k < INPUTS_PER_G_GATE * L; k++)
					f_r[i][k] = eval(polynomialsRound2[i][k], r[0]);
			}
	//computes p(r) and b for every party
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i != _id)
		{
			for (int j = 0; j < 2 * M + 1; j++)
				p_r[i] += parsedPIs[i][j + INPUTS_PER_G_GATE * L] * power(r[0], j);
			for (int j = 1; j < M +1; j++) {
				for (int k = 0; k < 2 * M + 1; k++)
					b[i] += parsedPIs[i][k + INPUTS_PER_G_GATE * L] * power(ZZ_p(j), k);
				b[i] *= bettas[j-1];//multiply each of the M g gate results with beta.
			}
		}
	}

	byte toSend[(INPUTS_PER_G_GATE * L + 2) * ELEMENT_SIZE]{};
	calculationForRound3.SetLength(INPUTS_PER_G_GATE * L + 2);
	//send message to id-1 and save calculations for round 3
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == (_id + 2) % NUM_OF_PARTIES)
		{
			for (int j = 0; j < INPUTS_PER_G_GATE * L; j++)
				BytesFromZZ(toSend + ELEMENT_SIZE*j, rep(f_r[i][j]), ELEMENT_SIZE);
			//send the polynomials at point r and b to party i-1
			BytesFromZZ(toSend + ELEMENT_SIZE * INPUTS_PER_G_GATE * L, rep(b[i]), ELEMENT_SIZE);
			BytesFromZZ(toSend + ELEMENT_SIZE *( INPUTS_PER_G_GATE * L+1), rep(p_r[i]), ELEMENT_SIZE);
			sendTo((_id + 2) % NUM_OF_PARTIES, F_VERIFY_ROUND2_MESSAGE, toSend);
		}
		else if(i==(_id+1)%NUM_OF_PARTIES) {
			//save the polynonials at point r for round 3
			for (int j = 0; j < INPUTS_PER_G_GATE * L; j++)
				calculationForRound3[j] = f_r[i][j];
			calculationForRound3[INPUTS_PER_G_GATE * L] = p_r[i];
			calculationForRound3[INPUTS_PER_G_GATE * L + 1] = b[i];
		}
	}
}
void Party::fCoin(vec_ZZ_p& thetas, int numOfElements)
{
	thetas.SetLength(numOfElements);
	//generate numOfElements random numbers 
	for (int i = 0; i < numOfElements; i++) {
		Share* randomShare = fRand();
		thetas[i] = ZZ_p(finalReconstruct(*randomShare));
		delete randomShare;
	}
}

void Party::interpolateInputPolynomials(unsigned int polynomialsDegree, unsigned int numOfPolynomials, vec_vec_ZZ_p& pointsToInterpolate,vec_ZZ_pX& inputPolynomials)const
{
	//build a sequance of points from 0 to M
	vec_ZZ_p range;
	range.SetLength(polynomialsDegree + 1);
	for (int i = 0; i < polynomialsDegree + 1; i++)
		range[i] = i;
	for (int i = 0; i < numOfPolynomials; i++) {
		interpolate(inputPolynomials[i], range, pointsToInterpolate[i]);
	}
}
void Party::verifyRound3(vec_ZZ_p& polynomialsAtPointR){
	byte buffer[(INPUTS_PER_G_GATE * L + 2) * ELEMENT_SIZE]{};
	vec_ZZ_p parsedFinal;
	vec_ZZ_p constructedElements;

	parsedFinal.SetLength(INPUTS_PER_G_GATE * L + 2);
	constructedElements.SetLength(INPUTS_PER_G_GATE * L + 2);
	readFrom((_id+1)%NUM_OF_PARTIES, buffer);
	rawDataToVec(parsedFinal, INPUTS_PER_G_GATE * L + 2, buffer);

	for (int j = 0; j < F_VERIFY_ROUND2_MESSAGE_LEN; j++)
		constructedElements[j] = parsedFinal[j] + polynomialsAtPointR[j];
	cout << "Receved: " << parsedFinal << endl;
	cout << "Final construction:"<<constructedElements;
	cout << "g(f(r)) = " << cFunction(constructedElements);
}
void Party::rawDataToVec(vec_ZZ_p& vec, unsigned int vectorLen, byte* rawData) {
	vec.SetLength(vectorLen);
	for (int j = 0; j < vectorLen; j++) {
		ZZ temp;
		ZZFromBytes(temp, &rawData[j * ELEMENT_SIZE], ELEMENT_SIZE);
		conv(vec[j], temp);
	}
	cout << endl;
}
ZZ_p Party::cFunction(vec_ZZ_p inputsToGGate)const {
	return inputsToGGate[0] * inputsToGGate[2] + inputsToGGate[0] * inputsToGGate[3] + inputsToGGate[1] * inputsToGGate[2] + inputsToGGate[4] - inputsToGGate[5];

}
ZZ_p Party::setFunctionAtPoint(const ZZ_pX& function,ZZ_p functionDegree, ZZ_p point)const {
	ZZ_p ans;
	for(int i=0;i<rep(functionDegree)+1;i++)
		ans += function[i] * power(point, i);
	return ans;
}