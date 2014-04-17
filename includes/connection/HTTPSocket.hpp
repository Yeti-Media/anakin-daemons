#ifndef HTTPSOCKET_HPP
#define HTTPSOCKET_HPP

#include "mongoose.h"
#include <tbb/concurrent_queue.h>
#include "semaphore.h"
#include <vector>
#include <pthread.h>
#include "utils/BlockingMap.hpp"

namespace Anakin {

/**
 * this class allows to get the body from a request and to send a response
 *
 * it uses a BlockingQueue for reading (the HTTPListener pushes requests to this queue and then the
 * read function obtains the message from that queue)
 *
 * it uses a BlockingMap to send responses (the response function create a MessageData object and stores it
 * in the blocking map with a request id)
 */
class HTTPSocket {
public:
	/**
	 * Constructor
	 * port : the port in which the HTTPListener will listen to connections/requests
	 * threads : how many threads will mongoose use in HTPPListener
	 */
	HTTPSocket(std::string port, int threads);

	/**
	 * this will generate a MessageData corresponding to a response
	 * body : the body of the response
	 * statusOK : if true then the response will have a code 200 else the response will have a code 400
	 * reqID : the request ID to which this response responds
	 */
	void respond(std::string body, bool statusOK, int reqID);

	/**
	 * get the body of an HTTP request
	 * uses a blocking queue to get a MessageData object stored by HTTPListener
	 */
	std::string read();

	static const char POST = 0;
	static const char GET = 1;
	static const char RESPONSE = 2;

	/**
	 * sets verbose mode (on/off)
	 */
	void setShowComs(bool v) {
		showComs = v;
	}

	/**
	 * structure used to stored the data from an HTTP request
	 */
	struct MessageData {
		std::string method;
		std::string action;
		std::string body;
		char request;
		int status;
		MessageData(std::string action, std::string body, char request,
				int status) :
				action(action), body(body), request(request), status(status) {
			switch (request) {
			case HTTPSocket::POST: {
				method = "POST";
				break;
			}
			case HTTPSocket::GET: {
				method = "GET";
				break;
			}
			case HTTPSocket::RESPONSE: {
				method = "";
				break;
			}
			}
		}
	};

	/**
	 * structure used to pass arguments to the HTTPListener when creating
	 * the listening thread
	 */
	struct ListenerArgs {
		std::string port;
		tbb::concurrent_bounded_queue<MessageData*>* readingQueue;
		BlockingMap<int, MessageData*>* writtingQueue;
		int threads;
		ListenerArgs(std::string port,
				tbb::concurrent_bounded_queue<MessageData*>* readingQueue,
				BlockingMap<int, MessageData*>* writtingQueue, int threads) :
				port(port), readingQueue(readingQueue), writtingQueue(
						writtingQueue), threads(threads) {
		}
	};

	/**
	 * waits for the listening thread to stop
	 */
	void stop();

protected:
private:
	bool showComs = false;
	struct mg_server *server;
	std::string port;

	sem_t sem;
	pthread_t t;

	tbb::concurrent_bounded_queue<MessageData*>* readingQueue;
	BlockingMap<int, MessageData*>* writtingQueue;

	/**
	 * creates a new thread that will run an HTTPListener
	 */
	void startToListen(std::string port,
			tbb::concurrent_bounded_queue<MessageData*>* readingQueue,
			BlockingMap<int, MessageData*>* writtingQueue, int threads);
	/**
	 * starts the HTTPListener
	 */
	static void * startListener(void *ptr);
};
}
;

#endif // HTTPSOCKET_HPP
