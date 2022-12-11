#include "../header/cryptofile.h"

#include <iostream>
using std::cout;
using std::endl;

CryptoFile::CryptoFile(string fname, bool &success)
{
    this->crypto = Crypto();
    this->filePath = fname;
    this->nRecords = 0;

    if(access(fname.c_str(),F_OK) != -1)
    {
        success = false;
        return;
    }
    else
        this->created = false;

    bool e = this->create();

    if(!e)
        success = false;
    else
        success = true;

}

CryptoFile::CryptoFile(string fname,string key,bool keyHexEncoded,bool &success)
{
    this->crypto = Crypto(key,keyHexEncoded);
    this->filePath = fname;
    this->nRecords = 0;

    bool e;

    //Verifying if the file already exists
    if(access(fname.c_str(),F_OK) != -1)
    {
        this->created = true;

        //Opening a stream for reading the file
        this->file.open(this->filePath,fstream::out | fstream::in);

        if(!this->file)
        {
            success = false;
            return;
        }

        //Retreiving number of stored records
        e = readRecordsNumberFromFile(this->nRecords);
        if(!e)
        {
            success = false;
            return;
        }

        //Closing the stream
        this->file.close();

    }
    else
    {
        this->created = false;
        this->nRecords = 0;

        //Creating the file
        e = create();

        if(!e)
        {
            success = false;
            return;
        }
    }

    success = true;

}

CryptoFile::CryptoFile(string fname,CryptoFileMap keys,bool &success)
{
    bool e;
    string key,iv;
    this->filePath = fname;
    this->nRecords = 0;

    //Verifying if the file already exists
    if(access(fname.c_str(),F_OK) != -1)
    {
        this->created = true;

        //Getting the key from CryptoFileMap
        key = keys.read(this->filePath+"_key",e);

        if(!e)
        {
            success = false;
            return;
        }

        //Initializing the cryptographyc engine
        crypto = Crypto(key,true);

        //Opening a stream for reading the file
        this->file.open(this->filePath,fstream::out | fstream::in);

        if(!this->file)
        {
            success = false;
            return;
        }

        //Retreiving number of stored records
        e = readRecordsNumberFromFile(this->nRecords);
        if(!e)
        {
            success = false;
            return;
        }

        //Closing the stream
        this->file.close();

    }
    else
    {
        this->created = false;
        
        //Searching an existing key in the keys file
        string key = keys.read(this->filePath+"_key",e);

        if(e)
        {
            /*
                A key already exists. 
                No need to generate a new key
            */
            
            //Initializing the cryptographyc engine
            crypto = Crypto(key,true);
        }
        else
        {
            /*
                The key does not exist.
                A new one must be generated
            */
            //Generating a new key
            this->crypto = Crypto();
            
            //Saving the generated key in CryptoFileMap
            e = keys.write(this->filePath+"_key",crypto.getKey());

            if(!e)
            {
                success = false;
                return;
            }
        }

        //Creating a blank file
        e = this->create();

        if(!e)
        {
            success = false;
            return;
        }


    }

    success = true;

}

bool CryptoFile::create()
{
    bool e;
    string hash;
    string endata,iv;


    if(this->created)
        return false;

    try
    {
        //Creating the file by opening a stream
        ofstream f = ofstream(this->filePath);
        
        if(!f.is_open())
            return false;

        /*
            Generating and writing hash
        */
        //Generating file hash
        hash = generateZeroHash();

        //Generating the IV
        iv = crypto.generateIV();

        //Encrypting the hash
        e = crypto.encrypt(hash,iv,endata);

        if(!e)
            return false;
        
        //Writing reserved record 0
        f << endata << this->recContentSeparator << iv << this->recSeparator;

        /*
            Writing reserved record n+1
        */
        //Generating the IV
        iv = crypto.generateIV();

        //Encrypting the number
        e = crypto.encrypt(to_string(this->nRecords),iv,endata);

        if(!e)
            return false;

        //Writing the record n+1 on the file
        f << endata << this->recContentSeparator << iv;


        //Closing the stream
        f.close();

        this->created = true;
        return true;
    }
    catch(const std::exception& e)
    {
        return false;
    }
}

