#include "ATCPSocket.hpp"
#include <boost/lexical_cast.hpp>

using namespace Anakin;
using namespace std;

ATCPSocket::ATCPSocket(string ip, string port, char sendType) : s(io_service){
    tcp::resolver resolver(io_service);
    boost::asio::connect(this->s, resolver.resolve({ip, port}));
    this->sendType = sendType;
}

ATCPSocket::ATCPSocket(tcp::socket sock, char sendType) : s(std::move(sock)){
    this->sendType = sendType;
}

//PROTECTED

void ATCPSocket::bsend(string msg) {
    char *request=new char[msg.size()+1];
    request[msg.size()]=0;
    memcpy(request,msg.c_str(),msg.size()+1);
    size_t request_length = std::strlen(request);
    boost::asio::write(s, boost::asio::buffer(request, request_length));
    if (showComs) cout << "sent " << request << " to " << boost::lexical_cast<std::string>(s.remote_endpoint()) << endl;
}

void ATCPSocket::ssend(string msg) {
    bsend(msg);
    bread(3);
}

string ATCPSocket::bread(int lenght) {
    char msg[lenght];
    /*size_t reply_length = */boost::asio::read(s, boost::asio::buffer(msg, lenght));
    string result(msg);
    result = result.substr(0, lenght);
    if (showComs) cout << "received " << result << " from " << boost::lexical_cast<std::string>(s.remote_endpoint()) << endl;
    return result;
}

string ATCPSocket::sread(int lenght) {
    string msg = bread(lenght);
    bsend("OK");
    return msg;
}
