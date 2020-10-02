#pragma once
#include "Party.h"
#include "Share.h"
#include <assert.h>

class Party;


class PartyShare:public Share
{
		Party* _owner;														//pointer to the party object in order to perform communication while a multipiction between two share accures.					
public:
	PartyShare(Share* prevOutput = nullptr, Party* p = nullptr);
	void setParty(Party* p);												//setter for pointer to party.
	int correlatedRandomness() const;										//calculate correlated randomness as described in pepare.
	Party* getParty()const;													//getter for the pointer to party.
	~PartyShare()=default;
	Share& operator=(Share const& other);									//assighnment operator
	//calculate the multipication of two shares
	friend Share operator*(Share& left, PartyShare& right) {
		//must hold, in order for the function to work properly.
		assert(right.getFirst().getIndex() < right.getSecond().getIndex());
		assert(left.getFirst().getIndex() < left.getSecond().getIndex());

		static unsigned int currentNumOfMulGates = 0;

		//calculate z_i
		//int alpha = right.correlatedRandomness();
		int alpha = 0;
		alpha %= ZP;
		if (alpha < 0)
			alpha += ZP;
		unsigned short id = right.getParty()->getID();
		long rightFirstVal = right[id].getValue(), rightSecondVal = right[(id + 2) % NUM_OF_PARTIES].getValue();
		long leftFirstVal = left[id].getValue(), leftSecondVal = left[(id + 2) % NUM_OF_PARTIES].getValue();
		//z_i = u_i*v_i+ u_i*v_{i-1}+u_{i-1}*v_i+alpha_i|left-u,right-v
		long firstPartOputput = leftFirstVal * rightFirstVal + leftFirstVal * rightSecondVal + leftSecondVal * rightFirstVal + alpha;//needs to compute Zi and send it to Party _id+1
		firstPartOputput %= ZP;
		if (firstPartOputput < 0)
			firstPartOputput += ZP;


		//send zi to the next party
		byte* buffer = (byte*)&firstPartOputput;
		right.getParty()->sendTo((id + 1) % NUM_OF_PARTIES, MUL_GATE, buffer);

		//recieve z_{i-1}
		long secondPartOutput = 0;
		right.getParty()->readFrom((id + 2) % NUM_OF_PARTIES, (byte*)&secondPartOutput);

		//set output values to be: (z_{i-1},z_i)
		Share output((id + 2) % NUM_OF_PARTIES, 'z');
		output[right.getParty()->getID()].setValue(firstPartOputput);
		output[(right.getParty()->getID() + 2) % NUM_OF_PARTIES].setValue(secondPartOutput);

		//save inputs to each g gate in the circuit
		Party* partyPtr = right.getParty();
		partyPtr->setG_GateInput(currentNumOfMulGates * INPUTS_PER_G_GATE, left[id]);
		partyPtr->setG_GateInput(1 + currentNumOfMulGates * INPUTS_PER_G_GATE, left[(id+2)%NUM_OF_PARTIES]);
		partyPtr->setG_GateInput(2 + currentNumOfMulGates * INPUTS_PER_G_GATE, right[id]);
		partyPtr->setG_GateInput(3 + currentNumOfMulGates * INPUTS_PER_G_GATE, right[(id + 2) % NUM_OF_PARTIES]);
		partyPtr->setG_GateInput(4 + currentNumOfMulGates * INPUTS_PER_G_GATE, Part('a', id, alpha));
		partyPtr->setG_GateInput(5 + currentNumOfMulGates * INPUTS_PER_G_GATE, output[id]);
		for (int i = 0; i < INPUTS_PER_G_GATE; i++)
			std::cout << " Inputs to g Gate: " << partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 0].getValue() << " ";
		std::cout << " C is: " << (partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 0].getValue() * partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 2].getValue() + partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 0].getValue() * partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 3].getValue() + partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 1].getValue() * partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 2].getValue() + partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 4].getValue() - partyPtr->_gGatesInputs[currentNumOfMulGates * INPUTS_PER_G_GATE + 5].getValue())%ZP;
		currentNumOfMulGates++;

		return output;
	}
};

