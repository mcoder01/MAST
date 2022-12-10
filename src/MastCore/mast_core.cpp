#include "../../lib/hasher/hasher.h"
#include "../../lib/CryptoFile/header/cryptofile.h"
#include <iostream>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <sstream>
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
    Hasher* hasher = new Hasher();

    CryptoFile* cf = new CryptoFile("/opt/MAST/hashes.txt", pwd, false, success);
    success = cf->open();
    if (!success) {
        cout<<"Can't access the file containing the hashes!"<<endl;
        return 1;
    }

    int mfd[2];
    pipe(mfd);

    do {
        string line;
        success = cf->read(line);
        if (!success)
            return 1;

        stringstream ss(line);
        string filetype, filepath, hash;
        getline(ss, filetype, ':');
        getline(ss, filepath, ':');
        getline(ss, hash, ':');

        if(hasher->verifyIntegrity(filepath, hash, success)) {
            if (filetype == "module") {
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
            }
        }else{
            string command = "/opt/MAST/bin/alert_user MAST \"Corrupted file " + filepath + "!\"";
            system(command.c_str());
        }
    } while (cf->next());

    for(int pid:pids)
        waitpid(pid,NULL,0);

    close(mfd[0]);
    close(mfd[1]);
    return 0;
}
