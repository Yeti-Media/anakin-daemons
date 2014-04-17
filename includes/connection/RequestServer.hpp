#ifndef REQUESTSERVER_HPP
#define REQUESTSERVER_HPP

#include "Server.hpp"

#include <tbb/concurrent_queue.h>
#include <vector>

namespace Anakin {

/**
 * This class extends from Server and it receives and process requests
 * it uses a blocking queue to pass each request to the workers (objects that process a request)
 * it sends a NULL value to each worker when receiving a stop message
 */
class RequestServer: public Server {
public:
	/**
	 * Constructor
	 * will call Server constructor and initialize some internal structures like
	 * a blocking queue with a capacity of <cap>
	 * a vector of threads of size <threads>
	 */
	RequestServer(unsigned const short port, int cap, int threads,
			bool verbose = false, char mode = TCP, std::string ld = "",
			std::string md = "");
protected:
	/**
	 * pushes the input to the blocking queue
	 */
	void execute(std::vector<std::string>* input);
	/**
	 * calls the function stopWorkers
	 */
	void executeStop();
	/**
	 * TODO: MOVE IMPLEMENTATION TO SERVER AND REMOVE FROM THIS CLASS
	 */
	bool stopMessageReceived(std::string rawMsg);
	/**
	 * calls the function startWorkers
	 */
	void startServer();
	/**
	 * call pthread_join for every thread/worker
	 */
	void endServer();
private:
	/**
	 * for every thread/worker pushes a NULL value in the blocking queue
	 */
	void stopWorkers();
	/**
	 * starts <threads> threads and calls startWorker for each one
	 */
	void startWorkers(AnakinFlags* aflags, DataOutput* output);
	/**
	 * creates and start a Worker
	 * ptr is a pointer to the worker's params
	 */
	static void * startWorker(void *ptr);
	int threads;
	int qcap;
	tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue;
	/**
	 * structure to pass arguments to a Worker
	 */
	struct WorkerArgs {
		int id;
		Flags* flags;
		DataOutput* output;
		SFBMCache* cache;
		tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue;
		WorkerArgs(int id, Flags* flags, DataOutput* output, SFBMCache* cache,
				tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue) :
				id(id), flags(flags), output(output), cache(cache), workingQueue(
						workingQueue) {
		}
	};
	std::vector<pthread_t>* workerThreads;
};
}
;

#endif // REQUESTSERVER_HPP
