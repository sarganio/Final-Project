#include "Share.h"

Part::Part(unsigned short index, char name) :_index(index), _name(name) {}
unsigned short Part::getIndex()const { return this->_index; }
char Part::getName()const { return _name; }

InputPart::InputPart(long value, unsigned short index, char name) :Part(index,name), _value(value){}
long InputPart::getValue()const { return this->_value; }
bool Share::isAddable(const Share& other) const{
	return this->_value.first.getIndex() == other._value.first.getIndex() && this->_value.second.getIndex() == other._value.second.getIndex();
}
Part& Share::operator[](unsigned short index) {
	if (this->_value.first.getIndex() == index)
		return this->_value.first;
	if (this->_value.second.getIndex() == index)
		return this->_value.second;
	throw std::exception(__FUNCTION__ "Index is invalid!");
}
void InputPart::setValue(long value) {this->_value = value;}
Share::Share(const Part& v1, const Part& v2) {
	if(v1.getName() != v2.getName())
		throw std::exception(__FUNCTION__ "name of shares is not the same!");
	if(v1.getIndex() <= v2.getIndex())
		this->_value = std::make_pair(v1, v2);
	else
		this->_value = std::make_pair(v2, v1);

}
string InputPart::toString()const {
	//return string(" Index:"+std::to_string(_index)  + "Value:" + std::to_string(_value) + " Part Name:" + std::to_string(_name)+"\n");
	return string("("+ std::to_string(_index)+","+std::to_string(_value)+","+ std::to_string(_name) +")");
}
Share Share::operator+(const Share& other) const {
	if(!isAddable(other))
		throw std::exception(__FUNCTION__ "Shares are not addible!");
	if (this->_value.first.getName() != other._value.first.getName())
		throw std::exception(__FUNCTION__ "Shares name is not same!");
	InputPart a(this->_value.first.getValue() + other._value.first.getValue(), this->_value.first.getIndex(),other._value.first.getName());
	InputPart b(this->_value.second.getValue() + other._value.second.getValue(), this->_value.second.getIndex(),other._value.first.getName());
	return Share(a, b);
}
Share Share::operator+(long scalar) {

	try {
		return Share (InputPart((*this)[0].getValue() + scalar,_value.first.getIndex(),_value.first.getName()), (*this)[1]);

	}
	catch (...) {
		return *this;
	}
}
Share Share::operator*(int constant) {
	InputPart a(this->_value.first.getValue()*constant, this->_value.first.getIndex(),_value.first.getName());
	InputPart b(this->_value.second.getValue()*constant, this->_value.second.getIndex(), _value.second.getName());
	return Share(a, b);
}
string Share::toString()const {
	return "("+string(this->_value.first.toString() +","+ this->_value.second.toString())+")";
}
