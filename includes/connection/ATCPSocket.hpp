#ifndef ATCPSOCKET_HPP
#define ATCPSOCKET_HPP

#include "connection/Socket.hpp"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace Anakin
{

/**
* A TCP implementation of Socket
* it doesn't have any connection/disconnection protocol
* and just uses the inner tcp socket to connect/disconnect
*/
class ATCPSocket : public Socket
{
public:
    /**
    * Constructor
    * ip : the ip to connect
    * port : port to use
    * sendType : synchonized or default send (see Socket)
    */
    ATCPSocket(std::string ip, std::string port, char sendType=DEFAULTSEND);
    /**
    * Constructor (used by ATCPServerSocket)
    * sock : internal socket created by the server when a connection is made
    * sendType : synchonized or default send (see Socket)
    */
    ATCPSocket(tcp::socket sock, char sendType=DEFAULTSEND);
    boost::asio::io_service io_service;
    tcp::socket s;
protected:
    void bsend(std::string msg);
    void ssend(std::string msg);
    std::string bread(int lenght);
    std::string sread(int lenght);
private:
};
};

#endif // ATCPSOCKET_HPP
