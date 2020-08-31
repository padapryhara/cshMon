#include <iostream>
#include <cstring>
#include <numeric>
#include <assert.h>  

#include "signer.h"


Signer::Signer(const string& seedVal, size_t messageBufferSz) :
  signedMessage(nullptr),
  unsignedMessage(nullptr),
  messageBufferSz(messageBufferSz)
{
  if (sodium_init() < 0)
  {
    cout << "Error: libsodium was failed" << endl;
    return;
  }

  assert(seedVal.length() >= crypto_sign_SEEDBYTES);
  memcpy(sd, seedVal.c_str(), crypto_sign_SEEDBYTES);
  crypto_sign_seed_keypair(pk, sk, sd);

  unsignedMessage = new unsigned char[messageBufferSz];
  signedMessage = new unsigned char[messageBufferSz];    
}

bool Signer::checkSignature(const string& signedMsg, string& message) const
{
  assert(messageBufferSz >= signedMsg.length());

  hexToBinary(signedMsg, signedMessage, signedMessageSz);
  auto rv = crypto_sign_open(unsignedMessage, &unsignedMessageSz, signedMessage, signedMessageSz, pk);
  if (rv == 0)  	
    message = string(unsignedMessage, unsignedMessage + unsignedMessageSz);

  return rv == 0;
}

string Signer::addSignature(const string& message) const
{
   unsignedMessageSz = message.length();
   assert(messageBufferSz >= unsignedMessageSz);
    
   memcpy(unsignedMessage, message.c_str(), unsignedMessageSz);    
   crypto_sign(signedMessage, &signedMessageSz, unsignedMessage, unsignedMessageSz, sk);

   return binaryToHex(signedMessage, signedMessageSz);   
}

void Signer::hexToBinary(const string& hexString, unsigned char* buffer, unsigned long long& size) const
{
	size = 0;
	for (unsigned int len = hexString.length(), i = 0; i < len; i += 2)
	{
		auto byteString = hexString.substr(i, 2);
		buffer[size++] = static_cast<unsigned char>(strtol(byteString.c_str(), NULL, 16));
	}
}

std::string Signer::binaryToHex(const unsigned char* buffer, unsigned long long size) const
{
	stringstream str;
	str.setf(ios_base::hex, ios::basefield);
	str.setf(ios_base::uppercase);
	str.fill('0');
	
	for(auto i = 0; i < size; ++i)
	{
		str.width(2);
		str << static_cast<int>(buffer[i]);
	}

	return str.str();
}
