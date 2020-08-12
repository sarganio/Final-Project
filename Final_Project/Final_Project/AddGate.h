#pragma once
#include "Gate.h"
#include <stdlib.h>     //use rand() to generate random circuit for debuging.

//This class represents an addition gate which has 2 inputs: A share and a constant or 2 Shares
template <class operand>
class AddGate : public Gate
{
private:
	operand* _right;																//either a Share or a constant
public:
	AddGate() : Gate::Gate(), _right(nullptr) {};
	AddGate(Share* left, operand* right) : Gate::Gate(left), _right(right) {};
	void calculateOutput() override{*this->_output = *this->_left + *this->_right;}
};

