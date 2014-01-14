#ifndef AUDPSOCKET_HPP
#define AUDPSOCKET_HPP

#include <Socket.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

namespace Anakin {

class AUDPSocket : public Socket {
    public:
        AUDPSocket(std::string ip, std::string port, char sendType=DEFAULTSEND);
        AUDPSocket(unsigned short port, char sendType=DEFAULTSEND);
        bool connect();
        bool waitForConnection();
        boost::asio::io_service io_service;
        udp::socket s;
    protected:
        virtual void bsend(std::string msg);
        virtual void ssend(std::string msg);
        virtual std::string bread(int lenght);
        virtual std::string sread(int lenght);
        udp::endpoint remote_endpoint;
        udp::endpoint sender_endpoint;
        std::string ip;
        std::string port;
    private:
};

};

#endif // AUDPSOCKET_HPP
