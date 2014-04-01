#ifndef AUDPSERVERSOCKET_HPP
#define AUDPSERVERSOCKET_HPP

#include "connection/ServerSocket.hpp"
#include <boost/asio.hpp>

using boost::asio::ip::udp;

namespace Anakin {

class AUDPServerSocket : public ServerSocket{
    public:
        AUDPServerSocket(unsigned short port);
        Socket* waitForConnection();
        void stopServer();
    protected:
    private:
};
};

#endif // AUDPSERVERSOCKET_HPP
