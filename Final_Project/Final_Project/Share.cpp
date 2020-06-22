#include "Share.h"

//-------------------------------------------------Part----------------------------------------------------------

//constructor
Part::Part(char name, unsigned short index, long value) :_value(value),_index(index), _name(name) {}
Part::Part(const Part& other) {
	this->_index = other._index;
	this->_name = other._name;
	this->_value = other._value;

}
//getters & setters
unsigned short Part::getIndex()const { return this->_index; }
char Part::getName()const { return _name; }
long Part::getValue()const { return this->_value; }
void Part::setValue(long value) { this->_value = value; }

//operator = overload
Part& Part::operator=(long newVal) {
	this->setValue(newVal);
	return *this;
}
//convert part to printable string
string Part::toString()const {
	//return string(" Index:"+std::to_string(_index)  + "Value:" + std::to_string(_value) + " Part Name:" + std::to_string(_name)+"\n");
	return string("(" + std::to_string(_value) + "," + std::to_string(_index) + "," + _name + ")");
}

//-------------------------------------------------Share----------------------------------------------------------

//constructor
Share::Share(Part* v1, Part* v2) {
	if (v1->getName() != v2->getName())
		throw std::exception(__FUNCTION__ "name of shares is not the same!");

	this->_value = std::make_pair(v1, v2);
}

//constructor
Share::Share(unsigned short index, char name) {
	this->_value = std::make_pair(new Part(name, index), new Part(name, (index + 1) % NUM_OF_PARTIES));
}

//copy constructor
Share::Share(const Share& other) {
	//this->_value = other._value;
	*this = other;
}//?????

Share& Share::operator=(Share const& other) {
	this->_value = std::make_pair(new Part(other._value.first->getName(), other._value.first->getIndex()), new Part(other._value.second->getName(), other._value.second->getIndex()));
	this->_value.first->setValue(other._value.first->getValue());
	this->_value.second->setValue(other._value.second->getValue());

	return *this;
}
//operator [] overload
Part& Share::operator[](unsigned short index) const {
	if (this->_value.first->getIndex() == index)
		return *this->_value.first;
	if (this->_value.second->getIndex() == index)
		return *this->_value.second;
	throw std::exception(__FUNCTION__ "Index is invalid!");
}
//operator + with share overload
Share Share::operator+(const Share& other) const {
	Part* a = nullptr, * b = nullptr;
	if(!isAddable(other))
		throw std::exception(__FUNCTION__ "Shares are not addible!");
	if (this->_value.first->getName() != other._value.first->getName())
		throw std::exception(__FUNCTION__ "Shares name is not same!");

	a = new Part(this->_value.first->getName(), this->_value.first->getIndex(), this->_value.first->getValue() + other._value.first->getValue());
	b = new Part(other._value.first->getName(), this->_value.second->getIndex(), this->_value.second->getValue() + other._value.second->getValue());

	return Share(a, b);
}

//operator + with constant overload
Share Share::operator+(long scalar) const{
	Part* a = nullptr, * b = this->_value.second;
	//Share ans = Share(a, b);
	if (this->_value.first->getIndex() != 0)
		return *this;
	if (this->_value.first) {
		a = new Part(_value.first->getName(), this->_value.first->getIndex(), this->_value.first->getValue() + scalar);
	}
	return Share(a, b);
}

//operator * with constant overload
Share Share::operator*(int constant)const {
	Part* a = nullptr,* b = nullptr;
	if (this->_value.first) 
		a = new Part(_value.first->getName(), this->_value.first->getIndex(), this->_value.first->getValue() * constant);

	if (this->_value.second) 
		b = new Part(_value.second->getName(), this->_value.second->getIndex(), this->_value.second->getValue() * constant);

	return Share(a, b);
}

//check if two part is addiable
bool Share::isAddable(const Share& other) const {
	return this->_value.first->getIndex() == other._value.first->getIndex() && this->_value.second->getIndex() == other._value.second->getIndex();
}

//convert Share to printable string
string Share::toString()const {
	return "("+string(this->_value.first->toString() +","+ this->_value.second->toString())+")";
}

//distructor
Share::~Share() {
	delete this->_value.first;
	this->_value.first = nullptr;
	delete this->_value.second;
	this->_value.second = nullptr;
}

