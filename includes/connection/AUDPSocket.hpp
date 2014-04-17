#ifndef AUDPSOCKET_HPP
#define AUDPSOCKET_HPP

#include "connection/Socket.hpp"
#include <boost/asio.hpp>

using boost::asio::ip::udp;

namespace Anakin {

/**
 * An UDP implementation of Socket
 * there's no actual connection but there's a connection protocol
 *
 * connect :
 *     1 - send local ip
 *     2 - send local port
 *     3 - read
 *     4 - if message received is "OK" then the connection was made succesfully
 *
 * wait for connection :
 *     1 - read remote ip
 *     2 - read remote port
 *     3 - send "OK"
 *
 * there's not protocol for disconnecting
 *
 */
class AUDPSocket: public Socket {
public:
	/**
	 * Constructor
	 * ip : the ip to connect
	 * port : the port to use
	 * sendType : synchonized or default send (see Socket)
	 */
	AUDPSocket(std::string ip, std::string port, char sendType = DEFAULTSEND);
	/**
	 * Constructor (used by AUDPServerSocket)
	 * port : the port to listen
	 * sendType : synchonized or default send (see Socket)
	 */
	AUDPSocket(unsigned short port, char sendType = DEFAULTSEND);bool connect();bool waitForConnection();
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

}
;

#endif // AUDPSOCKET_HPP
