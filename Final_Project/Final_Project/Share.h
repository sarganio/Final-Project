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
	vector<byte> _value;
	BytePart(byte* value = 0, unsigned short index = 0, char name = 0);
};
class InputPart :public Part {
private:
	long _value;

public:
	InputPart(long value = 0, unsigned short index = 0, char name = 0);
	inline long getValue()const;
	inline void setValue(long value);
	string toString()const;
};
class Share {
private:
	pair<Part&, Part&> _value;
public:
	Share(const Part& v1, const Part& v2);

	bool isAddable(const Share& other)const;

	Share operator+(const Share& b) const;
	Share operator+(long scalar);

	Part& operator[](unsigned short index);

	Share operator*(int b);

	string toString()const;
};
