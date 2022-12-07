#include "../header/crypto.h"

#include <iostream>
using std::cout;
using std::endl;

Crypto::Crypto()
{
    AutoSeededRandomPool prg;

    //Encryption key generation
    this->key = SecByteBlock(AES::DEFAULT_KEYLENGTH);
	prg.GenerateBlock(key, key.size());
}

Crypto::Crypto(string key,bool hexencoded)
{
    AutoSeededRandomPool prg;
    CryptoPP::byte decoded[AES::BLOCKSIZE];

    //Manupulating encryption key (if necessary)
    if(hexencoded)
    {  
        StringSource(key, true,
			     new HexDecoder(
			                    new ArraySink(decoded,AES::BLOCKSIZE)
                               )
                );
    }
    else
        StringSource(key, true, new ArraySink(decoded,AES::BLOCKSIZE));

    this->key = SecByteBlock(decoded,AES::BLOCKSIZE);

}

bool Crypto::encrypt(string plain, string iv, string &cipher)
{
    AutoSeededRandomPool rng;
    CryptoPP::byte decoded[AES::BLOCKSIZE];
    cipher = string();

    //Decoding the IV
    StringSource(iv, true,
			     new HexDecoder(
			                    new ArraySink(decoded,AES::BLOCKSIZE)
                               )
                );
    SecByteBlock siv = SecByteBlock(decoded,AES::BLOCKSIZE);

    try
    {
        GCM<AES>::Encryption en;

        //Setting the key with the IV
        en.SetKeyWithIV(this->key,this->key.size(),siv,siv.size());

        //Encryption
        StringSource(plain, true, 
			         new AuthenticatedEncryptionFilter(en,
			                new HexEncoder(
                                new StringSink(cipher)
                                )
                        )     
		            );

        return true;
    }
    catch(CryptoPP::Exception& e)
    {
        return false;
    }

}

bool Crypto::decrypt(string cipher, string iv, string &plain)
{
    AutoSeededRandomPool rng;
    CryptoPP::byte decoded[AES::BLOCKSIZE];
    plain = string();

    //Decoding the IV
    StringSource(iv, true,
			     new HexDecoder(
			                    new ArraySink(decoded,AES::BLOCKSIZE)
                               )
                );
    SecByteBlock siv = SecByteBlock(decoded,AES::BLOCKSIZE);

    try
    {
        GCM<AES>::Decryption de;

        //Setting the key with the iv
        de.SetKeyWithIV(this->key,this->key.size(),siv,siv.size());

        //Decryption
        StringSource(cipher, true,
                     new HexDecoder(
                        new AuthenticatedDecryptionFilter(de,
                                new StringSink(plain)
                            )
                        ) 
                    );

        return true;
    }
    catch(CryptoPP::Exception& e)
    {
        return false;
    }
 
}

string Crypto::getKey()
{
    string encoded;
    
    StringSource(this->key, this->key.size(), true,
			     new HexEncoder(
			                    new StringSink(encoded)
                               )
                );

    return encoded;
}

string Crypto::generateIV()
{
    AutoSeededRandomPool prg;
    string encoded;

    //Initialization vector generation
    SecByteBlock iv= SecByteBlock(AES::BLOCKSIZE);
	prg.GenerateBlock(iv, iv.size());
    
    //Encoding the generated IV
    StringSource(iv, iv.size(), true,
                 new HexEncoder(
			                    new StringSink(encoded)
                               )
                );

    return encoded;
}