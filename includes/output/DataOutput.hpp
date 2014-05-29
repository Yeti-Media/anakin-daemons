#ifndef DATAOUTPUT_HPP
#define DATAOUTPUT_HPP

#include <iostream>
#include "connection/HTTPSocket.hpp"
#include <output/DataOutputWorker.hpp>
#include <output/DataEnumerates.hpp>
#include <utils/BlockingQueue.hpp>
#include <output/DataOutputWorker.hpp>
#include <output/Msj.hpp>
#include <pthread.h>

using namespace std;

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
	~DataOutput();

	/**
	 * output data and can optionally set an id for the message
	 *
	 * data : the data to output
	 * reqID : the message id
	 * --------------------------------
	 * note : if using HTTPSocket then reqID must be set
	 */
	void output(string data, int reqID = 0);
	/**
	 * output data and can optionally set an id for the message
	 *
	 * data : the data to output
	 * reqID : the message id
	 * --------------------------------
	 * note : if using HTTPSocket then reqID must be set
	 */
	void output(wstring data, int reqID = 0);

	void error(string data);

	void error(wstring data);

	/**
	 * On some cases the internal mechanism used to output data
	 * needs to be closed, for example when using Socket
	 * this function should always be called when closing anakin
	 */
	void close();
private:

	mutex mutex1;
	mutex mutex2;

	/**
	 * used for msj storage and future delivering (so working thread can be realesed
	 * to continue)
	 */
	BlockingQueue<Msj*>* workingQueue;

	/**
	 * start worker thread that dispatch msj
	 */
	static void * startWorker(void *ptr);
	/**
	 * structure to pass arguments to a Worker
	 */
	struct WorkerArgs {
		E_DataOutputType outputType;
		HTTPSocket* httpSocket;
		BlockingQueue<Msj*>* workingQueue;
		WorkerArgs(E_DataOutputType outputType, HTTPSocket* httpSocket,
				BlockingQueue<Msj*>* workingQueue) :
				outputType(outputType), httpSocket(httpSocket), workingQueue(
						workingQueue) {
		}
	};

	pthread_t workerThread;

};
}
;

#endif // DATAOUTPUT_HPP
