#include "connection/DTCPServerSocket.hpp"
#include "connection/DelimiterBasedTCPSocket.hpp"
#include <boost/lexical_cast.hpp>

using namespace Anakin;

DTCPServerSocket::DTCPServerSocket( unsigned short port,
                                    std::string lineDelimiter,
                                    std::string messageDelimiter) : ATCPServerSocket(port)
{
    this->lineDelimiter = lineDelimiter;
    this->messageDelimiter = messageDelimiter;
}

Socket* DTCPServerSocket::waitForConnection()
{
    tcp::socket sock(io_service);
    if (showComs) std::cout << "listening at port " << this->port << std::endl;
    a.accept(sock);
    if (showComs) std::cout << "accepted a connection from " << boost::lexical_cast<std::string>(sock.remote_endpoint()) << std::endl;
    Socket* s = new DelimiterBasedTCPSocket(std::move(sock), this->lineDelimiter, this->messageDelimiter);
    return s;
}

std::string DTCPServerSocket::getLineDelimiter()
{
    return this->lineDelimiter;
}

std::string DTCPServerSocket::getMessageDelimiter()
{
    return this->messageDelimiter;
}
