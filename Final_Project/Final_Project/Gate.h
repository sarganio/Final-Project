#pragma once
#include "Share.h"

class Gate
{
protected:
	//Inputs & Output of the gate
	Share* _left; 
	Share* _output;
	
public:
	~Gate() { if (_output)delete _output; }
	Gate():_left(nullptr),_output(nullptr) {};
	Gate(Share* left) :_left(left), _output(new Share()) {};

	//Getters & setters
	Share* getLeft();
	Share* getOutput();
	
	void setLeft(Share* input);
	void setOutput(Share* output);

	virtual void calculateOutput() = 0;//calculate the output of this Gate(Addition/Multipication)

};

