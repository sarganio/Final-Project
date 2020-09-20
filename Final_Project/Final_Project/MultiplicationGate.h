#pragma once
#include "Gate.h"
#include "Share.h"

//This class represents a multipication gate which has 2 inputs: A share and a constant or 2 Shares
template <class operand>
class MultiplicationGate : public Gate
{

private:
	operand* _right;																	//either a Share or a constant

public:
	MultiplicationGate() {};
	MultiplicationGate(Share* left, operand* right,Party* p=nullptr): Gate::Gate(left,p), _right(right) {};

	void calculateOutput() override { *this->_output = *this->_left * *this->_right;};
	operand* getRight() { return _right; }
};

