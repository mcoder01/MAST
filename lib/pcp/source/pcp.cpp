#include "../header/pcp.h"

namespace PCP
{
    
    /// @brief Class used for performing cryptographyc operations
    Crypto crypto;

    /// @brief Class used for performing pipe data transfers
    Pipe pipe;

    /// @brief PID of the process to which the protocol is connected
    pid_t procConnected;

    /// @brief Public state of the protocol
    PCPPublicState publicState;

    /// @brief Internal state of the protocol
    PCPInternalState internalState;

    /// @brief Function that will be executed after data was received
    function<void(string)> dataReceivedHandler;

    /// @brief Function that will be executed before data will be received
    function<EventResult(pid_t)> dataReceivingHandler;

    EventResult voidReceiving(pid_t p) { return EventResult::OK;}
    void voidReceived(string d) {}

    void initialize()
    {
        //Cryptographic component initialization
        crypto = Crypto();

        //Initializing the protocol state
        publicState = PCPPublicState::NOT_CONNECTED;
        internalState = PCPInternalState::LISTENING;

        //Setting the event functions
        dataReceivingHandler = voidReceiving;
        dataReceivedHandler = voidReceived;

        //Setting connect signal handler (SIGUSR1)
        struct sigaction sigusr1_action;
        sigusr1_action.sa_sigaction = connectionHandler;
        sigemptyset(&sigusr1_action.sa_mask);
        sigusr1_action.sa_flags = SA_RESTART|SA_SIGINFO;
        sigaction(SIGUSR1,&sigusr1_action,NULL);

        //Setting receive signal handler (SIGUSR2)
        struct sigaction sigusr2_action;
        sigusr2_action.sa_sigaction = receiveHandler;
        sigemptyset(&sigusr2_action.sa_mask);
        sigusr2_action.sa_flags = SA_RESTART|SA_SIGINFO;
        sigaction(SIGUSR2,&sigusr2_action,NULL);
    }

    void connectionHandler(int signo,siginfo_t *info, void *context)
    {
        sigset_t waitset;
        siginfo_t infow;
        struct timespec waitTime;
        int error;
        PipeError e;
        sigval val;

        if(signo == SIGUSR1)
        {

            if(internalState == PCPInternalState::LISTENING)
            {
                internalState = PCPInternalState::HANDSHAKING;

                //First time opening of the pipe
                e = pipe.initialize(false);

                if(e != PipeError::OK)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::PIPE_FAIL;
                    return;
                }

                //Send a receive request for sending the public key
                error = sigqueue(info->si_pid,SIGUSR2,val);

                if(error != 0)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::CONNECTION_FAIL;
                    return;
                }

                //Sending the public key
                e = pipe.send(crypto.getPublicKey());

                if(e != PipeError::OK)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::PIPE_FAIL;
                    return;
                }

                //Preparing for receiving the receive signal
                sigemptyset(&waitset);
                sigaddset(&waitset, SIGUSR2);
                waitTime.tv_sec = maxWaitSeconds;
                waitTime.tv_nsec = 0;

                //Waiting the process to send a receive signal
                error = sigtimedwait(&waitset,&infow,&waitTime);

