#include "hasher.h"

string Hasher::generateFromFile(string fpath,bool &res)
{
    string digest;
    SHA3_256 sha3;

    //Checking if the file exists
    if(access(fpath.c_str(),F_OK) == -1)
    {
        res = false;
        return NULL;
    }

    try
    {
        FileSource(fpath.c_str(), true, new HashFilter(sha3, 
                                    new HexEncoder(
                                        new StringSink(digest)
                                        )
                                )
                      );

        res = true;
    }
    catch(CryptoPP::Exception& e)
    {
        res = false;
        return NULL;
    }
    
    return digest;
}

bool Hasher::verifyIntegrity(string fpath,string hash,bool &res)
{
    return (Hasher::generateFromFile(fpath,res) == hash);
}