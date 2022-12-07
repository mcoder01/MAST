#include "../../lib/hasher/hasher.h"
#include "../../lib/CryptoFile/header/cryptofile.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <password>\n", argv[0]);
        return 1;
    }

    bool success;
    CryptoFile* cf = new CryptoFile("/opt/MAST/hash.txt", argv[2], false, success);
    if (!success) {
        cout<<"Errore nell'apertura del file degli hash!"<<endl;
        return 1;
    }
    cf->open();
    Hasher* hasher = new Hasher();

    string filepath = string("/opt/MAST/modules/") + argv[1];
    string hash = hasher->generateFromFile(filepath, success);
    int loc;
    string line = string(argv[1]) + ":" + hash;
    cout<<line<<endl;
    success = cf->addRecord(line, loc);
    if (!success) {
        cout<<"Errore nella scrittura!"<<endl;
        return 1;
    }
}
