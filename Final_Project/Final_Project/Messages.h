#pragma once
//#define WINDOWS_IGNORE_PACKING_MISMATCH
#include "secblock.h"
#include<cstdint>
#include <cstddef>
using CryptoPP::byte;

#define KEY_LEN 32
#define SEQ_LEN 4
#define RECONSTRUCT_LEN 22 //size of Share (two parts)
#define NUM_OF_PARTIES 3
#define MAX_MESSAGE_SIZE 64
#define ENC_INPUT_LEN sizeof(long)

#define BASE_PORT 62000
#define BASE_IP "192.168.0."

#define HEADER_SIZE 3
#define ALIGNIG sizeof(void*)

enum types{SEQ = 1,KEY,RECONSTRUCT, ENC_INPUT,MUL_GATE};

//#pragma pack(1)//allow no padding
#pragma pack(push, 1)
typedef class Message {
private:
	byte _type;				//an identifier to ID each type of message.
	unsigned short _size;	//the size of the accual message without header(_type+_size)
	byte* _data;			//an array of length _size + 1 for null character. 
	bool _isRead = true;	//flag to inidicate wether the message was read.
public:
	Message(byte type = 0);				
	void setSize(int type);
	short getSize()const;
	bool getIsRead()const;
	void setData(const byte* dataPtr);
	void setIsRead(bool val);
	byte* getData()const;
	~Message();
} Message;
#pragma pack(pop)