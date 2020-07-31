#pragma once

#include "Helper.h"
#include "Messages.h"
#include "PartyShare.h"
#include <vector>

using std::pair;
using std::vector;

class Party;
class PartyShare;

class Part {
protected:				
	long _value = 0;
	unsigned short _index;//the index of the part e.g a_# , x_#
	char _name;
public:
	Part(char name = 0, unsigned short index = 0, long value = 0);
	Part(const Part& other);
	~Part() {};
	inline long getValue()const;
	inline void setValue(long value);
	string toString()const;
	inline unsigned short getIndex()const;
	inline char getName()const;
	Part& operator=(long newVal);
};
class PartyShare;
class Share {
private:
	pair<Part*, Part*> _value;
public:
	~Share();
	Share(unsigned short index = 0, char name = 0);
	Share(const Share& other);
	Share(Part* v1,Part* v2);

	bool isAddable(const Share& other)const;

	Share& operator=(Share const & other);
	Share operator+(const Share& other) const;
	Share operator+(long scalar)const;

	Part& operator[](unsigned short index)const;

	Share operator*(int b)const;
	Share operator*(PartyShare& other)const;

	string toString()const;
};