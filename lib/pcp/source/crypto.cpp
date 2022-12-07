#include "../header/crypto.h"

Crypto::Crypto()
{
    //Parameters generation
    AutoSeededRandomPool prg;
    InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(prg, 4096);

    //Keys generation
    this->privateKey = RSA::PrivateKey(params);
    this->publicKey = RSA::PublicKey(params);
}

string Crypto::encrypt(string plain,CryptoError &error)
{
    string cipher,signature;
    AutoSeededRandomPool rng;
        
    //Encryption
    try
    {
        CryptoPP::RSAES<OAEP<SHA256>>::Encryptor e(this->processPublicKey);
        StringSource(plain, true,
                     new CryptoPP::PK_EncryptorFilter(rng,e,
                                                      new CryptoPP::StringSink(cipher)
                                                     )
                    );
    }
    catch(CryptoPP::Exception& e)
    {
        error = CryptoError::ENCRYPTION_ERROR;
        return NULL;
    }

    //Signature
    try
    {
        RSASS<PSS,SHA1>::Signer signer(privateKey);

        StringSource(cipher, true, 
                    new SignerFilter(rng, signer,
                                    new StringSink(signature)
                                    )
                    );
    }
    catch(CryptoPP::Exception& e)
    {
        error = CryptoError::SIGNING_ERROR;
        return NULL;
    }

    error = CryptoError::OK;

    return cipher+signature;
}

string Crypto::decrypt(string cipher,CryptoError &error)
{
    string msg,plain;
    AutoSeededRandomPool rng;


    //Signature verification
    try
    {
        RSASS<PSS,SHA1>::Verifier verifier(this->processPublicKey);

        StringSource(cipher, true,
                     new SignatureVerificationFilter(verifier, 
                                                     new StringSink(msg),
                                                     SignatureVerificationFilter::THROW_EXCEPTION | 
                                                     SignatureVerificationFilter::PUT_MESSAGE)
                    );
    }
    catch(CryptoPP::Exception& e)
    {
        error = CryptoError::VERIFICATION_ERROR;
        return NULL;
    }

    // Decryption of the message
    try
    {
        CryptoPP::RSAES<OAEP<SHA256>>::Decryptor d(this->privateKey);

        StringSource(msg, true,
                     new CryptoPP::PK_DecryptorFilter(rng,d,
                                                      new StringSink(plain)
                                                     ) 
                    );
    }
    catch(CryptoPP::Exception& e)
    {
        error = CryptoError::DECRYPTION_ERROR;
        return NULL;
    }

    error = CryptoError::OK;

    return plain;   
}

void Crypto::setProcessPublicKey(string pkey)
{
    StringSource stringSource(pkey, true);
    this->processPublicKey.BERDecode(stringSource);
}

string Crypto::getPublicKey()
{
    string pkey;
    StringSink stringSink(pkey);
    this->publicKey.DEREncode(stringSink);

    return pkey;
}