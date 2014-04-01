#ifndef DTCPSERVERSOCKET_HPP
#define DTCPSERVERSOCKET_HPP

#include "connection/ATCPServerSocket.hpp"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace Anakin {

class DTCPServerSocket : public ATCPServerSocket {
    public:
        DTCPServerSocket(unsigned short port, std::string lineDelimiter, std::string messageDelimiter);
        Socket* waitForConnection();
        std::string getLineDelimiter();
        std::string getMessageDelimiter();
    protected:
        std::string lineDelimiter;
        std::string messageDelimiter;
    private:
};
};

#endif // DTCPSERVERSOCKET_HPP
