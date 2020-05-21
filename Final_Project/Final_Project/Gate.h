#pragma once
#include "Share.h"

class Gate
{
private:
	//Inputs & Output of the gate
	Share _left; 
	Share _right;
	Share _output;
	
public:
	Gate(Share left, Share right) :_left(left), _right(right) {};

	//Getters & setters
	Share getLeft() { return _left; };
	Share getRight() { return _right; };
	Share getOutput() { return _output; };

	void setLeft(Share input) { _left = input; }
	void setRight(Share input) { _right = input; }
	void setOutput(Share output) { _output = output; }

	virtual void calculateOutput() = 0;

};

