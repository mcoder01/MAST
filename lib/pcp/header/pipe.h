#ifndef PIPE_H
#define PIPE_H

#include <string>
using std::string;
using std::to_string;

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/// @brief Errors encountered using the pipe
enum class PipeError
{
    /// @brief No problems were found
    OK,
    /// @brief An error occurred on creating the pipe
    CREATION_ERROR,
    /// @brief An error occurred on closing the pipe
    CLOSING_ERROR,
    /// @brief An error occurred on writing
    WRITE_ERROR,
    /// @brief An error occurred on reading
    READ_ERROR
};

/// @brief Class responsible of communication via pipe
class Pipe
{
    private:

        /// @brief PID of the process to which connect
        pid_t process;

        /// @brief Path of the used pipe
        string pipePath;

        /// @brief Specifies if this was the process who created the pipe
        bool created;

    public:

        /**
         * @brief Construct a new Pipe object
         * 
         * @param process The process to which connect
         */
        Pipe(pid_t process)
        {
            this->process = process;
        }

        /// @brief Construct a new Pipe object (do NOT use this for initializing the pipe)
        Pipe()
        {
            this->process = process;
        }

        /**
         * @brief Opens the pipe connection
         * 
         * @param create Specifies if the pipe needs to be created. Default: true
         * @return PipeError Error code resulting after the operation 
         */
        PipeError initialize(bool create);

        /**
         * @brief Send data trought the pipe
         * 
         * @param data Data to send
         * @return PipeError Error code resulting after the operation 
         */
        PipeError send(string data);

        /**
         * @brief Receives data sent trought the pipe
         * 
         * @param error Error code resulting after the operation
         * @return string Data received
         */
        string receive(PipeError &error);

        /**
         * @brief Closes the pipe connection
         * 
         * @return PipeError Error code resulting after the operation
         */
        PipeError destroy();
};

#endif