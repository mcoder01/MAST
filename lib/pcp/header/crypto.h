#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <cryptopp/rsa.h>
using CryptoPP::RSA;
using CryptoPP::RSASS;
using CryptoPP::InvertibleRSAFunction;

#include <cryptopp/pssr.h>
using CryptoPP::PSS;

#include <cryptopp/sha.h>
using CryptoPP::SHA1;
using CryptoPP::SHA256;
using CryptoPP::OAEP;

#include <cryptopp/filters.h>
using CryptoPP::SignerFilter;
using CryptoPP::SignatureVerificationFilter;
using CryptoPP::StringSink;
using CryptoPP::StringSource;

#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;

/// @brief Error that Crypto class could encounter
enum class CryptoError
{
    OK, //No errors encountered
    ENCRYPTION_ERROR,
    SIGNING_ERROR,
    DECRYPTION_ERROR,
    VERIFICATION_ERROR
};


/// @brief Class responsible for PCP cryptographic operations
class Crypto
{
    private:
      
        /// @brief Local private key
        RSA::PrivateKey privateKey;

        /// @brief Local public key
        RSA::PublicKey publicKey;

        /// @brief Pubic key of the process to which communicate
        RSA::PublicKey processPublicKey;
        
        /**
         * @brief Generates local public and private keys 
         * 
         */
        void RsaKeyGen();

    public:

        /**
         * @brief Construct a new Crypto object
         * 
         */
        Crypto();

        /**
         * @brief Sets the public key of the process to which communicate
         * 
         * @param key Public key to set
         */
        void setProcessPublicKey(string pkey);

        /**
         * @brief Gets the local public key
         * 
         * @return string String containing the local public key
         */
        string getPublicKey();

        /**
         * @brief Encrypts and signs a string
         * 
         * @param plain String to encrypt
         * @param error Error code resulting after the operation
         * @return string Encrypted string
         */
        string encrypt(string plain,CryptoError &error);

        /**
         * @brief Decrypts and verifies the signature of a string
         * 
         * @param cipher String to decrypt
         * @param error Error code resulting after the operation
         * @return std::string Decrypted string
         */
        string decrypt(string cipher,CryptoError &error);

};

#endif