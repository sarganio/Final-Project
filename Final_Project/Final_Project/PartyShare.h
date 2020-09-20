#pragma once
#include "Party.h"
#include "Share.h"
#include <assert.h>

class Party;


class PartyShare:public Share
{
		Party* _owner;
public:
	PartyShare(Share* prevOutput=nullptr, Party* p=nullptr) :Share(*prevOutput) {
		_owner = p;
	}
	int correlatedRandomness() const{//calculate correlated randomness as described in pepare.
	Share* randomNumbers =  _owner->fRand();//memory needs to be released!!
	return (*randomNumbers)[_owner->getID()].getValue() - (*randomNumbers)[(_owner->getID() + 2) % NUM_OF_PARTIES].getValue();
}	

	Party* getParty()const { return _owner; }
	~PartyShare() {
		if (this->_owner)
			delete _owner;
	}
	friend Share operator*(Share& left, PartyShare& right) {//TODO----------------------------
		//must hold in order for the function to work properly.
		assert(right.getFirst().getIndex() < right.getSecond().getIndex());
		assert(left.getFirst().getIndex() < left.getSecond().getIndex());

		//calculate z_i
		unsigned int alpha = right.correlatedRandomness();
		long rightFirstVal = right.getFirst().getValue(), rightSecondVal = right.getSecond().getValue();
		long leftFistVal = left.getFirst().getValue(), leftSecondVal = left.getSecond().getValue();
		//z_i = u_i*v_i+ u_i*v_{i-1}+u_{i-1}*v_i+alpha_i
		long firstPartOputput = leftSecondVal* rightSecondVal + leftSecondVal*rightFirstVal + leftFistVal*rightSecondVal + alpha;//needs to compute Zi and send it to Party _id+1
		firstPartOputput %= P;
		if (firstPartOputput < 0)
			firstPartOputput += P;


		//send zi to the nest party
		byte* buffer = (byte*)&firstPartOputput;
		unsigned short id = right.getParty()->getID();
		right.getParty()->sendTo((id + 1) % NUM_OF_PARTIES, MUL_GATE, buffer);

		//recieve z_{i-1}
		long secondPartOutput = 0;
		right.getParty()->readFrom((id + 2) % NUM_OF_PARTIES, (byte*)&secondPartOutput);

		//set output values to be: (z_{i-1},z_i)
		Share output((id + 2)%NUM_OF_PARTIES, 'z');
		output[right.getParty()->getID()].setValue(firstPartOputput);
		output[(right.getParty()->getID() + 2)%NUM_OF_PARTIES].setValue(secondPartOutput);

		return output;
	}
	Share& operator=(Share const& other) {
		*(Share*)this = other;
		return *this;
	}
};

