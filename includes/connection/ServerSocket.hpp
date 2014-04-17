#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "connection/Socket.hpp"

namespace Anakin {

/**
 * Base class to listen for socket connections
 */
class ServerSocket {
public:
	/**
	 * if v then all received messages will be written to console
	 * this is basically to set the server verbose mode
	 */
	void setShowComs(bool v) {
		showComs = v;
	}
	/**
	 * if the server is listening
	 */
	bool isConnected() {
		return connected;
	}
	/**
	 * wait for a connection
	 */
	virtual Socket* waitForConnection() = 0;

	/**
	 * stops the server
	 */
	virtual void stopServer() = 0;
protected:
	bool showComs = false;
	bool connected = false;
	unsigned short port;
private:
};

}
;

#endif // SERVERSOCKET_HPP