bool CryptoFile::addRecord(string data,int &loc)
{
    bool e;
    streampos read_pos;
    streampos write_pos;
    string endata,iv;

    if(!file.is_open())
        return false;

    //Saving current pointer state
    read_pos = this->file.tellg();
    write_pos = this->file.tellp();
    
    try
    {
        //Generating the IV for the record
        iv = crypto.generateIV();

        //Encrypting data to write
        e = crypto.encrypt(data,iv,endata);

        if(!e)
            return false;

        //Positioning the stream pointer correctly before writing
        this->file.seekg(-1,this->file.end);
        this->file.seekp(-1,this->file.end);
        this->setPointerPositionBackwards(this->recSeparator);
        
        //Setting pointer position after delimiter for writing data
        this->file.seekg(1,this->file.cur);
        this->file.seekp(0,this->file.cur);

        //Writing data
        this->file << endata + this->recContentSeparator << iv << this->recSeparator;

        /*
            Updating number of records
        */
        this->nRecords++;
        loc = this->nRecords - 1;

        /* 
            Writing to the file the new number of records
        */
        //Generating the IV
        iv = crypto.generateIV();

        //Encrypting the number
        e = crypto.encrypt(to_string(this->nRecords),iv,endata);

        if(!e)
            return false;

        //Writing the record n+1 on the file
        file << endata << this->recContentSeparator << iv;

        /*
            Updating file hash
        */
        string hash;
        e = generateFileHash(hash);

        if(!e)
            return false;

        /*
            Writing generated hash on file
        */
        this->writeHashToFile(hash);


        /*
            Restoring previous pointer state
        */
        file.seekg(read_pos);
        file.seekp(write_pos);

        return true;
    }
    catch(const std::exception& e)
    {
        //Restoring previous pointer state
        file.seekg(read_pos);
        file.seekp(write_pos);

        return false;
    }
}

bool CryptoFile::open()
{
    bool e,res;

    if(this->file.is_open() || !this->created)
        return false;

    //Opening the stream
    this->file.open(this->filePath,fstream::out | fstream::in);

    if(!this->file)
        return false;

    //Setting the position to the first record
    this->file.seekg(0,this->file.beg);
    this->file.seekp(0,this->file.beg);
    this->setPointerPositionForward(this->recSeparator);

    this->pointerPosition = 0;
    if(this->nRecords > 0)
        e = this->verifyIntegrity(res);
    else
    {
        /*
            The file is a blank file
            Nothing to verify
        */
        e = true;
        res = true;
    }
    
    return e && res;
}

bool CryptoFile::close()
{
    if(!this->file.is_open())
        return false;

    /*
        Closing the stream
    */
    this->file.close();

    return true;
}

bool CryptoFile::read(string &data)
{
    if (this->nRecords == 0)
        return false;

    string rec,iv,en,de;
    bool e;

    if(!this->file.is_open())
        return false;

    //Getting the record and repositioning the pointer
    this->setPointerPositionForward(this->recSeparator,rec);
    this->setPointerPositionBackwards(this->recSeparator);


    //Getting encrypted data
    en = this->getStringFromBeg(rec,this->recContentSeparator);

    //Getting the IV
    iv = this->getStringFromEnd(rec,this->recContentSeparator);

    //Decrypting data
    e = crypto.decrypt(en,iv,de);

    if(!e)
        return false;

    data = de;

    return true;  
}

bool CryptoFile::next()
{
    if(!file.is_open())
        return false;

    this->pointerPosition++;

    if(this->pointerPosition < this->nRecords)
    {
        this->setPointerPositionForward(this->recSeparator);
        return true;
    }
    else
    {
        this->pointerPosition--;
        return false;
    }

}

bool CryptoFile::previous()
{
    if(!file.is_open())
        return false;

    this->pointerPosition--;

    if(this->pointerPosition >= 0)
    {
        this->setPointerPositionBackwards(this->recSeparator);
        return true;
    }
    else
    {
        this->pointerPosition++;
        return false;
    }
}

bool CryptoFile::verifyIntegrity(bool &res)
{
    string hash,hash2;
    streampos read_pos,write_pos;
    bool e,opened;
    res = false;

    //Creating or using a stream for writing data
    if(!this->file.is_open())
    {
        this->file.open(this->filePath,fstream::in | fstream::out);
        opened = true;
    }
    else
    {
        //Saving pointer state
        read_pos = file.tellg();
        write_pos = file.tellp();
    }

    //Getting the hash
    e = readHashFromFile(hash);

    if(!e)
        return false;

    //Generating file hash
    e = this->generateFileHash(hash2);

    if(!e)
        return false;
    
    //Saving hash validation result
    res = (hash == hash2);

    //Closing the stream or restoring previous pointer state
    if(opened)
        file.close();
    else
    {
        //Restoring previous pointer state
        file.seekg(read_pos);
        file.seekp(write_pos);
    }

    return true;
}

