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

#include "pch.h"
#include "aes.h"
#include "osrng.h"
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>

using CryptoPP::AutoSeededRandomPool;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "cryptlib.h"
using CryptoPP::Exception;

#include "hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include "aes.h"
using CryptoPP::AES;

#include "ccm.h"
using CryptoPP::CBC_Mode;

#include "secblock.h"
using CryptoPP::SecByteBlock;

#include <cstdlib>
using std::exit;

//byte variable type
#include <cstddef>
using CryptoPP::byte;

#define NTL_NO_MIN_MAX
#include "NTL/ZZ_p.h"

#include <string>

#define ZP 127 //make sure all the elements are belong to Z_p (mod p)
#define L  1//num of multipication gates per G gate
#define ELEMENT_SIZE sizeof(NTL::ZZ_p)


using std::string;

class Helper
{
public:
	//returns true if last octet is bigger that second's
	static bool IPCompare(string first, string second);										//return true if first is greater than second.
	static void encryptAES(byte* plainText, size_t messageLen, SecByteBlock key,byte* IV);	//perform an AES encryption.
};

