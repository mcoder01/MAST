#ifndef CRYPTOFILEMAP_H
#define CRYPTOFILEMAP_H

 #include "cryptofile.h"

class CryptoFile;

/// @brief Class for interacting with a record based, key-value pair based encrypted file
class CryptoFileMap
{
    private:

        /// @brief CryptoFile that manages the CryptoFileMap file
        CryptoFile *cryptoFile = NULL;

        /// @brief Separator used for the key-value pairs
        const char pairSeparator = ':';

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
         * @brief Construct a new CryptoFileMap object
         * 
         * @param fname Path of the file to use
         * @param pwd Password used for cryptographic operation
         * @param success If the CryptoFileMap initialization succeded
         */
        CryptoFileMap(string fname, string pwd, bool &success);

        /**
         * @brief Destroy the CryptoFileMap object
         * 
         */
        ~CryptoFileMap();

        /**
         * @brief Adds a key-value pair to the file
         * 
         * @param key Key of the pair
         * @param value Value of the pair
         * @return bool If the operation succeded
         */
        bool write(string key, string value);

        /**
         * @brief Reads a value from a key-value pair
         * 
         * @param key Key of the pair
         * @param success If the operation succeded
         * @return string The obtained value
         */
        string read(string key, bool &success);
};

#endif