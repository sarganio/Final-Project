#pragma once
#include "Party.h"
#include "Gate.h"
#include "AddGate.h"
#include "MultiplicationGate.h"

class Circuit
{
private:
	Party* _party;
	vector<vector<Gate*>> _circuit;
	int _numOfLayers;
	vector<int> _gatesPerLayer;



public:
	Circuit() {};
	Circuit(byte seed[SEQ_LEN], Party* party);
		



};

