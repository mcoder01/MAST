#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <string>
using std::string;

#include "../header/cryptofile.h"

int main(int argc, char* argv[])
{
    bool s;
    int l;
    string d;

    CryptoFileMap* cfm = new CryptoFileMap("k.txt","ciaociaociaociao",s);
    cout <<"SKSK: " << s << endl;

    cout << "W:" << cfm->write("LPC","Luca") << endl;

    delete cfm;

    CryptoFileMap* cfm2 = new CryptoFileMap("k.txt","ciaociaociaociao",s);
    cout <<"SKSK: " << s << endl;

    string str;
    str = cfm2->read("LPC",s);

    cout <<"Res: " << s << "|" << str << endl;



    return 0;
}