#ifndef ATCPSERVERSOCKET_HPP
#define ATCPSERVERSOCKET_HPP

#include "connection/ServerSocket.hpp"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace Anakin {

class ATCPServerSocket : public ServerSocket {
    public:
        ATCPServerSocket(unsigned short port);
        Socket* waitForConnection();
        void stopServer();
        boost::asio::io_service io_service;
        tcp::acceptor a;
    protected:
    private:
};

};

#endif // ATCPSERVERSOCKET_HPP
