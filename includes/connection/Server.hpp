#ifndef SERVER_HPP
#define SERVER_HPP
#include "connection/ServerSocket.hpp"
#include "processing/AnakinFlags.hpp"
#include "output/DataOutput.hpp"
#include "processing/SFBMCache.hpp"
#include "db/DBDriver.hpp"
#include "connection/HTTPSocket.hpp"

namespace Anakin {

class Server {
    public:
        Server(unsigned const short port, bool verbose, char mode=TCP, std::string ld="", std::string md="");
        void start(AnakinFlags* aflags, DataOutput* output);
        static const char CONSOLE = 1;
        static const char TCP = 2;
        static const char UDP = 4;
        static const char DTCP = 8;
        static const char HTTP = 16;
        HTTPSocket* getHttpSocket();
    protected:
        std::string read();
        std::vector<std::vector<std::string>*>* getInputs(std::string rawInput, bool * stopReceivedInsideInput);
        std::vector<std::string>* rawToInput(std::string rawInput);
        virtual void execute(std::vector<std::string>* input);
        virtual void executeStop();
        virtual bool stopMessageReceived(std::string rawMsg);
        virtual void startServer();
        virtual void endServer();
        void stopReceived();
        unsigned short port;
        ServerSocket* server;
        bool verbose;
        Socket* socket;
        HTTPSocket* httpSocket;
        char mode;
        AnakinFlags* aflags;
        DataOutput* output;
        SFBMCache* cache;
        DBDriver* dbdriver;
    private:
};

};

#endif // SERVER_HPP
