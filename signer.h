#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <sodium.h>

using namespace std;

struct Signer
{
  Signer(const string& seedVal, size_t messageBufferSz);
  ~Signer()
  {
  	if (signedMessage)
  	  delete[] signedMessage;
  	if (unsignedMessage)
  	  delete[] unsignedMessage;	
  };

  bool checkSignature(const string& signedMsg, string& message) const;
  string addSignature(const string& message) const;

private:
  //public, private and seed key values	
  unsigned char pk[crypto_sign_PUBLICKEYBYTES];
  unsigned char sk[crypto_sign_SECRETKEYBYTES];
  unsigned char sd[crypto_sign_SEEDBYTES];
  
  const size_t messageBufferSz;
  //crypto buffers
  mutable unsigned char *signedMessage;
  mutable unsigned long long signedMessageSz;
  mutable unsigned char *unsignedMessage;
  mutable unsigned long long unsignedMessageSz;

  void hexToBinary(const string& hexString, unsigned char* buffer, unsigned long long& size) const;
  std::string binaryToHex(const unsigned char* buffer, unsigned long long size) const;
};

