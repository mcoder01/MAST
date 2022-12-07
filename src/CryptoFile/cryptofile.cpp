#include "../../lib/CryptoFile/header/cryptofile.h"
#include "../../lib/CryptoFile/header/cryptofilemap.h"

#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

string checkFlag(string,int);
int main(int argc,char *argv[]){
    if(argc==1){
        fprintf(stderr, "Usage: cryptofile -w or cryptofile -r\n");
        return 1;
    }

    string pwd;
    cin>>pwd;
    
    string flag = argv[1]; //flag -r o -w
    string file_name = argv[2]; //nome file
    bool success;

    CryptoFileMap* cfm = new CryptoFileMap("/var/MAST/keys.txt",pwd,success);
    if (!success) {
        fprintf(stderr, "Password non valida!");
        return 1;
    }

    CryptoFile* cf = new CryptoFile(file_name,*cfm,success);
    if(checkFlag(flag,argc)=="-r"){
        //Fase di lettura
        success = cf->open();
        do {
            string buffer;
            cf->read(buffer);
            cout<<buffer<<endl;
        }while(cf->next());
        cf->close();
    }else if(checkFlag(flag,argc)=="-w"){
        //Fase di scrittura
        string data_input = argv[3]; //Data
        int location;
        success = cf->open();
        success = cf->addRecord(data_input,location);
        cf->close();
    }
    
    return 0;
}

string checkFlag(string flag, int n_arguments){
    if(flag == "-w"){
        //SCRITTURA
        if(n_arguments != 4){
            fprintf(stderr, "Usage: cryptofile -w <filename> <data>");
            return "";
        }

        return flag;
    }

    if(flag == "-r"){
        //LETTURA
        if(n_arguments != 3){
            fprintf(stderr, "Usage: cryptofile -r <filename>");
            return "";
        }

        return flag;
    }

    cout<<"Usage: cryptofile -w or cryptofile -r"<<endl;
    return "";
}
