#pragma once
#define WINDOWS_IGNORE_PACKING_MISMATCH
#include<cstdint>

#define KEY_LEN 32
#define SEQ_LEN 4
#define RECONSTRUCT_LEN 64
#define MAX_MESSAGE_SIZE 65
#define NUM_OF_PARTIES 3

#define BASE_PORT 62000
#define BASE_IP "192.168.0."

#define HEADER_SIZE 3

enum types{SEQ = 1,KEY,RECONSTRUCT};

#pragma pack(1)//allow no padding
typedef class Message {
private:
	uint8_t _type;
	unsigned short _size;
	char* _data; 
public:
	Message(unsigned char type = 0):_type(type),_size(0){
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
		default:
			break;
		}
		_data = new char[_size + 1]();//Increment by 1 for null character

	}
	short getSize()const { return _size; }
	void setData(const char* dataPtr) {
		memcpy(_data , dataPtr, _size);
	}
	char* getData()const { return _data; }

	~Message(){
		if(_data)
			delete[] _data;
		_data = nullptr;
	}
} Message;