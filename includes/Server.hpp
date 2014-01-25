#ifndef SERVER_HPP
#define SERVER_HPP
#include <ServerSocket.hpp>
#include <AnakinFlags.hpp>
#include <DataOutput.hpp>

namespace Anakin {

class Server {
    public:
        Server(unsigned const short port, bool verbose, char mode=TCP, std::string ld="", std::string md="");
        void start(AnakinFlags* aflags, DataOutput* output);
        static const char CONSOLE = 1;
        static const char TCP = 2;
        static const char UDP = 4;
        static const char DTCP = 8;
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
        char mode;
        AnakinFlags* aflags;
        DataOutput* output;
    private:
};

};

#endif // SERVER_HPP
