#pragma once
#include "Share.h"
#include <vector>
#include "Helper.h"

using std::pair;
using std::vector;
class Part {
protected:
	unsigned short _index;
	char _name;
public:
	Part(unsigned short index = 0, char name = 0);
	virtual ~Part() {};
	virtual long getValue()const = 0;
	virtual string toString()const = 0;
	inline unsigned short getIndex()const;
	inline char getName()const;
};
class BytePart:public Part {
private:
	vector<byte> _value;
	unsigned short _size;
public:
	BytePart(byte* value = 0,unsigned short numOfBytes = 0, unsigned short index = 0, char name = 0);
	inline unsigned short getSize()const;
	inline vector<byte> getByteValue()const;
};
class InputPart :public Part {
private:
	long _value = 0;

public:
	InputPart(long value = 0, unsigned short index = 0, char name = 0);
	inline long getValue()const;
	inline void setValue(long value);
	string toString()const;
};
class Share {
private:
	pair<Part*, Part*> _value;
public:
	~Share();
	Share();
	Share( Part* v1,Part* v2);

	bool isAddable(const Share& other)const;

	Share operator+(const Share& b) const;
	Share operator+(long scalar)const;

	Part& operator[](unsigned short index)const;

	Share operator*(int b)const;

	string toString()const;
};
