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
#include <iostream>
#include "aes.h"
#include <Windows.h>
#include "osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <cstdlib>
using std::exit;

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

//byte variable type
#include <cstddef>
using CryptoPP::byte;

#include "secblock.h"
using CryptoPP::SecByteBlock;
#include <string>

using std::string;

class Helper
{
	//static void decryptAES(byte* chiperText, size_t messageLen, CryptoPP::SecByteBlock key, CryptoPP::SecByteBlock iv);
public:
	//returns true if last octet is bigger that second's
	static bool IPCompare(string first, string second);
	static void encryptAES(byte* plainText, size_t messageLen, SecByteBlock key,byte* IV);
};

