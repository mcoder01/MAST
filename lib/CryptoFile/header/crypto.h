#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <cryptopp/cryptlib.h>
using CryptoPP::Exception;

#include <cryptopp/hex.h>
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include <cryptopp/filters.h>
using CryptoPP::ArraySink;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::AuthenticatedEncryptionFilter;
using CryptoPP::AuthenticatedDecryptionFilter;

#include <cryptopp/aes.h>
using CryptoPP::AES;

#include <cryptopp/gcm.h>
using CryptoPP::GCM;

#include <cryptopp/secblock.h>
using CryptoPP::SecByteBlock;

#include "cryptopp/osrng.h"
using CryptoPP::AutoSeededRandomPool;

/// @brief Class responsible for CryptoFile cryptographic operations
class Crypto
{
    private:
      
        /// @brief Encryption key
        SecByteBlock key;

    public:

        /**
         * @brief Construct a new Crypto object and generates the key
         * 
         */
        Crypto();

        /**
         * @brief Construct a new Crypto object
         * 
         * @param key Encryption key
         * @param hexencoded If the key is encoded in hexadecimal
         */
        Crypto(string key,bool hexencoded);

        /**
         * @brief Gets the encryption key (coded in hexadecimal)
         * 
         * @return string String containing the key
         */
        string getKey();

        /**
         * @brief Generates an initialization vector (coded in hexadecimal)
         * 
         * @return string String containing the initialization vector
         */
        string generateIV();

        /**
         * @brief Encrypts and signs a string
         * 
         * @param plain String to encrypt
         * @param iv Initialization vector (in hexadecimal encoding)
         * @param cipher Encrypted string
         * @return bool If the operation succeded
         */
        bool encrypt(string plain, string iv, string &cipher);

        /**
         * @brief Decrypts and verifies the signature of a string
         * 
         * @param cipher String to decrypt
         * @param iv Initialization vector (in hexadecimal encoding)
         * @param plain Decrypted string
         * @return bool If the operation succeded
         */
        bool decrypt(string cipher, string iv, string &plain);

};

#endif