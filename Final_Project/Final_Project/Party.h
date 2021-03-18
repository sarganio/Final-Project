#pragma once
#include "TcpSocket.h"
#include "Helper.h"
#include "Messages.h"

#include "Share.h"
#define NTL_NO_MIN_MAX
#include "NTL/vec_ZZ_p.h"
#include "NTL/ZZ_pX.h"
#include <vector>
#include <string>
#include <mutex>

//#define ELEMENT_SIZE sizeof(ZZ_p)
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
using NTL::vec_vec_ZZ_p;
using NTL::vec_ZZ_pX;

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
	vector<Share*> _shares;											//vector which contains each party's share.index of vector is the id of input's party
	Circuit* _arithmeticCircuit;									//the function which needs to be computed.
	//void printKey(unsigned short index)const;						//recieves the index of the key's owner and print it.
	vector<Part> _multipicationGateOutputs;
public:
	vector<Part> _gGatesInputs;										//vector which contains all g gates inputs for verification stage.
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
	void vecToRawData(unsigned int numOfElements, NTL::vec_ZZ_p& vec, byte* rawData)const;
	//the first round of fVerify as described in the paper.
	long finalReconstruct(Share&);									//function used at the end of the protocol, recieve shares from other parties and return the result.
	Share& RecieveShareFrom(unsigned short id);						//read raw data of share from the socket convert it to Share class form.
	void sendShareTo(unsigned short id, Share& toSend)const;		//convert a share to raw data and send it to specified by id party .
	~Party();														//D'tor- release dynamicly allocated memory in heap.
	void fVerify();													//fVerify functuality as described in the paper.
	Circuit* getArithmeticCircuit()const;							//getter for the arithmetic circuit pointer.
	void fCoin(vec_ZZ_p& thetas,unsigned int numOfElements);		//fCoin functuality as described in the paper.Generate numOfElements random elements fromZp and send to all parties.
																	//interpulates numOfPolynomials polinomials each of degree polynomialsDegree .Note:the coeffient index of pointsToInterpolate is X value.
																	//the first coeffient is a random number from Zp. The calculated polynomials are stored in inputPolynomials.
	void interpolateInputPolynomials(unsigned int polynomialsDegree, unsigned int numOfPolynomials, vec_vec_ZZ_p& pointsToInterpolate, vec_ZZ_pX& inputPolynomials)const;//perform interpolation on the given points
	void verifyRound1(vec_ZZ_p& thetas, unsigned int M, vec_vec_ZZ_p& pointsToInterpulate, ZZ_pX& p);
	void verifyRound2(unsigned int M, vec_vec_ZZ_p& pointsToInterpolate, ZZ_pX& p,vec_ZZ_p& calculationForRound3);
	void verifyRound3(vec_ZZ_p& thetas,vec_ZZ_p& polynomialsAtPointR);				//round 3 of the verification stage as described in the paper
	void printVecVec(NTL::vec_vec_ZZ_p& pointsToInterpolateRound2)const;
	//round 2 of the verification stage as described in the paper 
	void rawDataToVec(vec_ZZ_p& vec, unsigned int vectorLen, byte* rawData);//convert an array of bytes to ZZ_p elements
	template<typename T,typename G>
	G cFunction(T& inputsToGGate)const {//caculate cFunction as described in papere.
		return inputsToGGate[0] * inputsToGGate[2] + inputsToGGate[0] * inputsToGGate[3] + inputsToGGate[1] * inputsToGGate[2] + inputsToGGate[4] - inputsToGGate[5];
	}
	Part& getMultipicationOutput(unsigned short index);				//get a Part of multipication gate output by index.
	void setMultipicationOutput(Part& toSave);						//set a Part of the current multipication gate.
	void orderInputVector(vec_vec_ZZ_p& inputVector, unsigned short proverIndex,unsigned int l);//order the inputs to the g gates so the order will be coherent with all parties.
	template<typename T, typename G>
	void gGateOutput(vec_ZZ_p& thetas, T& polynomials, G& output)const {
		for (int i = 0; i < L; i++)
			output = thetas[i] * cFunction<T, G>(polynomials);
	}
};