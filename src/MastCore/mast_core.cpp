#include "../../lib/hasher/hasher.h"
#include "../../lib/CryptoFile/header/cryptofile.h"
#include <iostream>
#include <string.h>
#include <vector>
#include <sys/wait.h>

using namespace std;

int main(){

    string pwd;
    int fd[2];
    pipe(fd);
    if(fork()){
        dup2(fd[0],STDIN_FILENO);
        close(fd[1]);
    }else{
        dup2(fd[1],STDOUT_FILENO);
        close(fd[0]);
        execl("/opt/MAST/bin/password_asker","password_asker",NULL);
    }
    cin>>pwd;
    cout<<pwd<<endl;

    vector<int> pids;
    bool success = true;
    string hash_string;
    Hasher* hashGenerator = new Hasher();
    CryptoFile* cf = new CryptoFile("/opt/MAST/hash.txt", pwd, false, success);

    success = cf->open();
    if (!success) {
        cout<<"Problema apertura CryptoFileMap"<<endl;
        return 1;
    }
    do {
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
            int* array_fds = (int*)calloc(2,sizeof(int));
            pipe(array_fds);
            int pid = fork();
            if(pid>0){
                pids.push_back(pid);
                dup2(array_fds[1],STDOUT_FILENO);
                close(array_fds[0]);
                cout<<pwd<<endl;
            }else{
                dup2(array_fds[0],STDIN_FILENO);
                close(array_fds[1]);
                execl(filepath.c_str(),filename.c_str(),NULL);
            }
        }else{
            execl("/opt/MAST/bin/alert_user","alert_user","MAST","Corrupted modules files",NULL);
        }
    } while (cf->next());
/*
    for(int pid:pids){
        waitpid(pid,NULL,0);
    }
 */
    return 0;
}
