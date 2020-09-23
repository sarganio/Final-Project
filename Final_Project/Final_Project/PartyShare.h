#pragma once
#include "Party.h"
#include "Share.h"
#include <assert.h>

class Party;


class PartyShare:public Share
{
		Party* _owner;
public:
	PartyShare(Share* prevOutput = nullptr, Party* p = nullptr);
	void setParty(Party* p);
	int correlatedRandomness() const;										//calculate correlated randomness as described in pepare.
	Party* getParty()const;
	~PartyShare();
	friend Share operator*(Share& left, PartyShare& right) {
		//must hold in order for the function to work properly.
		assert(right.getFirst().getIndex() < right.getSecond().getIndex());
		assert(left.getFirst().getIndex() < left.getSecond().getIndex());

		static unsigned int currentNumOfMulGates = 0;

		//calculate z_i
		unsigned int alpha = right.correlatedRandomness();
		long rightFirstVal = right.getFirst().getValue(), rightSecondVal = right.getSecond().getValue();
		long leftFisrtVal = left.getFirst().getValue(), leftSecondVal = left.getSecond().getValue();
		//z_i = u_i*v_i+ u_i*v_{i-1}+u_{i-1}*v_i+alpha_i
		long firstPartOputput = leftSecondVal * rightSecondVal + leftSecondVal * rightFirstVal + leftFisrtVal * rightSecondVal + alpha;//needs to compute Zi and send it to Party _id+1
		firstPartOputput %= ZP;
		if (firstPartOputput < 0)
			firstPartOputput += ZP;


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
		right.getParty()->setG_GateInput(currentNumOfMulGates * 6, left.getFirst());
		right.getParty()->setG_GateInput(1 + currentNumOfMulGates * 6, left.getSecond());
		right.getParty()->setG_GateInput(2 + currentNumOfMulGates * 6, right.getFirst());
		right.getParty()->setG_GateInput(3 + currentNumOfMulGates * 6, right.getSecond());
		right.getParty()->setG_GateInput(4 + currentNumOfMulGates * 6, Part('a', id, alpha));
		right.getParty()->setG_GateInput(5 + currentNumOfMulGates * 6, output[id]);

		currentNumOfMulGates++;

		return output;
	}
	Share& operator=(Share const& other);
};

