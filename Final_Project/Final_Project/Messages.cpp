#include "Messages.h"

Message::Message(byte type) :_type(type), _size(0) {
	setSize(type);
	_data = new byte[1 + (type ? _size : MAX_MESSAGE_SIZE)]();//Increment by 1 for null character. Allocate MAX_MESSAGE_SIZE in case type = 0.
}
void Message::setSize(int type) {
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
	default:
		break;
	}
	if(_data)
}
short Message::getSize()const { return _size; }
bool Message::getIsRead()const { return _isRead; }
void Message::setData(const byte* dataPtr) {
	memcpy(_data, dataPtr, _size);
}
void Message::setIsRead(bool val) { this->_isRead = val; }
byte* Message::getData()const { return _data; }
Message::~Message() {
	if (_data)
		delete[] _data;
	_data = nullptr;
}