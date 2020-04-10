#include <iostream>
#include <string>

#include "cryptlib.h"
#include "filters.h"
#include "files.h"
#include "modes.h"
#include "queue.h"
#include "hex.h"
#include "aes.h"
#include "osrng.h."
//using namespace CryptoPP;
using CryptoPP::AutoSeededRandomPool;
using CryptoPP::SecByteBlock;
using CryptoPP::AES;
using CryptoPP::byte;
using CryptoPP::CFB_Mode;

int main()
{
    AutoSeededRandomPool rnd;

    // Generate a random key
    SecByteBlock key(0x00, AES::DEFAULT_KEYLENGTH);
    rnd.GenerateBlock(key, key.size());

    // Generate a random IV
    SecByteBlock iv(AES::BLOCKSIZE);
    rnd.GenerateBlock(iv, iv.size());

    byte plainText[] = "Hello! How are you.";
    size_t messageLen = std::strlen((char*)plainText) + 1;

    //////////////////////////////////////////////////////////////////////////
    // Encrypt

    CFB_Mode<AES>::Encryption cfbEncryption(key, key.size(), iv);
    cfbEncryption.ProcessData(plainText, plainText, messageLen);

    //////////////////////////////////////////////////////////////////////////
    // Decrypt

    CFB_Mode<AES>::Decryption cfbDecryption(key, key.size(), iv);
    cfbDecryption.ProcessData(plainText, plainText, messageLen);
    return 0;
}