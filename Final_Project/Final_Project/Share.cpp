#include "Share.h"

Part::Part(unsigned short index, char name,long value) :_value(value),_index(index), _name(name) {}
unsigned short Part::getIndex()const { return this->_index; }
char Part::getName()const { return _name; }
long Part::getValue()const { return this->_value; }
bool Share::isAddable(const Share& other) const{
	return this->_value.first->getIndex() == other._value.first->getIndex() && this->_value.second->getIndex() == other._value.second->getIndex();
}
void Part::setValue(long value) {this->_value = value;}
Share::Share(Part* v1, Part* v2) {
	if(v1->getName() != v2->getName())
		throw std::exception(__FUNCTION__ "name of shares is not the same!");

	this->_value = std::make_pair(v1, v2);
}
string Part::toString()const {
	//return string(" Index:"+std::to_string(_index)  + "Value:" + std::to_string(_value) + " Part Name:" + std::to_string(_name)+"\n");
	return string("("+ std::to_string(_value)+","+std::to_string(_index)+","+ _name +")");
}
Part& Share::operator[](unsigned short index) const {
	if (this->_value.first->getIndex() == index)
		return *this->_value.first;
	if (this->_value.second->getIndex() == index)
		return *this->_value.second;
	throw std::exception(__FUNCTION__ "Index is invalid!");
}
Share Share::operator+(const Share& other) const {
	Part* a = nullptr, * b = nullptr;
	if(!isAddable(other))
		throw std::exception(__FUNCTION__ "Shares are not addible!");
	if (this->_value.first->getName() != other._value.first->getName())
		throw std::exception(__FUNCTION__ "Shares name is not same!");

	a = new Part(this->_value.first->getValue() + other._value.first->getValue(), this->_value.first->getIndex(), this->_value.first->getName());
	b = new Part(this->_value.second->getValue() + other._value.second->getValue(), this->_value.second->getIndex(), other._value.first->getName());

	return Share(a, b);
}
Share Share::operator+(long scalar) const{
	Part* a = nullptr, * b = nullptr;
	if (this->_value.first->getIndex() != 0)
		return *this;
	if (this->_value.first) {
		a = new Part(this->_value.first->getValue() + scalar, this->_value.first->getIndex(), _value.first->getName());
	}
	return Share(a,b);

}
Share Share::operator*(int constant)const {
	Part* a = nullptr,* b = nullptr;
	if (this->_value.first) 
		a = new Part(this->_value.first->getValue() * constant, this->_value.first->getIndex(), _value.first->getName());

	if (this->_value.second) 
		b = new Part(this->_value.second->getValue() * constant, this->_value.second->getIndex(), _value.second->getName());

	return Share(a, b);
}
string Share::toString()const {
	return "("+string(this->_value.first->toString() +","+ this->_value.second->toString())+")";
}
Share::~Share() {
	delete this->_value.first;
	this->_value.first = nullptr;
	delete this->_value.second;
	this->_value.second = nullptr;
}
Share::Share(unsigned short index,char name) {
	this->_value = std::make_pair(new Part(index, name), new Part((index+1)%NUM_OF_PARTIES, name));
}
Part& Part::operator=(long newVal) {
	this->setValue(newVal);
	return *this;
}