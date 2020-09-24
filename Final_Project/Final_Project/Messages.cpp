#include "Messages.h"

Message::Message(byte type) :_type(type), _size(0),_data(nullptr) {
	setSize(type);
}
std::mutex& Message::getDataMutex() {
	return _dataMutex;
}
std::unique_lock<std::mutex>& Message::getIsReadMutex() {
	return _isReadMutex;
}
void Message::setSize(int type,unsigned int size) {
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
	case PROOF_MESSAGE:
		_size = size;
	}
	if (oldSize < _size) {
		_dataMutex.lock();
		if (_data) {
			delete _data;
		}
		_data = new byte[_size]();
		_dataMutex.unlock();
	}
	else
		memset(_data, 0, _size);
}
short Message::getSize()const { return _size; }

void Message::setData(const byte* dataPtr) {
	_dataMutex.lock();
	memcpy(_data, dataPtr, _size);
	_dataMutex.unlock();
}
byte* Message::getData()const { return _data; }
Message::~Message() {
	if (_data)
		delete[] _data;
		_data = nullptr;
}