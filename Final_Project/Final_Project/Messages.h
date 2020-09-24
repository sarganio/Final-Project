#pragma once
//#define WINDOWS_IGNORE_PACKING_MISMATCH
#include "secblock.h"
#include <mutex>
#include<cstdint>
#include <cstddef>
using CryptoPP::byte;

#define KEY_LEN 32
#define SEQ_LEN 4
#define RECONSTRUCT_LEN 14 //size of Share (two parts):2*(4B(long)+2B(short)+1B(unsighed char))
#define NUM_OF_PARTIES 3
#define MAX_MESSAGE_SIZE 64
#define ENC_INPUT_LEN sizeof(long)
#define MUL_GATE_LEN sizeof(long)



#define BASE_PORT 62000
#define BASE_IP "192.168.0."

#define HEADER_SIZE 3
#define ALIGNIG sizeof(void*)

enum types{SEQ = 1,KEY,RECONSTRUCT, ENC_INPUT,MUL_GATE, PROOF_MESSAGE};

//#pragma pack(1)//allow no padding
#pragma pack(push, 1)
typedef class Message {
private:
	byte _type;				//an identifier to ID each type of message.
	unsigned short _size;	//the size of the accual message without header(_type+_size)
	byte* _data;			//an array of length _size + 1 for null character. 
	bool _isRead = true;	//flag to inidicate wether the message was read.
	std::mutex _m;
public:
	Message(byte type = 0);				
	void setSize(int type,unsigned int size = 0);
	short getSize()const;
	bool getIsRead()const;
	void setData(const byte* dataPtr);
	void setIsRead(bool val);
	byte* getData()const;
	std::mutex& getMutex();
	~Message();
} Message;
#pragma pack(pop)