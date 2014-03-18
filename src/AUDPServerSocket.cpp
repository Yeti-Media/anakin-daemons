#include "AUDPServerSocket.hpp"

#include <AUDPSocket.hpp>
#include <boost/lexical_cast.hpp>

using namespace Anakin;
using namespace std;

AUDPServerSocket::AUDPServerSocket(unsigned short port) {
    this->port = port;
}

Socket* AUDPServerSocket::waitForConnection() {
//    boost::asio::io_service io_service;
//    udp::socket sock(io_service, udp::endpoint(udp::v4(), this->port));
    Socket* s = new AUDPSocket(this->port);
    s->setShowComs(this->showComs);
    s->waitForConnection();
    return s;
}

void AUDPServerSocket::stopServer() {}
