#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <string>
using std::string;

#include "../header/pcp.h"

void fun(string data)
{
    cout<<"String received: "<<data<<endl;
    exit(0);
}

EventResult fun2(pid_t sender)
{
    return EventResult::OK;
}

static string t;

int main(int argc, char* argv[])
{
    cout<<getpid()<<endl;

    cin >> t;

    PCP::initialize();

    PCP::dataReceivingHandler = fun2;
    PCP::dataReceivedHandler = fun;

    if(argc == 2)
    {
        PCP::connect(atoi(t.c_str()));

        if(PCP::internalState == PCPInternalState::FAILED)
            cout << "Not connected" << endl;
        else
            cout << "Connected" << endl;

        cout << "Internal state: " << (int)PCP::internalState << endl;
        cout << "Public state: " << (int)PCP::publicState << endl;

        PCP::send("LPC");

        if(PCP::internalState == PCPInternalState::FAILED)
            cout << "Not sent" << endl;
        else
            cout << "Sent" << endl;

        cout << "Internal state: " << (int)PCP::internalState << endl;
        cout << "Public state: " << (int)PCP::publicState << endl;
    }
    else
    {  
        while(1) { }
    }
    

    return 0;
}
