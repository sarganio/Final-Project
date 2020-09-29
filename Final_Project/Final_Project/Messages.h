#pragma once
#include "Helper.h"
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
#define F_VERIFY_ROUND2_MESSAGE_LEN L*6+2



#define BASE_PORT 62000
#define BASE_IP "192.168.0."

#define HEADER_SIZE 3
#define ALIGNIG sizeof(void*)

enum types{SEQ = 1,KEY,RECONSTRUCT, ENC_INPUT,MUL_GATE, F_VERIFY_ROUND1_MESSAGE, F_VERIFY_ROUND2_MESSAGE
};

//#pragma pack(1)//allow no padding
#pragma pack(push, 1)
typedef class Message {
private:
	byte _type;									//an identifier to ID each type of message.
	unsigned short _size;						//the size of the accual message without header(_type+_size)
	byte* _data;								//an array of length _size + 1 for null character. 
	std::mutex _dataMutex;						//mutex protector of the data in message
	std::mutex _isSetSizeMutex;					//mutex protector of the size of message
	std::mutex _isReadMutex;					//mutex to make sure that old messages dont get confused with new messages.
	std::condition_variable _listenerCV, _partyCV, _isSetSizeCV;
	bool _isRead = true;						//true if the current message was read by the party thread.

public:
	Message(byte type = 0);				
	void setSize(int type,unsigned int size = 0);//setter for the size of the message
	short getSize()const;						//getter for the size of message
	void setData(const byte* dataPtr);			//copy the data from dataPtr into the message
	byte* getData()const;						//getter for data pointer
	std::mutex& getDataMutex();					//getter for the data mutex
	std::mutex& getIsSetSizeMutex();				//getter for the setSize mutex
	bool getIsRead()const;						//getter for _isRead flag
	void setIsRead(bool val);					//setter for _isRead flag
	std::mutex& getIsReadMutex();				//getter for isReadMutex
	std::condition_variable& getListenerCV();	//getter for ListenerCV
	std::condition_variable& getPartyCV();		//getter for PartyCV
	std::condition_variable& getIsSetSizeCV();	//getter for IsSetSizeCV
	~Message();
} Message;
#pragma pack(pop)