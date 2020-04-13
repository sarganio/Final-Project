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
	AutoSeededRandomPool rnd;
	//Generate a random key
	//CryptoPP::SecByteBlock key1(0x00, AES::DEFAULT_KEYLENGTH);
	//rnd.GenerateBlock(key1, key1.size());

	// Generate a random IV
	CryptoPP::SecByteBlock iv(IV,KEY_LEN);
	//rnd.GenerateBlock(iv, iv.size());

	//byte plainText[] = "Hello! How are you.";
	//size_t messageLen = std::strlen((char*)plainText) + 1;


	// EncryptAES
	CryptoPP::CFB_Mode<AES>::Encryption cbcEncryption(key, key.size(), iv);
	cbcEncryption.ProcessData(plainText, plainText, messageLen);

	//decryptAES(plainText, messageLen, key, iv);
	//cout << "Decrypted text: " << plainText << endl;
}
//void Helper::decryptAES(byte* chiperText, size_t messageLen, CryptoPP::SecByteBlock key, CryptoPP::SecByteBlock iv) {
//	 //Decrypt AES
//	CryptoPP::CFB_Mode<AES>::Decryption cfbDecryption(key, key.size(), iv);
//	cfbDecryption.ProcessData(chiperText, chiperText, messageLen);
//	cout << chiperText << endl;
//
//}