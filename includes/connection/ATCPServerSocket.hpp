#ifndef ATCPSERVERSOCKET_HPP
#define ATCPSERVERSOCKET_HPP

#include "connection/ServerSocket.hpp"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace Anakin {

/**
* An implementation of ServerSocket for ATCPSocket
*/
class ATCPServerSocket : public ServerSocket {
    public:
        /**
        * Constructor
        * port : the port in which to listen
        */
        ATCPServerSocket(unsigned short port);
        /**
        * uses internal tcp accept function
        */
        Socket* waitForConnection();
        void stopServer();
        boost::asio::io_service io_service;
        tcp::acceptor a;
    protected:
    private:
};

};

#endif // ATCPSERVERSOCKET_HPP
