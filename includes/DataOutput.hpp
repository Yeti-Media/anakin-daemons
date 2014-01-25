#ifndef DATAOUTPUT_HPP
#define DATAOUTPUT_HPP

#include <Socket.hpp>
#include <iostream>
#include <semaphore.h>

namespace Anakin {

class DataOutput {
    public:
        DataOutput(Socket* s);
        DataOutput();
        void output(std::string data);
        void output(std::wstring data);
        void close();
    protected:
    private:
        void initSem();
        Socket* s;
        bool consoleOutput=true;
        sem_t ssem;
        sem_t wssem;
};
};

#endif // DATAOUTPUT_HPP
