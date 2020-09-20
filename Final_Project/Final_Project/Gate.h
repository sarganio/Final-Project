#pragma once
#include "PartyShare.h"

class Gate
{
protected:
	//Inputs & Output of the gate
	Share* _left; 
	PartyShare* _output;
	
public:
	//~Gate() { if (_output)delete _output; }
	Gate():_left(nullptr),_output(nullptr) {};
	Gate(Share* left) :_left(left), _output(new PartyShare()) {};

	//Getters & setters
	Share* getLeft();
	Share* getOutput();
	
	void setLeft(Share* input);
	void setOutput(PartyShare* output);

	virtual void calculateOutput() = 0;//calculate the output of this Gate(Addition/Multipication)
	virtual ~Gate();

};

