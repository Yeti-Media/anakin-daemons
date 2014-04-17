#ifndef DTCPSERVERSOCKET_HPP
#define DTCPSERVERSOCKET_HPP

#include "connection/ATCPServerSocket.hpp"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace Anakin
{

/**
* A Server socket for DelimiterBasedTCPSocket connection
*/
class DTCPServerSocket : public ATCPServerSocket
{
public:
    /**
    * Constructor
    * port : the port in which to listen to connections
    * lineDelimiter : the line delimiter for the DelimiterBasedTCPSocket
    * messageDelimiter : the message delimiter for the DelimiterBasedTCPSocket
    */
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
