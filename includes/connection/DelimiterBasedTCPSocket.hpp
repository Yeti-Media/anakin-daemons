#ifndef DELIMITERBASEDTCPSOCKET_HPP
#define DELIMITERBASEDTCPSOCKET_HPP

#include "connection/ATCPSocket.hpp"

namespace Anakin
{

/**
* This class extends ATCPSocket to support messages using a line delimiter and a message delimiter
* because of these delimiters the read and send functions doesn't need the message lenght
*
* an example of a message using this class is
*
* this is a line<line>this is a second line<line><end>
*
*/
class DelimiterBasedTCPSocket : public ATCPSocket
{
public:
    /**
    * Constructor
    * ip : the ip to connect
    * port : the port to connect
    * ld : line delimiter
    * md : message delimiter
    * sendType : synchonized or default send (see Socket)
    */
    DelimiterBasedTCPSocket(std::string ip, std::string port, std::string ld, std::string md, char sendType=DEFAULTSEND);
    /**
    * Constructor (used by DTCPServerSocket when receiven a new connection)
    * sock : a tcp socket
    * ld : line delimiter
    * md : message delimiter
    * sendType : synchonized or default send (see Socket)
    */
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
    /**
    * will read from the socket until either a line delimiter or a message delimiter is found
    * ld : line delimiter
    * md : message delimiter
    * eom : will store true if a message delimiter was found
    * error : will store true if an error ocurred while reading
    */
    std::string read_until(std::string ld, std::string md, bool * eom, bool * error);
private:
};
};

#endif // DELIMITERBASEDTCPSOCKET_HPP
