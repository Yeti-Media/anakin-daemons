#ifndef DATAOUTPUT_HPP
#define DATAOUTPUT_HPP

#include <Socket.hpp>
#include <iostream>

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
        Socket* s;
        bool consoleOutput=true;
};
};

#endif // DATAOUTPUT_HPP
