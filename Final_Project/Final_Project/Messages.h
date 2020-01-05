#pragma once
#include<cstdint>
#include<WinSock2.h>
#define KEY_LEN 32
#define SEQ_LEN 4
#define RECONSTRUCT_LEN 64

enum types{SEQ = 0,KEY,RECONSTRUCT};

class Message {
private:
	uint8_t _type;
	short _size;
	char* _data;
public:
	Message(uint8_t type):_type(type) {
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
		_data = new char[_size + 1];//increment for type first field
		_data[0] = _type;
	}
	void setData(const char* dataPtr) {
		memcpy(_data + 1, dataPtr, _size);
	}
	bool sendMsg(SOCKET& sock, Message& msg) {
		short size = msg._size;
		int wasSent = send(sock, _data,size , 0);
		return wasSent == size;
	}
	~Message(){
		if(_data)
			delete[] _data;
		_data = nullptr;
	}
};
/*
#pragma pack(1)
typedef struct Seq:public Message {
	char _key[SEQ_LEN];
} Seq;
#pragma pack(1)
typedef struct Key :public Message {
	char _key[KEY_LEN];
} Seq;
#pragma pack(1)
typedef struct Reconstruct :public Message {
	char _key[KEY_LEN];
} Seq;
*/