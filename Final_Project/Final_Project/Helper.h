#pragma once
#ifdef _DEBUG // vs add this define in debug mode
#include <stdio.h>
// Q: why do we need traces ?
// A: traces are a nice and easy way to detect bugs without even debugging
// or to understand what happened in case we miss the bug in the first time
#define TRACE(msg, ...) printf(msg "\n", __VA_ARGS__);
// for convenient reasons we did the traces in stdout
// at general we would do this in the error stream like that
// #define TRACE(msg, ...) fprintf(stderr, msg "\n", __VA_ARGS__);

#else // we want nothing to be printed in release version
#define TRACE(msg, ...) printf(msg "\n", __VA_ARGS__);
#define TRACE(msg, ...) // do nothing
#endif

#include "CryptoPP\pch.h"
#include "CryptoPP\aes.h"
#include "CryptoPP\osrng.h"
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>

using CryptoPP::AutoSeededRandomPool;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "CryptoPP\cryptlib.h"
using CryptoPP::Exception;

#include "CryptoPP\hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "CryptoPP\filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include "CryptoPP\aes.h"
using CryptoPP::AES;

#include "CryptoPP\ccm.h"
using CryptoPP::CBC_Mode;

#include "CryptoPP\secblock.h"
using CryptoPP::SecByteBlock;

#include <cstdlib>
using std::exit;

//byte variable type
#include <cstddef>
using CryptoPP::byte;

#define NTL_NO_MIN_MAX
#include "NTL\ZZ_p.h"

#include <string>

#define ZP 127 //make sure all the elements are belong to Z_p (mod p)
#define L  5//num of multipication gates per G gate
#define INPUTS_PER_G_GATE  6//(u_i,u_{i-1},v_i,v_{i-1},alpha_i,z_i)
#define ELEMENT_SIZE sizeof(NTL::ZZ_p)
#define DISHONEST_PARTY_ID -1//the id of the dishonest party
#define LIE_MUL_GATE_INDEX 3//the index of the multipication gate at which the dishonest party cheat


using std::string;

class Helper
{
public:
	//returns true if last octet is bigger that second's
	static bool IPCompare(string first, string second);										//return true if first is greater than second.
	static void encryptAES(byte* plainText, size_t messageLen, SecByteBlock key,byte* IV);	//perform an AES encryption.
};

