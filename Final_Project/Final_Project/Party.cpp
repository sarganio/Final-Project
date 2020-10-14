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
//void Party::printKey(unsigned short index)const {
//	if (index < 0 || index > NUM_OF_PARTIES)
//		return;
//	string a((char*)_keys[index]->begin(), KEY_LEN);
//	for (int i = 0; i < a.length(); i++)
//		printf("%u ", (byte)a[i]);
//}
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

	TRACE("Succssesfully connected to all parties!");

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
		byte* toFree = _keys.back();
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
	byte IV[KEY_LEN]{};
	Share* ans = new Share((_id + 2) % NUM_OF_PARTIES, 'a' + calledThisFunc);
	AutoSeededRandomPool rnd;
	_keys[_id] = new byte[KEY_LEN]();
	rnd.GenerateBlock(_keys[_id],KEY_LEN);
	
	for (unsigned int i = 0; i < KEY_LEN / SEQ_LEN; i++)
		*(unsigned int*)(IV+i * SEQ_LEN) = *(unsigned int*)_finalSeq;
		//*(unsigned int*)(IV + i * SEQ_LEN) = *(unsigned int*)_finalSeq;

	//send this party key to the next party
	sendTo((_id + 1) % NUM_OF_PARTIES,KEY, _keys[_id]);
	_keys[(_id + 2) % NUM_OF_PARTIES] = new byte[KEY_LEN]();
	readFrom((_id + 2) % NUM_OF_PARTIES, _keys[(_id + 2) % NUM_OF_PARTIES]);


	for (unsigned short i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == (_id + 1)%NUM_OF_PARTIES)
			continue;
		memcpy_s(alpha[i], sizeof(int), &_finalSeq, sizeof(int));
		SecByteBlock key(_keys[i], KEY_LEN);
		Helper::encryptAES(alpha[i], KEY_LEN,key,IV); 
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
	//increment SEQ
	*(unsigned int*)_finalSeq += 1;
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
	_multipicationGateOutputs.resize(_arithmeticCircuit->getNumOfMulGates());

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
	unsigned int M = std::ceil(_arithmeticCircuit->getNumOfMulGates() / double(L));//as descussed in the pepare
	ZZ_pX p;
	vec_ZZ_p thetas;
	vec_ZZ_p polynomialAtR;
	vec_vec_ZZ_p pointsToInterpolate;
	//-----Round 1-----:
	cout << "\n-----Round 1-----" << endl << endl;
	verifyRound1(thetas,M, pointsToInterpolate,p);
	//-----Round 2-----:
	cout << "\n-----Round 2-----" << endl << endl;
	verifyRound2(M, pointsToInterpolate,p, polynomialAtR);
	//-----Round 3-----:
	cout << "\n-----Round 3-----" << endl << endl;
	verifyRound3(thetas,polynomialAtR);
	//----------------------------------------------------------------------------------
}
void Party::verifyRound1(vec_ZZ_p& thetas,unsigned int M, vec_vec_ZZ_p& pointsToInterpolate, ZZ_pX& p) {
	vec_ZZ_pX inputPolynomials;
	inputPolynomials.SetLength(INPUTS_PER_G_GATE * L);
	//(a)
	//generate L random elements from Zp and spread them to every party
	int numOfElements = L;
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
	for (int i = 0; i < INPUTS_PER_G_GATE * L; i++) {
		//set number of coeffients of every polynomial to be M+1
		pointsToInterpolate[i].SetLength(M + 1);
		//put the witness coeffient as the free coeffient
		pointsToInterpolate[i][0] = omegas[i];
		for (int j = 1; j < M + 1; j++) {
			if ((j - 1) * INPUTS_PER_G_GATE * L + i >= _gGatesInputs.capacity())
				pointsToInterpolate[i][j] = 0;
			else
				pointsToInterpolate[i][j] = this->_gGatesInputs[(j - 1) * INPUTS_PER_G_GATE * L + i].getValue();//j'th input ,j'th coefficient of the polynomial
		}
		cout << "pointsToInterpolate(" << i << "):" << pointsToInterpolate[i] << endl;
	}
	cout << endl;

	interpolateInputPolynomials(M, INPUTS_PER_G_GATE * L, pointsToInterpolate,inputPolynomials);

	for (int i = 0; i < INPUTS_PER_G_GATE * L; i++)
		std::cout << "f" << i << "(x)" << inputPolynomials[i] << std::endl;
	//(d)
	p.SetLength(2 * M + 1);
	gGateOutput(thetas, inputPolynomials, p);
	std::cout << "p(x) = " << p << std::endl;
	//(e)
	unsigned int orderOfPI = 2 * M + 1 + INPUTS_PER_G_GATE * L;
	vec_ZZ_p PI;
	PI.SetLength(orderOfPI);
	//generate random PI to send to party i+1
	AutoSeededRandomPool rnd;
	byte* nextPI = new byte[orderOfPI * ELEMENT_SIZE]();
	rnd.GenerateBlock(nextPI, orderOfPI * ELEMENT_SIZE);

	//convert the random data in next pie to vector of ZZ_p elements
	vec_ZZ_p nextPI_ZZ_p;
	rawDataToVec(nextPI_ZZ_p, orderOfPI, nextPI);
	cout << "nextPI:" << nextPI_ZZ_p << endl;

	//send PI_i+1 to i+1
	vecToRawData(orderOfPI, nextPI_ZZ_p, nextPI);
	
	sendTo((_id + 1) % NUM_OF_PARTIES, F_VERIFY_ROUND1_MESSAGE, nextPI);

	vec_ZZ_p beforePI;
	beforePI.SetLength(orderOfPI);

	//add 6*L omegas to f
	for (int i = 0; i < INPUTS_PER_G_GATE * L; i++) {
		PI[i] = omegas[i];
	}
	//add 2*M + 1 coeficients to f
	for (int i = 0; i < 2 * M + 1; i++) 
		PI[i + INPUTS_PER_G_GATE * L] = p[i];

	for (int i = 0; i < orderOfPI; i++) {
		beforePI[i] = PI[i] - nextPI_ZZ_p[i];
	}
	//send PI_+1
	byte* toSend = new byte[orderOfPI * ELEMENT_SIZE]{};
	vecToRawData(orderOfPI, beforePI, toSend);
	sendTo((_id + 2) % NUM_OF_PARTIES, F_VERIFY_ROUND1_MESSAGE, toSend);

	//------------------- memory release section-------------------

	delete[] nextPI;
	nextPI = nullptr;

	delete[] toSend;
	toSend = nullptr;
}

