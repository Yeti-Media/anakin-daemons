#ifndef REQUESTSERVER_HPP
#define REQUESTSERVER_HPP

//#include <boost/regex.hpp>
#include <connection/Server.hpp>
#include <output/DataOutput.hpp>
#include <processing/Flags.hpp>
#include <processing/Worker.hpp>
#include <pthread.h>
#include <tbb/concurrent_queue.h>
#include <connection/RequestServer.hpp>
#include <string>
#include <vector>

namespace Anakin {

/**
 * This class extends from Server and it receives and process requests
 * it uses a blocking queue to pass each request to the workers (objects that process
 * a request). It sends a NULL value to each worker when receiving a stop message.
 */
template<class SpecificCommandRunner>
class RequestServer: public Server<SpecificCommandRunner> {
public:
	/**
	 * Constructor
	 * will call Server constructor and initialize some internal structures like
	 * a blocking queue with a capacity of <cap>
	 * a vector of threads of size <threads>
	 */
	RequestServer(CacheConfig * cacheConfig, char mode, string pghost,
			string pgport, string dbName, string login, string pwd,
			unsigned int httpPort, int cap, int threads, bool verbose);

	virtual ~RequestServer();
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
	void startWorkers(DataOutput* output);
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
		DataOutput* output;
		SFBMCache* cache;
		tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue;
		WorkerArgs(int id, DataOutput* output, SFBMCache* cache,
				tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue) :
				id(id), output(output), cache(cache), workingQueue(workingQueue) {
		}
	};
	std::vector<pthread_t>* workerThreads;
};

template<class SpecificCommandRunner>
RequestServer<SpecificCommandRunner>::RequestServer(CacheConfig * cacheConfig,
		char mode, string pghost, string pgport, string dbName, string login,
		string pwd, unsigned int httpPort, int cap, int threads, bool verbose) :
		Server<SpecificCommandRunner>(cacheConfig, mode, pghost, pgport, dbName,
				login, pwd, httpPort, verbose) {
	this->threads = threads;
	this->workerThreads = new std::vector<pthread_t>(threads);
	this->qcap = cap;
	this->workingQueue = new tbb::concurrent_bounded_queue<
			std::vector<std::string>*>();
	this->workingQueue->set_capacity(cap);
}

//PROTECTED

template<class SpecificCommandRunner>
void RequestServer<SpecificCommandRunner>::execute(
		std::vector<std::string>* input) {
	this->workingQueue->push(input);
}

template<class SpecificCommandRunner>
void RequestServer<SpecificCommandRunner>::executeStop() {
	stopWorkers();
}

template<class SpecificCommandRunner>
void RequestServer<SpecificCommandRunner>::startServer() {
	startWorkers(this->output);
}

template<class SpecificCommandRunner>
void RequestServer<SpecificCommandRunner>::endServer() {
	for (int t = 0; t < this->threads; t++) {
		pthread_join(this->workerThreads->at(t), NULL);
	}
}

//PRIVATE
template<class SpecificCommandRunner>
void RequestServer<SpecificCommandRunner>::startWorkers(DataOutput* output) {
	for (int w = 0; w < this->threads; w++) {
		WorkerArgs* wargs = new WorkerArgs(w + 1, output, this->cache,
				this->workingQueue);
		pthread_create(&this->workerThreads->at(w), NULL, startWorker,
				(void*) wargs);
	}
}

template<class SpecificCommandRunner>
void * RequestServer<SpecificCommandRunner>::startWorker(void *ptr) {
	WorkerArgs* wargs = (WorkerArgs*) ptr;
	Worker* worker = new Worker(wargs->id, wargs->workingQueue,
			new SpecificCommandRunner(wargs->output, wargs->cache));
	worker->start();
}

template<class SpecificCommandRunner>
void RequestServer<SpecificCommandRunner>::stopWorkers() {
	for (int w = 0; w < this->threads; w++) {
		this->workingQueue->push(NULL);
	}
}

template<class SpecificCommandRunner>
RequestServer<SpecificCommandRunner>::~RequestServer() {

	while (!this->workingQueue->empty()) {
		vector<string>* input;
		this->workingQueue->pop(input);
		if (input != NULL) {
			delete input;
		}
	}
	delete this->workingQueue;
	delete this->workerThreads;
}

}
;

#endif // REQUESTSERVER_HPP
