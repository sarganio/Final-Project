#pragma once
#include "PartyShare.h"

class Gate
{
protected:
	//Inputs & Output of the gate
	Share* _left; 
	PartyShare* _output;
	
public:
	Gate():_left(nullptr),_output(nullptr) {};
	Gate(Share* left,Party* p=nullptr) :_left(left), _output(new PartyShare(nullptr,p)) {};

	//Getters & setters
	Share* getLeft();
	PartyShare* getOutput();
	
	void setLeft(Share* input);
	void setOutput(PartyShare* output);

	virtual void calculateOutput() = 0;//calculate the output of this Gate(Addition/Multipication)
	virtual ~Gate();

};

