#ifndef CRYPTOFILE_H
#define CRYPTOFILE_H

#include <cryptopp/blake2.h>
using CryptoPP::BLAKE2b;

#include <cryptopp/filters.h>
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::HashFilter;

#include <cryptopp/cryptlib.h>
using CryptoPP::Exception;

#include <cryptopp/hex.h>
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include <fstream>
using std::ofstream;
using std::ifstream;
using std::fstream;
using std::ios;
using std::streampos;

#include <string>
using std::string;
using std::to_string;

#include <unistd.h>

#include "crypto.h"
#include "cryptofilemap.h"

class CryptoFileMap;

/*
    File structure:

        The file is structured in records. Each record is called a location and
            it can be accessed using its index

        Reserved records: number : explanation
                            0    : hash of the file
                            n+1  : number of records

        Record structure:

        ||===================|======||
        ||   Encrypted data  |  IV  ||
        ||===================|======||
        
*/

/// @brief Class for interacting with a record based encrypted file
class CryptoFile
{ 
    private:
    
        /// @brief Class used for performing cryptographyc operations
        Crypto crypto;

        /// @brief Path of the file that will be used
        string filePath;

        /// @brief Stream used for file operations
        fstream file;

        /// @brief Position of the pointer (in records number)
        int pointerPosition;

        /// @brief If the file was already created
        bool created;

        /// @brief Number of written records
        int nRecords;

        /// @brief Seaparator chararcter for records
        const char recSeparator = '\n';

        /// @brief Seaparator chararcter for records
        const char recContentSeparator = '|';


        /**
         * @brief Writes a given hash in the file
         * 
         * @param hash Hash to write in the file
         * 
         * @return bool If the operation succeded
         */
        bool writeHashToFile(string hash);

        /**
         * @brief Reads the hash saved on the file
         * 
         * @param hash The hash saved on the file
         * 
         * @return bool If the operation succeded
         */
        bool readHashFromFile(string &hash);

        /**
         * @brief Reads the number of record from the file
         * 
         * @param n The retreived number
         * 
         * @return bool If the operation succeded
         */
        bool readRecordsNumberFromFile(int &n);

        /**
         * @brief Generates a hash for the file
         * 
         * @param hash The generated hash
         * 
         * @return If the hash generation succeded
         */
        bool generateFileHash(string &hash);

        /**
         * @brief Generates a zero hash (hash for number 0)
         * 
         * @return string The generated hash
         */
        string generateZeroHash();

        /**
         * @brief Moved the pointer a position after a certain character
         * 
         * @param del Character to encounter
         * 
         * @note Starts from the current pointer position moving backwards
         */
        void setPointerPositionBackwards(char del);

        /**
         * @brief Moved the pointer a position after a certain character
         * 
         * @param del Character to encounter
         * @param s Characters capured when moving the pointer
         * 
         * @note Starts from the current pointer position moving backwards
         */
        void setPointerPositionBackwards(char del,string &s);

        /**
         * @brief Moved the pointer a position after a certain character
         * 
         * @param del Character to encounter
         * 
         * @note Starts from the current pointer position moving forward
         */
        void setPointerPositionForward(char del);

        /**
         * @brief Moved the pointer a position after a certain character
         * 
         * @param del Character to encounter
         * @param s Characters capured when moving the pointer
         * 
         * @note Starts from the current pointer position moving forward
         * 
         */
        void setPointerPositionForward(char del,string &s);

        /**
         * @brief Get a delimited substring starting from the beginning of a string
         * 
         * @param s String from which extrapolate the substring
         * @param del Delimiter of the substring
         * @return string The obtained string
         */
        string getStringFromBeg(string s, char del);

        /**
         * @brief Get a delimited substring starting from the end of a string
         * 
         * @param s String from which extrapolate the substring
         * @param del Delimiter of the substring
         * @return string The obtained string
         */
        string getStringFromEnd(string s, char del);

    public:

        /**
         * @brief Construct a new CryptoFile object
         * @note This constructor does nothing, only used for code fixes. 
         */
        CryptoFile() = default;

        /**
         * @brief Construct a new CryptoFile object
         * 
         * @param fname Path for the file to use
         * @param success If the operation succeded
         * 
         * @note This constructor automatically creates a blank file
         */
        CryptoFile(string fname, bool &success);

        /**
         * @brief Construct a new Crypto File object
         * 
         * @param fname Path for the file to use
         * @param key Encryption key
         * @param keyHexEncoded If the key is encoded in hexadecimal
         * @param success If the operation succeded
         * 
         * @note If the file does not exist, it will create a blank file
         */
        CryptoFile(string fname,string key,bool keyHexEncoded,bool &success);

        /**
         * @brief Construct a new CryptoFile object
         * 
         * @param fname Path for the file to use
         * @param keys CryptoFileMap that containins / will contain the keys 
         * @param success If the operation succeded
         */
        CryptoFile(string fname,CryptoFileMap keys,bool &success);

        /**
         * @brief Creates the file
         * 
         * @return bool If the operation succeded
         */
        bool create();

        /**
         * @brief Adds a record to the file
         * 
         * @param data Data that will be written
         * @param loc Location assigned during record creation
         * 
         * @note Things to point out:
         *         - This function does not use and/or alter the pointer position
         *         - The file must be open before using this function
         * 
         * @return bool If the operation succeded
         */
        bool addRecord(string data,int &loc);

        /**
         * @brief Opens a the file
         * 
         * @return bool If the operation succeded
         */
        bool open();

        /**
         * @brief Reads the data contained in the currently pointed record
         * 
         * @param data Data contained in the record
         * 
         * @return bool If the operation succeded
         * 
         * @note The file must be open before using this function
         */
        bool read(string &data);

         /**
         * @brief Points to the next record
         * 
         * @return bool If the operation could be done
         * 
         * @note The file must be open before using this function
         */
        bool next();

        /**
         * @brief Points to the previous record
         * 
         * @return bool If the operation could be done
         * 
         * @note The file must be open before using this function
         */
        bool previous();

        /**
         * @brief Gets the current position of the pointer
         * 
         * @return int The position of the pointer (in record number)
         * 
         * @note If the file was previously closed, the position will be the last one before file closure
         */
        int getCurrentPosition();

        /**
         * @brief Closes the file
         * 
         * @return bool If the operation succeded
         * 
         * @note To note that, if hash calculation was skipped, this function will
         *          update the file hash before closure
         */
        bool close();

        /**
         * @brief Gets the number of added records
         * 
         * @return int The number of added records
         */
        int getRecordsNumber();

        /**
         * @brief Removes the file (in hexadecimal encoding)
         * 
         * @return bool If the operation succeded
         */
        bool destroy();

        /**
         * @brief Gets the encryption key (in hexadecimal encoding)
         * 
         * @return string The encryption key
         */
        string getEncryptionKey();

        /**
         * @brief Gets the initialization vector (in hexadecimal encoding)
         * 
         * @return string The initialization vector
         */
        string getEncryptionIV();

        /**
         * @brief Verifies the integrity of the file
         * 
         * @param res If the file is not corrupted
         * 
         * @return bool If the the operation succeded
         * 
         */
        bool verifyIntegrity(bool &res);

        /**
         * @brief Gets the used file path
         * 
         * @return string The file path
         */
        string getFilePath();

};

#endif