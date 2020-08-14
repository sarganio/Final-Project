#include "Gate.h"
//Getters & setters
Share* Gate::getLeft() { return _left; }
Share* Gate::getOutput() { return _output; }

void Gate::setLeft(Share* input) { _left = input; }
void Gate::setOutput(Share* output) { _output = output; }