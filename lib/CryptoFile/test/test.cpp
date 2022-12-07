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

    CryptoFile cf = CryptoFile("prova.txt","ciaociaociaociao",false,s);
    cout << "Success: " << s << endl << endl;

    s = cf.open();
    cout << "Open: " << s << endl <<endl;

    cout << "Add " << ":" << cf.addRecord("aaaaaaaaaaaaaaaaaaaaaaaaaa",l) << endl;
    cout << "Position: " << l << endl<<endl;

    for(int i=0;i<20;i++)
    {
        cout << "Add " << i << ":" << cf.addRecord("aaaaaaaaaaaaaaaaaaaaaaaaaa"+ to_string(i),l) << endl;
        cout << "Position: " << l << endl<<endl;
    }

    do
    {
        cout << "Read " << cf.getCurrentPosition() << ": " << cf.read(d) << endl;
        cout << "Data: " << d << endl << endl;

    } while (cf.next());


    cout << "Close: " << cf.close() << endl <<endl;

    return 0;
}