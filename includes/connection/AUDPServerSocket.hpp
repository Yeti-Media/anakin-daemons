#ifndef AUDPSERVERSOCKET_HPP
#define AUDPSERVERSOCKET_HPP

#include "connection/ServerSocket.hpp"
#include <boost/asio.hpp>

using boost::asio::ip::udp;

namespace Anakin {

/**
* An implementation of ServerSocket for AUDPSocket
*/
class AUDPServerSocket : public ServerSocket{
    public:
        /**
        * Constructor
        * port : the port in which to listen
        */
        AUDPServerSocket(unsigned short port);
        /**
        * uses AUDPSocket wait for connection protocol
        */
        Socket* waitForConnection();
        void stopServer();
    protected:
    private:
};
};

#endif // AUDPSERVERSOCKET_HPP
