#ifndef ATCPSOCKET_HPP
#define ATCPSOCKET_HPP

#include "connection/Socket.hpp"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace Anakin {

class ATCPSocket : public Socket {
    public:
        ATCPSocket(std::string ip, std::string port, char sendType=DEFAULTSEND);
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
