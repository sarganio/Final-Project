#pragma once
#include "Share.h"

template <class operand> 
class Gate
{
protected:
	//Inputs & Output of the gate
	Share _left; 
	operand _right;
	Share _output;
	
public:
	Gate() {};
	Gate(Share& left, operand& right) :_left(left), _right(right), _output(0){};

	//Getters & setters
	Share& getLeft() { return _left; };
	operand& getRight() { return _right; };
	Share& getOutput() { return _output; };

	void setLeft(Share input) { _left = input; }
	void setRight(operand input) { _right = input; }
	void setOutput(Share output) { _output = output; }

	virtual void calculateOutput() = 0;

};

