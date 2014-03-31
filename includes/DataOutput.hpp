#ifndef DATAOUTPUT_HPP
#define DATAOUTPUT_HPP

#include <Socket.hpp>
#include <iostream>
#include <semaphore.h>
#include "HTTPSocket.hpp"

namespace Anakin {

class DataOutput {
    public:
        DataOutput(Socket* s);
        DataOutput(HTTPSocket* httpSocket);
        DataOutput();
        void output(std::string data, int reqID=0);
        void output(std::wstring data, int reqID=0);
        void close();
    protected:
    private:
        void initSem();
        Socket* s;
        HTTPSocket* httpSocket;
        bool consoleOutput=true;
        bool httpOutput=false;
        sem_t ssem;
        sem_t wssem;
};
};

#endif // DATAOUTPUT_HPP