                if(error < 0)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::CONNECTION_FAIL;
                    return;
                }

                //Sending back the signal for signalling readiness
                error = sigqueue(info->si_pid,SIGUSR2,val);

                if(error != 0)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::CONNECTION_FAIL;
                    return;
                }

                //Receiving the public key
                string pkey = pipe.receive(e);

                if(e != PipeError::OK)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::PIPE_FAIL;
                    return;
                }

                //Memorizing the process ublic key
                crypto.setProcessPublicKey(pkey);

                //Sending the signal for the last time for completing the handshake
                error = sigqueue(info->si_pid,SIGUSR2,val);

                if(error != 0)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::CONNECTION_FAIL;
                    return;
                }

                //Handshake completed. Updating the protocol state
                internalState = PCPInternalState::IDLEING;
                publicState = PCPPublicState::CONNECTED;
            }
            else if(internalState != PCPInternalState::FAILED)
            {
                //Disconnecting from the pipe
                e = pipe.destroy();

                if(e != PipeError::OK)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::PIPE_FAIL;
                    return;
                }

                //Replying back
                error = sigqueue(info->si_pid,SIGUSR1,val);

                if(error != 0)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::CONNECTION_FAIL;
                    return;
                }

                //Disconnection completed. Updating protocol state
                internalState = PCPInternalState::LISTENING;
                publicState = PCPPublicState::NOT_CONNECTED;
            }    
        }
    }

    void receiveHandler(int signo,siginfo_t *info, void *context)
    {
        int error;
        PipeError e;
        CryptoError ce;
        sigval val;

        if(signo == SIGUSR2)
        {

            EventResult er = dataReceivingHandler(info->si_pid);

            if(er != EventResult::OK)
            {
                if(er == EventResult::IGNORE)
                    return;
                else
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::CONNECTION_FAIL;
                    return;
                }
            }

            if(internalState == PCPInternalState::IDLEING)
            {
                //Updating the protocol state
                internalState = PCPInternalState::RECEIVING;

                //Replying back
                error = sigqueue(info->si_pid,SIGUSR2,val);  

                if(error != 0)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::CONNECTION_FAIL;
                    return;
                }

                //Receiving data
                string data = pipe.receive(e);

                if(e != PipeError::OK)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::PIPE_FAIL;
                    return;
                }

                //Decrypting data
                data = crypto.decrypt(data,ce);

                if(ce != CryptoError::OK)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::CRYPTO_ERROR;
                    return;
                }

                //Confirming data was received
                error = sigqueue(info->si_pid,SIGUSR2,val);

                if(error != 0)
                {
                    internalState = PCPInternalState::FAILED;
                    publicState = PCPPublicState::CONNECTION_FAIL;
                    return;
                }

                //Send operation completed. Updating protocol state
                internalState = PCPInternalState::IDLEING;

                //Invocking handler for data reception
                dataReceivedHandler(data);
            }
        }
    }

    void connect(pid_t proc)
    {
        sigset_t waitset;
        siginfo_t info;
        struct timespec waitTime;
        int error;
        sigval val;

        //Updating the state
        internalState = PCPInternalState::HANDSHAKING;

        //Pipe handler initialization
        pipe = Pipe(proc);

        //Creating the pipe
        PipeError e = pipe.initialize(true);

        if(e != PipeError::OK)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::PIPE_FAIL;
            return;
        }

        //Sending a connection request
        error = sigqueue(proc,SIGUSR1,val);

        if(error != 0)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::CONNECTION_FAIL;
            return;
        }

        //Preparing for receiving back the receive signal
        sigemptyset(&waitset);
        sigaddset(&waitset, SIGUSR2);
        waitTime.tv_sec = maxWaitSeconds;
        waitTime.tv_nsec = 0;


        //Waiting the process to reply back with a receive signal
        error = sigtimedwait(&waitset,&info,&waitTime);

        if(error < 0)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::CONNECTION_FAIL;
            return;
        }

        //Receiving the process public key
        string pkey = pipe.receive(e);

        if(e != PipeError::OK)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::PIPE_FAIL;
            return;
        }

        //Setting the process public key
        crypto.setProcessPublicKey(pkey);

        //Sending a receive request (for sending the local public key)
        error = sigqueue(proc,SIGUSR2,val);

        if(error != 0)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::CONNECTION_FAIL;
            return;
        }

        //Waiting the process to reply back
        error = sigtimedwait(&waitset,&info,&waitTime);

        if(error < 0)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::CONNECTION_FAIL;
            return;
        }

        //Process is ready. Sending the local public key
        e = pipe.send(crypto.getPublicKey());

        if(e != PipeError::OK)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::PIPE_FAIL;
            return;
        }

        //Waiting for the last signal for confirming the handshake completion
        error = sigtimedwait(&waitset,&info,&waitTime);

        if(error < 0)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::CONNECTION_FAIL;
            return;
        }

        //Handshake completed. Updating the protocol state
        internalState = PCPInternalState::IDLEING;
        publicState = PCPPublicState::CONNECTED;
        procConnected = proc;
        
    }

    void disconnect()
    {
        sigset_t waitset;
        siginfo_t info;
        struct timespec waitTime;
        int error;
        PipeError e;

        //Sending a disconnection request
        error = kill(procConnected,SIGUSR1);

        if(error != 0)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::CONNECTION_FAIL;
            return;
        }

        //Preparing for receiving back the signal
        sigemptyset(&waitset);
        sigaddset(&waitset, SIGUSR1);
        waitTime.tv_sec = maxWaitSeconds;
        waitTime.tv_nsec = 0;

        //Waiting the process to reply back
        error = sigtimedwait(&waitset,&info,&waitTime);

        if(error < 0)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::CONNECTION_FAIL;
            return;
        }

        //Reply received. Proceding to disconnect
        e = pipe.destroy();

        if(e != PipeError::OK)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::PIPE_FAIL;
            return;
        }

        //Disconnection completed. Updating protocol state
        internalState = PCPInternalState::LISTENING;
        publicState = PCPPublicState::NOT_CONNECTED;
        procConnected = 0;
        
    }

    void send(string data)
    {
        sigset_t waitset;
        siginfo_t info;
        struct timespec waitTime;
        int error;
        PipeError e;
        CryptoError ce;
        sigval val;

        //Updating the protocol state
        internalState = PCPInternalState::SENDING;

        //Encrypting data
        data = crypto.encrypt(data,ce);

        if(ce != CryptoError::OK)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::CONNECTION_FAIL;
            return;
        }

        //Sending a receive request
        error = sigqueue(procConnected,SIGUSR2,val);

        if(error != 0)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::CONNECTION_FAIL;
            return;
        }

        //Preparing for receiving back the signal
        sigemptyset(&waitset);
        sigaddset(&waitset, SIGUSR2);
        waitTime.tv_sec = maxWaitSeconds;
        waitTime.tv_nsec = 0;


        //Waiting the process to reply back
        error = sigtimedwait(&waitset,&info,&waitTime);

        if(error < 0)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::CONNECTION_FAIL;
            return;
        }

        //Sending the data
        e = pipe.send(data);

        if(e != PipeError::OK)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::PIPE_FAIL;
            return;
        }

        //Waiting the process to confirm reception
        error = sigtimedwait(&waitset,&info,&waitTime);

        if(error < 0)
        {
            internalState = PCPInternalState::FAILED;
            publicState = PCPPublicState::CONNECTION_FAIL;
            return;
        }

        //Send operation completed. Updating protocol state
        internalState = PCPInternalState::IDLEING;
    }

}