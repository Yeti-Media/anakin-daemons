#ifndef HTTPLISTENER_HPP
#define HTTPLISTENER_HPP

#include <mongoose.h>
#include <tbb/concurrent_queue.h>
#include <vector>
#include "connection/HTTPSocket.hpp"
#include "utils/BlockingMap.hpp"
//#include <output/communicationFormatter/ICommunicationFormatter.hpp>

namespace Anakin {

/**
 * This class will wait for http connections and will handle http requests
 * a request id is randomly generated and added as a -reqID <id> flag
 * to the request in the http body
 *
 * the request is then pushed into a blocking queue
 *
 * after the request is pushed the response is obtained using the request ID as key
 * the get method will block until there's a request for that particular ID
 *
 * to avoid locking the listener until a request is processed several listener threads are created
 * mongoose is used to make the HTTP server and the listener threads are managed by this library
 */
class HTTPListener {
public:
	/**
	 * get an instance of this class
	 */
	static HTTPListener* getInstance(std::string port,
			tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
			BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue,
			int threads);
	/**
	 * starts to poll the HTTP server until a stop message is received
	 */
	void start();
protected:
	static HTTPListener* instance;
	/**
	 * Constructor
	 * port : the port in which the server will listen
	 * readingQueue : the queue for requests
	 * writtingQueue : a blocking map for responses
	 * threads : how many threads will mongoose use for polling
	 */
	HTTPListener(std::string port,
			tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
			BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue,
			int threads);
	static std::string port;
	static bool running;
	/**
	 * handles a request (more info on the implementation code)
	 */
	static int ev_handler(struct mg_connection *conn, enum mg_event ev);
	static struct mg_server *server;
	static tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue;
	static BlockingMap<int, HTTPSocket::MessageData*>* writtingQueue;
	static int lastID;
	//static I_CommunicationFormatter cf;
};

}
;

#endif // HTTPLISTENER_HPP
