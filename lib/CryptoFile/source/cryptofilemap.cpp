#include "../header/cryptofilemap.h"
#include <iostream>
CryptoFileMap::CryptoFileMap(string fname, string pwd, bool &success)
{
    bool e;

    //Initializing CryptoFile
    this->cryptoFile = new CryptoFile(fname,pwd,false,e);

    if(!e)
    {
        success = false;
        return;
    }

    //Opening the file
    e = this->cryptoFile->open();

    if(!e)
    {
        success = false;
        return;
    }

    success = true;
}

CryptoFileMap::~CryptoFileMap()
{
    //Closing the file
    this->cryptoFile->close();

    delete this->cryptoFile;
}

bool CryptoFileMap::write(string key, string value)
{
    bool e;
    int l;
    string kencoded,vencoded;

    //Encodicng the key
    StringSource(key,true,new HexEncoder(new StringSink(kencoded)));

    //Encodicng the value
    StringSource(value,true,new HexEncoder(new StringSink(vencoded)));

    //Adding the record
    e = this->cryptoFile->addRecord(kencoded + this->pairSeparator + vencoded,l);

    return e;
}

string CryptoFileMap::read(string key, bool &success)
{
    string res;
    bool e,found=false;
    string kdecoded,vencoded;

    //Searching for the record containing the key
    do
    {
        //Cleaning the string used for the key
        kdecoded = "";


        string rd;

        //Reading the record
        e = this->cryptoFile->read(rd);

        if(!e)
        {
            success = false;
            return "";
        }

        //Getting the key of the record
        string k = this->getStringFromBeg(rd,this->pairSeparator);

        //Decoding the key
        StringSource(k,true,new HexDecoder(new StringSink(kdecoded)));

        if(key == kdecoded)
        {
            //Extrapolating data from the record
            vencoded = this->getStringFromEnd(rd,this->pairSeparator);

            //Decodicng the value
            StringSource(vencoded,true,new HexDecoder(new StringSink(res)));

            found = true;

            break;
        }
    }
    while(this->cryptoFile->next());

    //Resetting pointer position
    while(this->cryptoFile->previous()) {}

    if(found)
    {
        success = true;
        return res;
    }
    else
    {
        success = false;
        return "";
    }

}

string CryptoFileMap::getStringFromBeg(string s, char del)
{
    string res;

    for(string::iterator it=s.begin(); it!=s.end(); it++)
    {
        if(*it == del)
            break;

        res.push_back(*it);
    }

    return res;
}

string CryptoFileMap::getStringFromEnd(string s, char del)
{
    string res;

    for(string::iterator it=s.end(); it!=s.begin(); it--)
    {
        if(*it == del)
            break;

        res.insert(0,1,*it);
    }

    return res;
}
