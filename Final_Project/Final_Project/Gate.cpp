#include "Gate.h"
//Getters & setters
Share* Gate::getLeft() { return _left; }
PartyShare* Gate::getOutput() { return _output; }

void Gate::setLeft(Share* input) { _left = input; }
void Gate::setOutput(PartyShare* output) { _output = output; }
Gate::~Gate() {
	if (_output)
		delete _output;
	if (_left)
		delete _left;
	_output = nullptr;
	_left= nullptr;
}