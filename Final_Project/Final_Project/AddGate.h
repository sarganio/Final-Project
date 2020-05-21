#pragma once
#include "Gate.h"
template <class operand>
class AddGate : public Gate<operand>
{
private:



public:
	AddGate(Share left, operand right) :Gate(left, right) {};


};

