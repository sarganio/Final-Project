#include "Messages.h"

Message::Message(byte type) :_type(type), _size(0) {
	setSize(type);
}
std::mutex& Message::getMutex() {
	return _m;
}
void Message::setSize(int type) {
	unsigned short oldSize = _size;
	switch (type)
	{
	case SEQ:
		_size = SEQ_LEN;
		break;
	case KEY:
		_size = KEY_LEN;
		break;
	case RECONSTRUCT:
		_size = RECONSTRUCT_LEN;
		break;
	case ENC_INPUT:
		_size = ENC_INPUT_LEN;
		break;
	case MUL_GATE:
		_size = MUL_GATE_LEN;
		break;
	}
	if (oldSize < _size) {
		_m.lock();
		if (_data) {
			delete _data;
		}
		_data = new byte[_size]();
		_m.unlock();
	}
	else
		memset(_data, 0, _size);
}
short Message::getSize()const { return _size; }
bool Message::getIsRead()const { return _isRead; }
void Message::setData(const byte* dataPtr) {
	_m.lock();
	memcpy(_data, dataPtr, _size);
	_m.unlock();
}
void Message::setIsRead(bool val) { this->_isRead = val; }
byte* Message::getData()const { return _data; }
Message::~Message() {
	if (_data)
		delete[] _data;
		_data = nullptr;
}