#ifndef DATAOUTPUT_HPP
#define DATAOUTPUT_HPP

#include <iostream>
#include <semaphore.h>
#include "connection/HTTPSocket.hpp"

namespace Anakin {

/**
 * This class is used to output anakin results/messages
 * The client of this class doesn't know how the messages are outputted
 */
class DataOutput {
public:
	/**
	 * Constructor (to use Socket to output data)
	 * s : a Socket object used to output data
	 */
	//DataOutput(Socket* s);

	/**
	 * Constructor (to use HTTP socket to output data)
	 * httpSocket : the http socket to output data
	 * -------------------------------------------
	 * note: read HTTPSocket and HTTPListener documentation
	 */
	DataOutput(HTTPSocket* httpSocket);
	/**
	 * Constructor (to use console to output data)
	 */
	DataOutput();
	/**
	 * output data and can optionally set an id for the message
	 *
	 * data : the data to output
	 * reqID : the message id
	 * --------------------------------
	 * note : if using HTTPSocket then reqID must be set
	 */
	void output(std::string data, int reqID = 0);
	/**
	 * output data and can optionally set an id for the message
	 *
	 * data : the data to output
	 * reqID : the message id
	 * --------------------------------
	 * note : if using HTTPSocket then reqID must be set
	 */
	void output(std::wstring data, int reqID = 0);

	void error(std::string data);

	void error(std::wstring data);

	/**
	 * On some cases the internal mechanism used to output data
	 * needs to be closed, for example when using Socket
	 * this function should always be called when closing anakin
	 */
	void close();
protected:
private:
	/**
	 * initialize the internal semaphores
	 */
	void initSem();
	//Socket* s;
	HTTPSocket* httpSocket;
	bool consoleOutput = true;
	bool httpOutput = false;
	sem_t ssem;
	sem_t wssem;
};
}
;

#endif // DATAOUTPUT_HPP
