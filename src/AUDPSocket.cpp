#include "AUDPSocket.hpp"
#include <boost/lexical_cast.hpp>

using namespace Anakin;
using namespace std;

AUDPSocket::AUDPSocket(std::string ip, std::string port, char sendType) : s(io_service/*, udp::endpoint(udp::v4(), boost::lexical_cast<int>(port))*/) {
    remote_endpoint = boost::asio::ip::udp::endpoint(
                                        boost::asio::ip::address::from_string(ip),
                                        boost::lexical_cast<int>(port)
                                        );
    this->ip = ip;
    this->port = port;
    this->s.open(udp::v4());
    //this->s.bind(remote_endpoint);
    this->sendType = sendType;
}

AUDPSocket::AUDPSocket(unsigned short port, char sendType) : s(io_service, udp::endpoint(udp::v4(), port)){
    this->sendType = sendType;
    //this->s.open(udp::v4());
}

bool AUDPSocket::connect() {
    if (showComs) cout << "Connecting to " << this->ip << ":" << this->port << endl;
    send(this->ip);
    send(boost::lexical_cast<std::string>(this->s.local_endpoint().port()));
    string response = read();
    if (showComs) cout << "Remote host responded with : " << response << endl;
    if (response == "OK") {
        if (showComs) cout << "Connected to " << this->ip << ":" << this->port << endl;
        return true;
    }
    if (showComs) cout << "Connection failed :(" << endl;
    return false;
}

bool AUDPSocket::waitForConnection() {
    if (showComs) cout << "Waiting for connection..." << endl;
    string ip = read();
    string port = read();
    string lastSender = boost::lexical_cast<std::string>(sender_endpoint);
    string fullMsg;
    fullMsg.append(ip).append(":").append(port);
    if (showComs) cout << "Connection data received: " << fullMsg << endl;
    remote_endpoint = boost::asio::ip::udp::endpoint(
                                        boost::asio::ip::address::from_string(ip),
                                        boost::lexical_cast<int>(port)
                                        );
    this->ip = ip;
    this->port = port;
    send("OK");
    if (showComs) cout << "Connected with: " << lastSender << endl;
    return true;
}

//PROTECTED

void AUDPSocket::bsend(string msg) {
    char *request=new char[msg.size()+1];
    request[msg.size()]=0;
    memcpy(request,msg.c_str(),msg.size()+1);
    size_t request_length = std::strlen(request);
    this->s.send_to(boost::asio::buffer(request, request_length), this->remote_endpoint);//boost::asio::write(s, boost::asio::buffer(request, request_length));
    if (showComs) cout << "sent " << request << " to " << boost::lexical_cast<std::string>(this->remote_endpoint) << endl;
}

void AUDPSocket::ssend(string msg) {
    bsend(msg);
    bread(3);
}

string AUDPSocket::bread(int lenght) {
    char msg[lenght];
    this->s.receive_from(boost::asio::buffer(msg, lenght), sender_endpoint);//*size_t reply_length = */boost::asio::read(s, boost::asio::buffer(msg, lenght));
    string result(msg);
    result = result.substr(0, lenght);
    if (showComs) cout << "received " << result << " from " << boost::lexical_cast<std::string>(sender_endpoint) << endl;
    return result;
}

string AUDPSocket::sread(int lenght) {
    string msg = bread(lenght);
    bsend("OK");
    return msg;
}
