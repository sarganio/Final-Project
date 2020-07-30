#pragma once
#include "Party.h"
#include "Gate.h"

class Party;
class Circuit
{
private:
	Party* _party;
	vector<vector<Gate*>> _circuit;
	int _numOfLayers;
	vector<int> _gatesPerLayer;

public:
	Circuit():_party(nullptr),_numOfLayers(0) {};
	Circuit(byte seed[SEQ_LEN], Party* party);
	void calculateOutput();
};
