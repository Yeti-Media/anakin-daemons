#ifndef SERVER_HPP
#define SERVER_HPP
#include <ServerSocket.hpp>
#include <Flags.hpp>
#include <DataOutput.hpp>

namespace Anakin {

class Server {
    public:
        Server(unsigned const short port, bool verbose=false, char mode=TCP);
        void start(Flags* flags, DataOutput* output);
        static const char CONSOLE = 1;
        static const char TCP = 2;
        static const char UDP = 4;
    protected:
    private:
        std::string read();
        unsigned short port;
        ServerSocket* server;
        bool verbose;
        Socket* socket;
        char mode;
};

};

#endif // SERVER_HPP
