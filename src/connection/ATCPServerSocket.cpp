#include "connection/ATCPServerSocket.hpp"

#include "connection/ATCPSocket.hpp"
#include <boost/lexical_cast.hpp>

using namespace Anakin;
using namespace std;

ATCPServerSocket::ATCPServerSocket(unsigned short port) : a(io_service, tcp::endpoint(tcp::v4(), port))
{
    this->port = port;
    this->connected = true;
}

Socket* ATCPServerSocket::waitForConnection()
{
    tcp::socket sock(io_service);
    if (showComs) cout << "listening at port " << this->port << endl;
    a.accept(sock);
    if (showComs) cout << "accepted a connection from " << boost::lexical_cast<std::string>(sock.remote_endpoint()) << endl;
    Socket* s = new ATCPSocket(std::move(sock));
    return s;
}

void ATCPServerSocket::stopServer()
{
    this->a.cancel();
    this->a.close();
    this->connected = false;
}