bool CryptoFile::generateFileHash(string &hash)
{
    string np1rec;
    streampos read_pos,write_pos;

    //Saving pointer state
    read_pos = file.tellg();
    write_pos = file.tellp();

    //Positioning the file pointers for reading the reserved record n+1
    this->file.seekg(-1,this->file.end);
    this->file.seekp(-1,this->file.end);

    //Getting the record n+1
    this->setPointerPositionBackwards(this->recSeparator,np1rec);

    //Positioning the file pointers to the beginning of the file
    this->file.seekg(0,this->file.beg);
    this->file.seekp(0,this->file.beg);

    //Skipping the reserved record 0
    this->setPointerPositionForward(this->recSeparator);

    //Initializing hash output parameter
    string ihash;
    BLAKE2b ha;
    for(int i=0;i<this->nRecords;i++)
    {
        string rec;

        //Getting the record
        this->setPointerPositionForward(this->recSeparator,rec);

        if(i == this->nRecords -1)
        {
            /*
                The last iteration must add the previous hash and the number of records
                First iteration is only the hash of the record (in this case the hash parameter is empty)
            */
            rec += ihash + np1rec;
        }
        else
        {
            /*
                Every other iteration must add the previous calculate dhash
                The first iteration uses only the red record (in this case the hash parameter is empty)

            */
            rec += ihash;
        }

        /*
            Generating the hash of each iteration:

            hash_rec_(i) = rec_(i) + hash_rec_(i-1)
        */
        ihash = string();
        StringSource(rec, true, new HashFilter(ha, 
                                    new HexEncoder(
                                        new StringSink(ihash)
                                        )
                                )
                    );

    }

    hash = ihash;

    //Restoring previous pointer state
    file.seekg(read_pos);
    file.seekp(write_pos);
    
    return true;
}

string CryptoFile::generateZeroHash()
{
    string digest;

    //Generating hash
    BLAKE2b hash;
    StringSource("0", true, new HashFilter(hash,
                                            new HexEncoder(
                                                new StringSink(digest)
                                                )
                                            )
                );

    return digest;
}

int CryptoFile::getCurrentPosition()
{
    return this->pointerPosition;
}

int CryptoFile::getRecordsNumber()
{
    return this->nRecords;
}

bool CryptoFile::readRecordsNumberFromFile(int &n)
{
    bool e;
    streampos read_pos;
    streampos write_pos;
    string rec,iv,endata,ns;

    try
    {
        //Saving pointer state
        read_pos = file.tellg();
        write_pos = file.tellp();

        //Setting the pointer for reading the reserved record
        this->file.seekg(-1,this->file.end);
        this->file.seekp(-1,this->file.end);

        //Reading the record
        this->setPointerPositionBackwards(this->recSeparator,rec);

        //Getting encrypted data
        endata = this->getStringFromBeg(rec,this->recContentSeparator);

        //Getting the IV
        iv = this->getStringFromEnd(rec,this->recContentSeparator);

        //Decrypting the obtained data
        e = crypto.decrypt(endata,iv,ns);

        if(!e)
            return false;

        //Getting the obtained number as an integer
        n = atoi(ns.c_str()); 

        //Restoring previous pointer state
        file.seekg(read_pos);
        file.seekp(write_pos);

        return true;
    }
    catch(const std::exception& e)
    {
        return false;
    }
}

bool CryptoFile::writeHashToFile(string hash)
{
    string enhash,iv,record;
    bool e;

    //Saving pointer state
    streampos write_pos = file.tellp();

    //Generating the IV
    iv = crypto.generateIV();

    //Encrypting the hash
    e = crypto.encrypt(hash,iv,enhash);

    if(!e)
        return false;

    //Constructing the final record
    record = enhash + this->recContentSeparator + iv + this->recSeparator;

    //Positioning write pointer for updating record 0 data
    file.seekp(0,ios::beg);

    //Updating the record
    file.write(record.c_str(),record.size());

    //Restoring previous pointer state
    file.seekp(write_pos);

    return true;
}

