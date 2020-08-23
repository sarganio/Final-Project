#pragma once
#include "Party.h"
#include "Share.h"

class Party;

class PartyShare:public Share
{
		Party* _owner;
public:
	PartyShare(Share* prevOutput, Party* p) :Share(*prevOutput) {
		_owner = p;
	}
	int correlatedRandomness(Party& p) const;	//calculate correlated randomness as described in pepare.
	Share calculateOutputMull(MultiplicationGate<Share*>& g);
	Party* getParty()const { return _owner; }
	friend Share operator*(Share& left, PartyShare& right) {//TODO----------------------------
		
		return Share();
	}
};

