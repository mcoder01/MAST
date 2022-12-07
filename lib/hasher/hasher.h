#ifndef HASHER_H
#define HASHER_H

#include <cryptopp/sha3.h>
using CryptoPP::SHA3_256;

#include <cryptopp/filters.h>
using CryptoPP::StringSink;
using CryptoPP::HashFilter;

#include <cryptopp/files.h>
using CryptoPP::FileSource;

#include <cryptopp/hex.h>
using CryptoPP::HexEncoder;

#include <string>
using std::string;

#include <unistd.h>

/// @brief Class used for generating an hash of a file
class Hasher
{

    public:
        /**
         * @brief Construct a new Hasher object
         * 
         */
        Hasher() {}

        /**
         * @brief Generates an hash
         * 
         * @param path Path of the file to hash
         * @param res Output value indicating the success of the generation
         * 
         * @return The generated hash
         * 
         */
        static string generateFromFile(string fpath,bool &res);

        /**
         * @brief Generates an hash
         * 
         * @param path Path of the file to hash
         * @param hash Hash to confront with
         * @param res Output value indicating the success of the generation
         * 
         * @return If the hashes match
         */
        static bool verifyIntegrity(string fpath,string hash,bool &res);

};

#endif