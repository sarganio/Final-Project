#pragma once
#include "Gate.h"
#include <stdlib.h>     /* rand */


template <class operand>
class AddGate : public Gate
{
private:
	operand* _right;
public:
	AddGate() : Gate::Gate() {};
	AddGate(Share* left, operand* right) : Gate::Gate(left), _right(right) {};

	void calculateOutput() override{
		*this->_output = *this->_left + *this->_right;
	}

};

