#pragma once
#include "Party.h"
#include "Share.h"
#include <assert.h>

class Party;


class PartyShare:public Share
{
		Party* _owner;
public:
	PartyShare(Share* prevOutput=nullptr, Party* p=nullptr)/* :Share(*prevOutput)*/ {
		if (prevOutput)
			*this = *prevOutput;
		_owner = p;
	}
	void setParty(Party* p) { _owner = p; }
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

		static unsigned int numOfMulGates = 0;

		//calculate z_i
		unsigned int alpha = right.correlatedRandomness();
		long rightFirstVal = right.getFirst().getValue(), rightSecondVal = right.getSecond().getValue();
		long leftFisrtVal = left.getFirst().getValue(), leftSecondVal = left.getSecond().getValue();
		//z_i = u_i*v_i+ u_i*v_{i-1}+u_{i-1}*v_i+alpha_i
		long firstPartOputput = leftSecondVal * rightSecondVal + leftSecondVal * rightFirstVal + leftFisrtVal * rightSecondVal + alpha;//needs to compute Zi and send it to Party _id+1
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
		Share output((id + 2) % NUM_OF_PARTIES, 'z');
		output[right.getParty()->getID()].setValue(firstPartOputput);
		output[(right.getParty()->getID() + 2) % NUM_OF_PARTIES].setValue(secondPartOutput);

		//save inputs to the G gate
		//6L(l-1) + j          -------------   need to fix!
		right.getParty()->setG_GateInput(numOfMulGates * L*6, left.getFirst());
		right.getParty()->setG_GateInput(1 + numOfMulGates * L*6, left.getSecond());
		right.getParty()->setG_GateInput(2 + numOfMulGates * L*6, right.getFirst());
		right.getParty()->setG_GateInput(3 + numOfMulGates * L*6, right.getSecond());
		right.getParty()->setG_GateInput(4 + numOfMulGates * L*6, Part('a', id, alpha));
		right.getParty()->setG_GateInput(5 + numOfMulGates * L*6, output[id]);

		numOfMulGates++;

		return output;
	}
	Share& operator=(Share const& other) {
		*(Share*)this = other;
		return *this;
	}
};

