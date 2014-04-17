#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sstream>

namespace Anakin
{

/**
*   Base clase for socket communication
*
*   note that when talking about connection we are not refering to the internal socket connection
*   like the connection made by a TCP socket. For example you could have a class that implements a socket
*   with TCP that uses a custom protocol for connecting, so even if the internal TCP socket is connected
*   our connection could not be made yet.
*/
class Socket
{
public:

    /**
    *   if v then all message received and sent
    *   will be written to console
    */
    void setShowComs(bool v)
    {
        showComs = v;
    }

    /**
    * check if a connection is made
    * even a UDP socket can have a true
    * value for this function.
    */
    bool isConnected()
    {
        return connected;
    }

    /**
    * initiates the connection protocol
    */
    virtual bool connect()
    {
        return true;
    }

    /**
    * initiates the wait for connection protocol
    */
    virtual bool waitForConnection()
    {
        return true;
    }

    /**
    * initiates the disconnection protocol
    */
    virtual bool disconnect()
    {
        return true;
    }

    /**
    * initiates the wait for disconnection protocol
    */
    virtual bool waitForDisconnection()
    {
        return true;
    }

    /**
    * send a message through the socket
    * if sendType == SYNCSEND this send will block
    * until a read is made from the other end
    */
    virtual void send(std::string msg)
    {
        std::stringstream ss2;
        ss2 << msg.length();
        std::string slength = ss2.str();
        slength = std::string( 6 - slength.length(), '0').append( slength);
        bsend(slength);
        if (this->sendType==DEFAULTSEND)
        {
            bsend(msg);
        }
        else if (this->sendType==SYNCSEND)
        {
            ssend(msg);
        }
    }

    /**
    * reads a message from the socket
    */
    virtual std::string read()
    {
        std::string slength = bread(6);
        int length = stoi(slength);
        std::string msg;
        if (this->sendType==DEFAULTSEND)
        {
            msg = bread(length);
        }
        else if (this->sendType==SYNCSEND)
        {
            msg = sread(length);
        }
        return msg;
    }

    /**
    * sends a stop message
    */
    virtual void sendStop()
    {
        send("stop");
    }

    /**
    * usually this means a blocking read
    * and a send that only blocks if the internal socket buffer is full
    */
    const static char DEFAULTSEND = 0;

    /**
    * both read and send blocks
    */
    const static char SYNCSEND = 1;
protected:
    bool showComs=false;
    bool connected=false;
    char sendType;
    bool constructedWithSocket = false;
    virtual void bsend(std::string msg) = 0;
    virtual void ssend(std::string msg) = 0;
    virtual std::string bread(int lenght) = 0;
    virtual std::string sread(int lenght) = 0;
private:
};

};

#endif // SOCKET_HPP
