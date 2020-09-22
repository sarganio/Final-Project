#pragma once
#include "TcpSocket.h"
#include "Helper.h"
#include "Messages.h"

#include "Share.h"

#include <vector>
#include <string>
#include <mutex>

#define L  1//num of multipication gates per G gate
#define INPUTS_PER_MUL_GATE  6

using std::vector;
using std::pair;
using std::string;
using std::mutex;

//class Share;
class Circuit;

class Party
{
private:
	unsigned short _id;												//unique identifier, lowest ip receive id = 1 and so on.
	long _input;													//secret input.
	byte _finalSeq[SEQ_LEN]{};										//mutual counter for all participents.
	vector<TcpSocket*> _sockets;									//socket for all TCP connection with other parties.
	vector<Message*>_msgs;											//vector which contains all recieved messages
	vector<SecByteBlock*>_keys;										//vector which contains all the keys required for generating random numbers
	vector<Share*> _shares;											//vector which contains each party's share.index of vector is the id of input's party
	Circuit* _arithmeticCircuit;									//the function which needs to be computed.
	void printKey(unsigned short index)const;						//recieves the index of the key's owner and print it.
	vector<Part> _gGatesInputs;
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
	Share* calcCircuit();												//calculate the output of all the gates in the circuit
	Share* fRand();													//fRand functuality as described in the paper.
	void fInput();													//fInput functuality as described in the paper.
	long reconstruct(vector<Share*>& myShare);						//receives all the Parts of a given share.
	//vector<Share*>& getAllShares();								
	long finalReconstruct(Share&);									//function used at the end of the protocol, recieve shares from other parties and return the result.
	Share& RecieveShareFrom(unsigned short id);						//read raw data of share from the socket convert it to Share class form
	void sendShareTo(unsigned short id, Share& toSend)const;		//convert a share to raw data and send it to specified by id party 
	~Party();														//D'tor- release dynamicly allocated memory in heap.
	void fVerify();
	Circuit* getArithmeticCircuit()const;
	void verifyRound1();
};