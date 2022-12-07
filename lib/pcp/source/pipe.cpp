#include "../header/pipe.h"

//#define DEBUG_TRYES_COUNTER 172

PipeError Pipe::initialize(bool create)
{
    this->created = create;

    if(create)
    {
        this->pipePath = "/tmp/" + to_string(getpid()) + '_' + to_string(this->process);

        int s = mkfifo(pipePath.c_str(),O_CREAT | O_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

        if(s != 0)
            return PipeError::CREATION_ERROR;
        else
            return PipeError::OK;
    }
    else
    {
        this->pipePath = "/tmp/" + to_string(this->process) + '_' + to_string((getpid()));

        if(access(pipePath.c_str(),F_OK) == 0)
            return PipeError::OK;
        else
            return PipeError::CREATION_ERROR;
    }

}

PipeError Pipe::send(string data)
{
    int s = 1;
    
    int fd = open(pipePath.c_str(), O_WRONLY);

    if(fd < 0)
        return PipeError::WRITE_ERROR;

    for(string::iterator it = data.begin(); it != data.end(); ++it) 
    {
        char buf = *it;
        s = write(fd,&buf,sizeof(buf));

        if(s < 0)
            return PipeError::WRITE_ERROR;
    }

    s = close(fd);

    if(s != 0)
        return PipeError::WRITE_ERROR;
    else
        return PipeError::OK;
}

string Pipe::receive(PipeError &error)
{
    string data;

    int fd = open(pipePath.c_str(), O_RDONLY);

    if(fd < 0)
        error = PipeError::READ_ERROR;

    int n;
    char buf;
    while( (n = read(fd,&buf,sizeof(buf))) > 0)
    {
        data.push_back(buf);
    }

    if(n < 0)
        error = PipeError::READ_ERROR;
    else
        error = PipeError::OK;

    int s = close(fd);

    if(s != 0)
        error = PipeError::READ_ERROR;
    else
        error = PipeError::OK;

    return data;
}

PipeError Pipe::destroy()
{
    if(created)
    {
        int e = unlink(pipePath.c_str());

        if(e < 0)
            return PipeError::CLOSING_ERROR;
        else
            return PipeError::OK;
    }

    return PipeError::OK;

    /*se fallisce inspiegabilmente vuol dire che òa cosa dei proprietari è vera*/
}