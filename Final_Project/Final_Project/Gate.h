#pragma once
#include "Share.h"

class Share;
class Gate
{
protected:
	//Inputs & Output of the gate
	Share* _left; 
	Share* _output;
	
public:
	Gate():_left(nullptr),_output(nullptr) {};
	Gate(Share* left) :_left(left), _output(new Share()) {};

	//Getters & setters
	Share* getLeft() { return _left; }
	Share* getOutput() { return _output; }
	
	void setLeft(Share* input) { _left = input; }
	void setOutput(Share* output) { _output = output; }

	virtual void calculateOutput() = 0;

};

