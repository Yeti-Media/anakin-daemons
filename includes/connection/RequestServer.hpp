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
#include <utils/files/TempDirCleaner.hpp>

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
	RequestServer(const Ptr<Flags> & serverFlags,
			const CacheConfig & cacheConfig, char mode, const string & pghost,
			const string & pgport, const string & dbName, const string & login,
			const string & pwd, unsigned int httpPort, int cap, int threads,
			bool verbose, const string & tempDir,
			TempDirCleaner * tempDirCleaner);

	virtual ~RequestServer();
protected:
	/**
	 * pushes the input to the blocking queue
	 */
	void execute(const Ptr<vector<string>> & input);
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
	void startWorkers(const Ptr<DataOutput> & output);
	/**
	 * creates and start a Worker
	 * ptr is a pointer to the worker's params
	 */
	static void * startWorker(void *ptr);
	int threads;
	int qcap;
	tbb::concurrent_bounded_queue<Ptr<vector<string>>>* workingQueue;
	/**
	 * structure to pass arguments to a Worker
	 */
	struct WorkerArgs {
		int id;
		string threadName;
		Ptr<DataOutput> output;
		Ptr<SFBMCache> cache;
		tbb::concurrent_bounded_queue<Ptr<vector<string>>>* workingQueue;
		WorkerArgs(int id, const string & threadName, const Ptr<DataOutput> & output, const Ptr<SFBMCache> & cache,
				tbb::concurrent_bounded_queue<Ptr<vector<string>>>* workingQueue) :
		id(id), threadName(threadName), output(output), cache(cache), workingQueue(workingQueue) {
		}
	};
	vector<pthread_t> * workerThreads;
};

template<class SpecificCommandRunner>
RequestServer<SpecificCommandRunner>::RequestServer(
		const Ptr<Flags> & serverFlags, const CacheConfig & cacheConfig,
		char mode, const string & pghost, const string & pgport,
		const string & dbName, const string & login, const string & pwd,
		unsigned int httpPort, int cap, int threads, bool verbose,
		const string & tempDir, TempDirCleaner * tempDirCleaner) :
		Server<SpecificCommandRunner>(serverFlags, cacheConfig, mode, pghost, pgport, dbName,
				login, pwd, httpPort, verbose, tempDir, tempDirCleaner) {
	this->threads = threads;
	this->workerThreads = new vector<pthread_t>(threads);
	this->qcap = cap;
	this->workingQueue = new tbb::concurrent_bounded_queue<Ptr<vector<string>>>();
	this->workingQueue->set_capacity(cap);
}

//PROTECTED

template<class SpecificCommandRunner>
void RequestServer<SpecificCommandRunner>::execute(
		const Ptr<vector<string>> & input) {
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
void RequestServer<SpecificCommandRunner>::startWorkers(
		const Ptr<DataOutput> & output) {
	for (int w = 0; w < this->threads; w++) {
		string threadName = to_string(w + 1);
		WorkerArgs* wargs = new WorkerArgs(w + 1, threadName, output,
				this->cache, this->workingQueue);
		pthread_create(&this->workerThreads->at(w), NULL, startWorker,
				(void*) wargs);
	}
}

template<class SpecificCommandRunner>
void * RequestServer<SpecificCommandRunner>::startWorker(void *ptr) {
	WorkerArgs* wargs = (WorkerArgs*) ptr;
	SpecificCommandRunner* commandRunner = new SpecificCommandRunner(
			wargs->threadName);
	commandRunner->initializeCommandRunner(wargs->output, wargs->cache);
	Worker* worker = new Worker(wargs->id, wargs->workingQueue, commandRunner);
	worker->start();
	delete worker;
	delete wargs;
	delete commandRunner;
}

template<class SpecificCommandRunner>
void RequestServer<SpecificCommandRunner>::stopWorkers() {
	for (int w = 0; w < this->threads; w++) {
		Ptr<vector<string>> nullPtr;
		this->workingQueue->push(nullPtr);
	}
}

template<class SpecificCommandRunner>
RequestServer<SpecificCommandRunner>::~RequestServer() {

	while (!this->workingQueue->empty()) {
		Ptr<vector<string>> input;
		this->workingQueue->pop(input);
	}
	delete this->workingQueue;
	delete this->workerThreads;
}

}
;

#endif // REQUESTSERVER_HPP
