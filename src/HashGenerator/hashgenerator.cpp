#include "../../lib/hasher/hasher.h"
#include "../../lib/CryptoFile/header/cryptofile.h"
#include <iostream>

#define HASHFILE "/opt/MAST/hashes.txt"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filetype> <filepath>\n", argv[0]);
        return 1;
    }

    string pwd;
    cin>>pwd;

    bool success;
    CryptoFile* cf = new CryptoFile(HASHFILE, pwd, false, success);
    if (!success) {
        cout<<"Errore nell'apertura del file degli hash!"<<endl;
        return 1;
    }
    cf->open();

    Hasher* hasher = new Hasher();
    string hash = hasher->generateFromFile(argv[2], success);
    int loc;
    string line = string(argv[1]) + ":" + string(argv[2]) + ":" + hash;
    success = cf->addRecord(line, loc);
    if (!success) {
        cout<<"Errore nella scrittura!"<<endl;
        return 1;
    }
}
