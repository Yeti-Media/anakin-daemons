#ifndef DATAOUTPUT_HPP
#define DATAOUTPUT_HPP

#include <opencv2/opencv.hpp>
#include <output/DataEnumerates.hpp>
#include <output/Msj.hpp>
#include <pthread.h>
#include <utils/BlockingQueue.hpp>
#include <mutex>
#include <string>
#include <connection/HTTPSocket.hpp>
using namespace std;
using namespace cv;

namespace Anakin {

/**
 * This class is used to output Anakin results/messages
 * The client of this class doesn't know how the messages are outputted.
 *
 * This Data Output system create a thread that store the data to be
 * delivered, one by one. This is useful when the choosed output system is
 * slower than the threads that deliver concurrent data to some destination,
 * using the same channel/socket.
 */
class DataOutput {
public:
	/**
	 * Constructor (to use HTTP socket to output data)
	 * httpSocket : the http socket to output data
	 * -------------------------------------------
	 * note: read HTTPSocket and HTTPListener documentation
	 */
	DataOutput(const Ptr<HTTPSocket> & httpSocket);
	/**
	 * Constructor (use console only to output data)
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
	void output(const Ptr<wstring> & data, int reqID = 0);

	void error(const Ptr<wstring> & data);

	/**
	 * On some cases the internal mechanism used to output data
	 * needs to be closed, for example when using Socket
	 * this function should always be called when closing anakin
	 */
	void close();
private:

	/**
	 * used for block the concurrent access to DataOutput methods
	 */
	mutex outputMutex;

	/**
	 * used for msj storage and future delivering (so a working thread can be
	 * released to continue)
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
		Ptr<HTTPSocket> httpSocket;
		BlockingQueue<Msj*>* workingQueue;
		WorkerArgs(E_DataOutputType outputType, const Ptr<HTTPSocket> & httpSocket,
				BlockingQueue<Msj*>* workingQueue) :
				outputType(outputType), httpSocket(httpSocket), workingQueue(
						workingQueue) {
		}
	};

	/**
	 * thread that read the workingQueue and deliver the data using some
	 * specific implementation
	 */
	pthread_t workerThread;

};
}
;

#endif // DATAOUTPUT_HPP
