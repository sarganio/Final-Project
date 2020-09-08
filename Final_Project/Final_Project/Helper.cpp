#include "Helper.h"
#include "Messages.h"

bool Helper::IPCompare(string first, string second) {
	string firstEnd = first.substr(first.rfind(".") + 1, string::npos);
	string secondtEnd = second.substr(second.rfind(".") + 1, string::npos);
	if (firstEnd.size() == secondtEnd.size())
		return first > second;
	return first.size() > second.size();
}

void Helper::encryptAES(byte* plainText, size_t messageLen,SecByteBlock key,byte* IV) {
	// Generate a random IV
	CryptoPP::SecByteBlock iv(IV,KEY_LEN);
	// EncryptAES
	CryptoPP::CFB_Mode<AES>::Encryption cbcEncryption(key, key.size(), iv);
	cbcEncryption.ProcessData(plainText, plainText, messageLen);
}