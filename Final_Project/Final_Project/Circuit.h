#pragma once
#include "Party.h"
#include "Gate.h"

class Party;

class Circuit
{
private:
	Party* _party;								//in order to perform communication rounds in multipication gates an access to Party is required.
	vector<vector<Gate*>> _circuit;				//first index is the layer index and the second index is the gate's index within each layer.
	int _numOfLayers;
	vector<int> _gatesPerLayer;					//num of gates in each layer of the circuits
	unsigned int _numOfMulGates;
public:
	Circuit():_party(nullptr),_numOfLayers(0),_numOfMulGates(0) {};
	Circuit(byte seed[SEQ_LEN], Party* party);
	void calculateOutput();						//computes the outout of each gate
	Share* getOutput();							//get the result of the circuit
	~Circuit();									//D'tor - deallocate all gates memmory.
	unsigned int getNumOfMulGates()const;
	//int correlatedRandomness(Party& p) const;	//calculate correlated randomness as described in pepare.
	//Share calculateOutputMull(MultiplicationGate<Share*>& g);
};