void Party::verifyRound2(unsigned int M, vec_vec_ZZ_p& pointsToInterpolate, ZZ_pX& p, vec_ZZ_p& calculationForRound3) {
	byte* PIs[NUM_OF_PARTIES];
	unsigned int orderOfPI = 2 * M + INPUTS_PER_G_GATE * L + 1;
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == _id) {
			PIs[i] = nullptr;
		}
		else {//set Message's size to: 2*M+6*L+2 before receiving the data
			PIs[i] = new byte[ELEMENT_SIZE * orderOfPI]();
			std::condition_variable& other = _msgs[i]->getListenerIsSetSizeCV();
			std::condition_variable& mine = _msgs[i]->getPartyIsSetSizeCV();
			std::mutex& isSetSize = _msgs[i]->getIsSetSizeMutex();
			std::unique_lock<std::mutex> partyUL(isSetSize);
			mine.wait(partyUL, [&] {return !_msgs[i]->getIsSetSize(); });
			_msgs[i]->setSize(F_VERIFY_ROUND1_MESSAGE, orderOfPI * ELEMENT_SIZE);
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
	vector<ZZ_p> p_r;
	p_r.resize(NUM_OF_PARTIES);
	//convert the message recevied to ZZ_p elements
	for (int i = 0; i < NUM_OF_PARTIES; i++)
		if (i == _id)
			continue;
		else {
			//parse the data received
			rawDataToVec(parsedPIs[i], orderOfPI, PIs[i]);
			//release PIs memory
			delete[] PIs[i];
			PIs[i] = nullptr;
			cout <<"Received PI from ID="<<i<<" :" << parsedPIs[i] << endl;
			ZZ_pX p;
			p.SetLength(2 * M + 1);
			//conv(func, parsedPIs[i]);
			for (int j = 0; j < 2 * M + 1; j++)
				p[j] = parsedPIs[i][j + 6 * L];
			cout << "p(x): " << p << endl;;
			 p_r[i] = NTL::eval(p, r[0]);
			 cout << "p(r) = " << p_r[i] << endl;
		}
	//(b)
	vector<ZZ_p> b;
	b.resize(NUM_OF_PARTIES);

	vector<vec_ZZ_pX> polynomialsRound2;
	polynomialsRound2.resize(NUM_OF_PARTIES);

	vector<vec_vec_ZZ_p> pointsToInterpolateRound2;
	pointsToInterpolateRound2.resize(NUM_OF_PARTIES);

	//set length for each polynomials and update omega according to the rellevant PI message
	for (int i = 0; i < NUM_OF_PARTIES; i++) 
		if (i != _id) {
			polynomialsRound2[i].SetLength(INPUTS_PER_G_GATE*L);
			pointsToInterpolateRound2[i].SetLength(INPUTS_PER_G_GATE*L);
			for (int j = 0; j < INPUTS_PER_G_GATE * L; j++) {
				pointsToInterpolateRound2[i][j].SetLength( M + 1);
				//copy inputs from previous round
				for (int k = 1; k < M + 1; k++) 
					pointsToInterpolateRound2[i][j][k] = pointsToInterpolate[j][k];
			}
			if (i == (_id + 2) % NUM_OF_PARTIES) {
				for(int l=1;l<L+1;l++)//l'th g gate
					for (int k = 1; k < M + 1; k++)//k'th coeffient
						pointsToInterpolateRound2[i][5*l][k] = getMultipicationOutput(k - 1).getValue();
				orderInputVector(pointsToInterpolateRound2[i], i);
			}
			else
				orderInputVector(pointsToInterpolateRound2[i], i);
			//set omegas as the fisrt point
			for(int j=0;j<INPUTS_PER_G_GATE*L;j++)
				pointsToInterpolateRound2[i][j][0] = parsedPIs[i][j];
			
			interpolateInputPolynomials(M, INPUTS_PER_G_GATE * L, pointsToInterpolateRound2[i], polynomialsRound2[i]);
		}

	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == (_id + 2) % NUM_OF_PARTIES) {
			cout << "Input point of ID=" << (i + 1) % NUM_OF_PARTIES << ", the prover is " << i << endl;
		}
		if (i == (_id + 1) % NUM_OF_PARTIES) {
			cout << "Input point of ID=" << (i+2)%NUM_OF_PARTIES << ", the prover is " << i << endl;
		}
		printVecVec(pointsToInterpolateRound2[i]);
	}
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == (_id + 2) % NUM_OF_PARTIES) {
			cout << "f(x) created from ID=" << (i + 1) % NUM_OF_PARTIES << " inputs, the prover is " << i << endl;
			for (int j = 0; j < INPUTS_PER_G_GATE * L; j++)
				std::cout << "f" << j << "(x)" << polynomialsRound2[i][j] << std::endl;
		}
		if (i == (_id + 1) % NUM_OF_PARTIES) {
			cout << "f(x) created from ID=" << (i + 2) % NUM_OF_PARTIES << " inputs, the prover is " << i << endl;
			for (int j = 0; j < INPUTS_PER_G_GATE * L; j++)
				std::cout << "f" << j << "(x)" << polynomialsRound2[i][j] << std::endl;
		}
	}

	//store every polynomials result in f_r
	vector<vec_ZZ_p> f_r;
	f_r.resize(NUM_OF_PARTIES);

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
			for (int j = 1; j < M +1; j++) {
				for (int k = 0; k < 2 * M + 1; k++)
					b[i] += parsedPIs[i][k + INPUTS_PER_G_GATE * L] * power(ZZ_p(j), k);
				b[i] *= bettas[j-1];//multiply each of the M g gate results with beta.
			}
			cout << b[i] << endl;
		}
	}

	byte toSend[(INPUTS_PER_G_GATE * L + 2) * ELEMENT_SIZE]{};
	calculationForRound3.SetLength(INPUTS_PER_G_GATE * L + 2);
	//send message to id-1 and save calculations for round 3
	for (int i = 0; i < NUM_OF_PARTIES; i++) {
		if (i == (_id + 1) % NUM_OF_PARTIES)
		{
			for (int j = 0; j < INPUTS_PER_G_GATE * L; j++)
				BytesFromZZ(toSend + ELEMENT_SIZE*j, rep(f_r[i][j]), ELEMENT_SIZE);
			//send the polynomials at point r and b to party i-1
			BytesFromZZ(toSend + ELEMENT_SIZE * INPUTS_PER_G_GATE * L, rep(p_r[i]), ELEMENT_SIZE);
			BytesFromZZ(toSend + ELEMENT_SIZE *( INPUTS_PER_G_GATE * L+1), rep(b[i]), ELEMENT_SIZE);
			cout << "calculted from ID=" << (i + 2) % NUM_OF_PARTIES << " inputs , the prover is " << i << endl;
			cout <<"f(r)"<< f_r[i] << endl;//i+2 => _id+4 =>_id + 1
			cout << "b :" << b[i] << endl;
			cout << "p(r) :" << p_r[i] << endl;
			sendTo((i + 1) % NUM_OF_PARTIES, F_VERIFY_ROUND2_MESSAGE, toSend);
		}
		else if (i == (_id + 2) % NUM_OF_PARTIES) {
			cout << "calculted from ID=" << (i + 1) % NUM_OF_PARTIES << " inputs , the prover is " << i << endl;
			cout << "f(r)" << f_r[i] << endl;
			cout << "b :" << b[i] << endl;
			cout << "p(r) :" << p_r[i] << endl;
			//save the polynonials at point r for round 3
			for (int j = 0; j < INPUTS_PER_G_GATE * L; j++)
				calculationForRound3[j] = f_r[i][j];
			//save the p(r) for round 3
			calculationForRound3[INPUTS_PER_G_GATE * L] = p_r[i];
			//save the b for round 3
			calculationForRound3[INPUTS_PER_G_GATE * L + 1] = b[i];
		}
	}
}
void Party::printVecVec(NTL::vec_vec_ZZ_p& pointsToInterpolateRound2)const
{
	for (int j = 0; j < pointsToInterpolateRound2.length(); j++)
		cout << "pointsToInterpolate(" << j << "):" << pointsToInterpolateRound2[j] << endl;
}
void Party::fCoin(vec_ZZ_p& thetas, unsigned int numOfElements)
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
void Party::verifyRound3(vec_ZZ_p& thetas,vec_ZZ_p& polynomialsAtPointR){
	byte buffer[(INPUTS_PER_G_GATE * L + 2) * ELEMENT_SIZE]{};
	vec_ZZ_p parsedFinal;
	vec_ZZ_p constructedElements;
	ZZ_p g_r;

	parsedFinal.SetLength(INPUTS_PER_G_GATE * L + 2);
	constructedElements.SetLength(INPUTS_PER_G_GATE * L + 2);
	readFrom((_id+1)%NUM_OF_PARTIES, buffer);
	rawDataToVec(parsedFinal, INPUTS_PER_G_GATE * L + 2, buffer);

	for (int j = 0; j < F_VERIFY_ROUND2_MESSAGE_LEN/ELEMENT_SIZE; j++)
		constructedElements[j] = parsedFinal[j] + polynomialsAtPointR[j];
	cout << "Received: " << parsedFinal << endl << endl;
	cout << "[f'1(r),f'2(r),f'3(r),f'4(r),f'5(r),f'6(r),p(r),b(r)]" << endl;
	cout << "Final construction:"<<constructedElements<<endl;
	gGateOutput<vec_ZZ_p, ZZ_p>(thetas, constructedElements, g_r);
	cout << "g(f(r)) = " << g_r << endl;
}
void Party::rawDataToVec(vec_ZZ_p& vec, unsigned int vectorLen, byte* rawData) {
	vec.SetLength(vectorLen);
	for (int j = 0; j < vectorLen; j++) {
		ZZ temp;
		ZZFromBytes(temp, &rawData[j * ELEMENT_SIZE], ELEMENT_SIZE);
		conv(vec[j], temp);
	}
}
void Party::vecToRawData(unsigned int numOfElements, NTL::vec_ZZ_p& vec, byte* rawData)const
{
	for (int i = 0; i < numOfElements; i++) {
		byte rawZp[ELEMENT_SIZE]{};
		BytesFromZZ(rawZp, rep(vec[i]), ELEMENT_SIZE);
		*(unsigned long long*)& rawData[i * ELEMENT_SIZE] = *(unsigned long long*)rawZp;
	}
}


Part& Party::getMultipicationOutput(unsigned short index){
	return _multipicationGateOutputs[index];
}
void Party::setMultipicationOutput(Part& toSave) {
	static int numOfElements = 0;
	_multipicationGateOutputs[numOfElements++] = toSave;
}
void Party::orderInputVector(vec_vec_ZZ_p& inputVector, unsigned short proverIndex) {
	if (proverIndex == (_id + 1) % NUM_OF_PARTIES) {
		inputVector[1] = inputVector[0];
		NTL::clear(inputVector[0]);

		inputVector[3] = inputVector[2];
		NTL::clear(inputVector[2]);

		inputVector[4] *= -1;

		NTL::clear(inputVector[5]);
	}
	if (proverIndex == (_id + 2) % NUM_OF_PARTIES) {
		inputVector[0] = inputVector[1];
		NTL::clear(inputVector[1]);

		inputVector[2] = inputVector[3];
		NTL::clear(inputVector[3]);

		inputVector[4] *= -1;

	}
}