#include "connection/DelimiterBasedTCPSocket.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/regex.hpp>

using namespace Anakin;

DelimiterBasedTCPSocket::DelimiterBasedTCPSocket(   std::string ip,
                                                    std::string port,
                                                    std::string ld,
                                                    std::string md,
                                                    char sendType
                                                    ) : ATCPSocket(ip, port, sendType){
    this->lineDelimiter = ld;
    this->messageDelimiter = md;
}

DelimiterBasedTCPSocket::DelimiterBasedTCPSocket(   tcp::socket sock,
                                                    std::string ld,
                                                    std::string md,
                                                    char sendType
                                                    ) : ATCPSocket(std::move(sock), sendType){
    this->lineDelimiter = ld;
    this->messageDelimiter = md;
}

void DelimiterBasedTCPSocket::send(std::string msg) {
    if (this->sendType==DEFAULTSEND) {
        bsend(msg);
    } else if (this->sendType==SYNCSEND) {
        ssend(msg);
    }
}

std::string DelimiterBasedTCPSocket::read() {
    std::string msg;
    if (this->sendType==DEFAULTSEND) {
        msg = bread();
    } else if (this->sendType==SYNCSEND) {
        msg = sread();
    }
    return msg;
}

std::string DelimiterBasedTCPSocket::getLineDelimiter() {
    return this->lineDelimiter;
}

std::string DelimiterBasedTCPSocket::getMessageDelimiter() {
    return this->messageDelimiter;
}

void DelimiterBasedTCPSocket::sendStop() {
    send("stop"+getMessageDelimiter());
}

//PROTECTED

void DelimiterBasedTCPSocket::bsend(std::string msg) {
    char *request=new char[msg.size()+1];
    request[msg.size()]=0;
    memcpy(request,msg.c_str(),msg.size()+1);
    size_t request_length = std::strlen(request);
    boost::asio::write(s, boost::asio::buffer(request, request_length));
    if (showComs) std::cout << "sent " << request << " to " << boost::lexical_cast<std::string>(s.remote_endpoint()) << std::endl;
}

void DelimiterBasedTCPSocket::ssend(std::string msg) {
    bsend(msg);
    bread(3);
}

std::string DelimiterBasedTCPSocket::bread(int lenght) {
    std::string msg = "";
    bool stop = false;
    bool error = false;
    while (!stop) {
        std::string lmsg;
        lmsg = read_until(this->lineDelimiter, this->messageDelimiter, &stop, &error);
        if (error) {
            std::cerr << "error while reading" << std::endl;
            exit(-1);
        }
        msg = msg.append(lmsg);
        if (showComs) std::cout << "read line " << lmsg <<  " from " << boost::lexical_cast<std::string>(s.remote_endpoint()) << std::endl;
    }
    boost::regex ldRx("<line>");
    boost::regex mdRx("<end>");
    std::string ldfmt("\\r\\n");
    std::string mdfmt("");
    msg = boost::regex_replace(msg, ldRx, ldfmt, boost::match_default | boost::format_all);
    msg = boost::regex_replace(msg, mdRx, mdfmt, boost::match_default | boost::format_all);
    if (showComs) std::cout << "received\n" << msg << " from " << boost::lexical_cast<std::string>(s.remote_endpoint()) << std::endl;
    return msg;
}

std::string DelimiterBasedTCPSocket::sread(int lenght) {
    std::string msg = bread();
    bsend("OK"+this->messageDelimiter);
    return msg;
}

std::string DelimiterBasedTCPSocket::read_until(std::string ld, std::string md, bool * eom, bool * error) {
    bool stop = false;
    bool eomFound = false;
    std::string msg = "";
    while (!stop) {
        boost::system::error_code ec;
        char character[1];
        boost::asio::read(s, boost::asio::buffer(character, 1), ec);
        if (ec) {
            *error = true;
        }
        else {
            std::string smsg(character);
            msg = msg.append(smsg);
            if (boost::algorithm::ends_with(msg, ld)) {
                stop = true;
            } else if (boost::algorithm::ends_with(msg, md)) {
                stop = true;
                eomFound = true;
            }
        }
    }
    *eom = eomFound;
    return msg;
}
