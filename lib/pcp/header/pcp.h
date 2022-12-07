#ifndef PCP_H
#define PCP_H

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <thread>
using std::thread;

#include <functional>
using std::function;

#include "crypto.h"
#include "pipe.h"

/// @brief Class used for indicating the public state of the protocol
enum class EventResult
{
    /// @brief The protocol can continue properly
    OK,
    /// @brief The protocol must stop without failing
    IGNORE,
    /// @brief The protocol must stop and fail
    FAIL
};

/// @brief Class used for indicating the public state of the protocol
enum class PCPPublicState
{
    /// @brief Connected and functioning properly
    CONNECTED,
    /// @brief No connection enstabilished
    NOT_CONNECTED,
    /// @brief Failed because of a cryptographic error
    CRYPTO_ERROR,
    /// @brief Failed because of a pipe error
    PIPE_FAIL,
    /// @brief Failed because of bad connection
    CONNECTION_FAIL
};

/// @brief Class used for indicating the internal state of the protocol
enum class PCPInternalState
{
    /// @brief Waiting for a connection
    LISTENING,
    /// @brief Doing nothing
    IDLEING,
    /// @brief Performing an handshake operation
    HANDSHAKING,
    /// @brief Performing a send operation
    SENDING,
    /// @brief Performing a receive operation
    RECEIVING,
    /// @brief Stopped because of an internal error
    FAILED
};

namespace PCP
{
        /// @brief Time to wait for a process to reply back
        const int maxWaitSeconds = 5;
      
        /// @brief Class used for performing cryptographyc operations
        extern Crypto crypto;

        /// @brief Class used for performing pipe data transfers
        extern Pipe pipe;

        /// @brief Public state of the protocol
        extern PCPPublicState publicState;

        /// @brief Internal state of the protocol
        extern PCPInternalState internalState;

        /// @brief Function that will be executed after data will be received
        extern function<void(string)> dataReceivedHandler;

        /// @brief Function that will be executed before data will be received
        extern function<EventResult(pid_t)> dataReceivingHandler;

        /**
         * @brief Handles PCP connection requests
         * 
         */
        void connectionHandler(int signo,siginfo_t *info, void *context);

        /**
         * @brief Handles PCP receive requests
         * 
         */
        void receiveHandler(int signo,siginfo_t *info, void *context);

        /**
         * @brief Construct a new PCP object
         * 
         * @param allowed Process to which allowed to communicate
         * @param onDataReceived Function that will handle received data (void received(string dataReceived))
         */
        void initialize();

        /**
         * @brief Connects to a process
         * 
         * @param proc Process to which connect
         */
        void connect(pid_t proc);

        /**
         * @brief Sends data
         * 
         * @param data Data to send
         */
        void send(string data);

        /**
         * @brief Disconnects from the connected process
         * 
         */
        void disconnect();
};

#endif