bool CryptoFile::readHashFromFile(string &hash)
{
    string enhash,iv;
    bool e;
    hash = string();

    //Saving pointer state
    streampos read_pos = file.tellg();

    //Positioning the pointer for reading the record
    file.seekg(0,ios::beg);

    //Getting the encrypted hash
    this->setPointerPositionForward(this->recContentSeparator,enhash);

    //Getting the IV
    this->setPointerPositionForward(this->recSeparator,iv);
    iv.erase(0,1); //Removing 'recContentSeparator' character

    //Decrypting saved hash
    e = crypto.decrypt(enhash,iv,hash);

    if(!e)
        return false;

    //Restoring previous pointer state
    file.seekg(read_pos);

    return true;
}

bool CryptoFile::destroy()
{
    if(file.is_open())
        return false;
    
    //Deleting the file
    int e = remove(this->filePath.c_str());

    if(e == 0)
    {
        created = false;
        return true;
    }
    else
        return false;
}

string CryptoFile::getEncryptionKey()
{
    return crypto.getKey();
}

void CryptoFile::setPointerPositionBackwards(char del)
{
    char x;

    /*
        Setting initial search position
    */
   this->file.read(&x,1);
   this->file.seekg(-1,this->file.cur);

   if(x == del)
   {
        /*
            The pointer is located at a delimiter,
             so the search must start one character back
        */

       this->file.seekg(-1,this->file.cur);
   }


    /*
        Moving the pointer
    */
    do
    {
        this->file.read(&x,1);//This automatically increments the pointer
        this->file.seekg(-2,this->file.cur);//Adjusting the pointer for reading the next character
    }
    while(x != del);

    //Setting the pointers to the delimiter
    this->file.seekg(1,this->file.cur);
    this->file.seekp(0,this->file.cur);
}

void CryptoFile::setPointerPositionBackwards(char del,string &s)
{
    char x;
    s = string();

    /*
        Setting initial search position
    */
   this->file.read(&x,1);
   this->file.seekg(-1,this->file.cur);

   if(x == del)
   {
        /*
            The pointer is located at a delimiter,
             so the search must start one character back
        */
       this->file.seekg(-1,this->file.cur);
   }

    /*
        Moving the pointer
    */
    do
    {
        this->file.read(&x,1);//This automatically increments the pointer
        s.insert(0,1,x);

        this->file.seekg(-2,this->file.cur);//Adjusting the pointer for the new character
    }
    while(x != del);

    //Revoving the separator character
    s.erase(0,1);

    //Setting the pointers to the delimiter
    this->file.seekg(1,this->file.cur);
    this->file.seekp(0,this->file.cur);
}

void CryptoFile::setPointerPositionForward(char del)
{
    char x;

    /*
        Setting initial search position
    */
   this->file.read(&x,1);
   this->file.seekg(-1,this->file.cur);

   if(x == del)
   {
        /*
            The pointer is located at a delimiter,
             so the search must start one character forward
        */
       this->file.seekg(1,this->file.cur);
   }

    do
    {
        this->file.read(&x,1);//This automatically increments the pointer
    }
    while(x != del);

    //Setting the pointers to the delimiter
    this->file.seekg(-1,this->file.cur);
    this->file.seekp(0,this->file.cur);

}

void CryptoFile::setPointerPositionForward(char del,string &s)
{
    char x;
    s = string();

    /*
        Setting initial search position
    */
   this->file.read(&x,1);
   this->file.seekg(-1,this->file.cur);

   if(x == del)
   {
        /*
            The pointer is located at a delimiter,
             so the search must start one character forward
        */
       this->file.seekg(1,this->file.cur);
   }

    do
    {
        this->file.read(&x,1);//This automatically increments the pointer
        s.push_back(x);
    }
    while(x != del);

    //Deleting the character delimiter from the resulting string
    s.pop_back();

    //Setting the pointers to the delimiter
    this->file.seekg(-1,this->file.cur);
    this->file.seekp(0,this->file.cur);
}

string CryptoFile::getFilePath()
{
    return this->filePath;
}

string CryptoFile::getStringFromBeg(string s, char del)
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

string CryptoFile::getStringFromEnd(string s, char del)
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
