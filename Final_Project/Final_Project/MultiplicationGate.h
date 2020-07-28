#pragma once
#include "Gate.h"


template <class operand>

class MultiplicationGate :public Gate
{

private:
	operand _right;

public:
	MultiplicationGate() {};
	MultiplicationGate(Share& left, operand& right): Gate::Gate(left, true), _right(right) {};

	void calculateOutput() override {}
};

