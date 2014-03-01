#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include <Socket.hpp>

namespace Anakin {

class ServerSocket {
    public:
        void setShowComs(bool v) {
            showComs = v;
        }
        bool isConnected() {
            return connected;
        }
        virtual Socket* waitForConnection() = 0;
        virtual void listen() {}
        virtual void stopServer() = 0;
    protected:
        bool showComs=false;
        bool connected=false;
        unsigned short port;
    private:
};

};

#endif // SERVERSOCKET_HPP
