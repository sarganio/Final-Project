#pragma once
#include "Gate.h"

template <class operand>
class AddGate : public Gate<operand>
{
public:
	AddGate() : Gate< operand>::Gate() {};
	AddGate(Share& left, operand& right) : Gate< operand>::Gate(left, right) {};
	void calculateOutput() override{
		this->_output = this->_left + this->_right;
	}

};

