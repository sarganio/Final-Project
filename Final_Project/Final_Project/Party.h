#pragma once
#include "TcpSocket.h"
#include "Helper.h"
#include "Messages.h"

#include "Share.h"
#define NTL_NO_MIN_MAX
#include "NTL/vec_ZZ_p.h"
#include <vector>
#include <string>
#include <mutex>

#define INPUTS_PER_MUL_GATE  6
#define ELEMENT_SIZE sizeof(ZZ_p)
//std
using std::vector;
using std::pair;
using std::string;
using std::mutex;
//NTL
using NTL::ZZ_p;
using NTL::ZZ;
using NTL::ZZ_pX;
using NTL::vec_ZZ_p;

//class Share;
class Circuit;

class Party
{
private:
	ZZ _P;
	unsigned short _id;												//unique identifier, lowest ip receive id = 1 and so on.
	long _input;													//secret input.
	byte _finalSeq[SEQ_LEN]{};										//mutual counter for all participents.
	vector<TcpSocket*> _sockets;									//socket for all TCP connection with other parties.
	vector<Message*>_msgs;											//vector which contains all recieved messages
	vector<SecByteBlock*>_keys;										//vector which contains all the keys required for generating random numbers
	vector<Share*> _shares;											//vector which contains each party's share.index of vector is the id of input's party
	Circuit* _arithmeticCircuit;									//the function which needs to be computed.
	void printKey(unsigned short index)const;						//recieves the index of the key's owner and print it.
	vector<Part> _gGatesInputs;										//vector which contains all g gates inputs for verification stage.
public:
	void setG_GateInput(unsigned short index, Part value);
	Party(short myID, long input);									//C'tor-takes an ID and a secret input as parameters
	bool sendTo(unsigned short id, byte messageType,byte* msg)const;//send message to party[id]
	void readFrom(unsigned short id,byte* msg);						//read message from party number id. this is a blocking function.
	void connectToAllParties(string IPs[NUM_OF_PARTIES]);			//connect to all 3 parties
	void broadcast(byte * msg, byte messageType)const;				//send message to all parties
	Share* getShare(int index);										//getter by index for the shares held by this party.
	void setShare(Share* share, int index);							//setter by index for the shares held by this party.
	unsigned short getID()const;									//getter for this party ID
	void calcSeq();													//calculate the joint seq. Initiates at the begining of the protocol.
	Share* calcCircuit();											//calculate the output of all the gates in the circuit
	Share* fRand();													//fRand functuality as described in the paper.
	void fInput();													//fInput functuality as described in the paper.
	long reconstruct(vector<Share*>& myShare);						//receives all the Parts of a given share.
	//vector<Share*>& getAllShares();								
	void verifyRound1(unsigned int M, vector<ZZ_pX>& inputPolynomials, ZZ_pX& p);//the first round of fVerify as described in the paper.
	long finalReconstruct(Share&);									//function used at the end of the protocol, recieve shares from other parties and return the result.
	Share& RecieveShareFrom(unsigned short id);						//read raw data of share from the socket convert it to Share class form.
	void sendShareTo(unsigned short id, Share& toSend)const;		//convert a share to raw data and send it to specified by id party .
	~Party();														//D'tor- release dynamicly allocated memory in heap.
	void fVerify();													//fVerify functuality as described in the paper.
	Circuit* getArithmeticCircuit()const;							//getter for the arithmetic circuit pointer.
	void fCoin(std::vector<ZZ_p>& thetas, int numOfElements);		//fCoin functuality as described in the paper.Generate numOfElements random elements fromZp and send to all parties.
																	//interpulates numOfPolynomials polinomials each of degree polynomialsDegree .Note:the coeffient index of pointsToInterpolate is X value.
																	//the first coeffient is a random number from Zp. The calculated polynomials are stored in inputPolynomials.
	void interpolateInputPolynomials(unsigned int polynomialsDegree, unsigned int numOfPolynomials, vec_ZZ_p& omegas, vector<ZZ_pX>&  inputPolynomials)const;
	void verifyRound2(unsigned int M, vector<ZZ_pX>& inputPolynomials, ZZ_pX& p);
	void verifyRound3();
	void rawDataToVec(vec_ZZ_p& vec, unsigned int vectorLen, byte* rawData);
};