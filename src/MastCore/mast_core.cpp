#include "../../lib/hasher/hasher.h"
#include "../../lib/CryptoFile/header/cryptofile.h"
#include <iostream>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <sys/wait.h>

using namespace std;

int main(){
    int fd[2];
    pipe(fd);
    if(fork()){
        dup2(fd[0],STDIN_FILENO);
        close(fd[1]);
    }else{
        dup2(fd[1],STDOUT_FILENO);
        close(fd[0]);
        execl("/opt/MAST/bin/password_asker", "password_asker", NULL);
        return 1;
    }

    string pwd;
    cin>>pwd;
    pwd += "\n";
    close(fd[0]);

    vector<int> pids;
    bool success = true;
    string hash_string;
    Hasher* hashGenerator = new Hasher();
    CryptoFile* cf = new CryptoFile("/opt/MAST/hash.txt", pwd, false, success);

    success = cf->open();
    if (!success) {
        cout<<"Can't access the file containing hashes!"<<endl;
        return 1;
    }

    int mfd[2];
    pipe(mfd);

    do {
        cout<<"["<<getpid()<<"] Entro nel do-while"<<endl;
        string delimiter = ":";
        string line;
        success = cf->read(line);
        if (!success)
            return 1;

        int finder = line.find(delimiter);
        string filename = line.substr(0,finder);
        string hash = line.substr(finder+1,line.length());
        string filepath = "/opt/MAST/modules/"+filename;
        if(hashGenerator->verifyIntegrity(filepath,hash,success) == true){
            int pid = fork();
            if(pid>0){
                pids.push_back(pid);
                write(mfd[1], pwd.c_str(), pwd.length()+1);
            }else{
                dup2(mfd[0], STDIN_FILENO);
                close(mfd[1]);
                execl("/bin/bash", "bash", filepath.c_str(),NULL);
                return 1;
            }
        }else{
            string command = "/opt/MAST/bin/alert_user MAST \"Corrupted module " + filename + "!\"";
            system(command.c_str());
        }
    } while (cf->next());

    for(int pid:pids)
        waitpid(pid,NULL,0);

    close(mfd[0]);
    close(mfd[1]);
    return 0;
}
