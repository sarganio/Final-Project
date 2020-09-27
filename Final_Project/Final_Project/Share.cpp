#include "Share.h"
#include <cassert>

//-------------------------------------------------Part----------------------------------------------------------
/*
unique identifier for each party based on the indexes of the parts in a given share.
e.g. the first party holds the share: (?_0,?_1) -> 1
	the second party holds the share: (?_1,?_2) -> 3
	the third party holds the share: (?_0,?_2) -> 2
*/
enum{FIRST_PARTY_ID = 0, SECOND_PARTY_ID = 3, THIRD_PARTY_ID = 2};

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
void Part::setIndex(unsigned short index) { _index = index; }

//-------------------------------------------------Share----------------------------------------------------------

//constructor
Share::Share(Part* v1, Part* v2) {
	//if (v1->getName() != v2->getName())
		//throw std::exception(__FUNCTION__ "name of shares is not the same!");


	this->_value = std::make_pair(v1, v2);
}

//constructor
Share::Share(unsigned short index, char name) {
	Part* firstPart = new Part(name, index);
	Part* secondPart = new Part(name, (index + 1) % NUM_OF_PARTIES);
	//the lower index should be first part.
	this->_value = index != NUM_OF_PARTIES - 1 ? std::make_pair(firstPart, secondPart) : std::make_pair(secondPart, firstPart);
}

//copy constructor
Share::Share(const Share& other) {
	//this->_value = other._value;
	if(this != &other)
		*this = other;
}
Share& Share::operator=(Share const& other) {
	assert(other.getFirst().getIndex() < other.getSecond().getIndex());
	//create the parts of the share
	this->_value = std::make_pair(new Part(other._value.first->getName(), other._value.first->getIndex()), new Part(other._value.second->getName(), other._value.second->getIndex()));
	//copy values
	this->_value.first->setValue(other._value.first->getValue());
	this->_value.second->setValue(other._value.second->getValue());

	return *this;
}
Share Share::operator+=(const Share& other) const {
	return other + *this;
}
//see above the documentation of the sharesIndexSum value.
unsigned short Share::getOwnerId()const {
	unsigned short sharesIndexSum = _value.first->getIndex() + _value.second->getIndex();
	switch (sharesIndexSum){
	case FIRST_PARTY_ID:
		return 0;
	case SECOND_PARTY_ID:
		return 1;
	case THIRD_PARTY_ID:
		return 2;
	default:
		throw std::exception(__FUNCTION__ "Invalid share indexes!");
	};
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
	string name;
	if(!isAddable(other))
		throw std::exception(__FUNCTION__ "Shares are not addible!");
	if (this->_value.first->getName() != other._value.first->getName())
		name = this->_value.first->getName() + other._value.first->getName();
		//throw std::exception(__FUNCTION__ "Shares name is not same!");

	a = new Part(this->_value.first->getName(), this->_value.first->getIndex(), (this->_value.first->getValue() + other._value.first->getValue())%ZP);
	b = new Part(other._value.first->getName(), this->_value.second->getIndex(),(this->_value.second->getValue() + other._value.second->getValue())%ZP);
	return Share(a, b);
}

//operator + with constant overload
Share Share::operator+(long scalar)const {
	Part* a = nullptr;
	Part* b = nullptr;
	scalar %= ZP;
	if (scalar < 0)
		scalar += ZP;
	assert(this->_value.first->getIndex() < this->_value.second->getIndex());

	if (this->_value.first->getIndex() == 0) {
		a = new Part(_value.first->getName(), 0, (this->_value.first->getValue() + scalar)%ZP);
		b = new Part(_value.second->getName(), this->_value.second->getIndex(), this->_value.second->getValue());

	}
	else
		return *this;
	return Share(a, b);
}

//operator * with constant overload
Share Share::operator*(long constant)const {
	Part* a = nullptr,* b = nullptr;
	constant %= ZP;
	if (constant < 0)
		constant += ZP;
	assert(this->_value.first->getIndex() < this->_value.second->getIndex());
	if (this->_value.first) 
		a = new Part(_value.first->getName(), this->_value.first->getIndex(), (this->_value.first->getValue() * constant)%ZP);

	if (this->_value.second) 
		b = new Part(_value.second->getName(), this->_value.second->getIndex(), (this->_value.second->getValue() * constant)%ZP);

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
	if (_value.first) {
		delete this->_value.first;
		this->_value.first = nullptr;
	}
	if (_value.second) {
		delete this->_value.second;
		this->_value.second = nullptr;
	}
}