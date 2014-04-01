#ifndef DELIMITERBASEDTCPSOCKET_HPP
#define DELIMITERBASEDTCPSOCKET_HPP

#include "connection/ATCPSocket.hpp"

namespace Anakin {

class DelimiterBasedTCPSocket : public ATCPSocket {
    public:
        DelimiterBasedTCPSocket(std::string ip, std::string port, std::string ld, std::string md, char sendType=DEFAULTSEND);
        DelimiterBasedTCPSocket(tcp::socket sock, std::string ld, std::string md, char sendType=DEFAULTSEND);
        void send(std::string msg);
        std::string read();
        std::string getLineDelimiter();
        std::string getMessageDelimiter();
        void sendStop();
    protected:
        void bsend(std::string msg);
        void ssend(std::string msg);
        std::string bread(int lenght=0);
        std::string sread(int lenght=0);
        std::string lineDelimiter;
        std::string messageDelimiter;
        std::string read_until(std::string ld, std::string md, bool * eom, bool * error);
    private:
};
};

#endif // DELIMITERBASEDTCPSOCKET_HPP
