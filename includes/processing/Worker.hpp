#ifndef WORKER_HPP
#define WORKER_HPP

#include "processing/Flags.hpp"
#include "output/DataOutput.hpp"
#include "processing/SFBMCache.hpp"
#include "processing/commandrunner/CommandRunner.hpp"

#include <tbb/concurrent_queue.h>

namespace Anakin {

/**
 * This class will receive requests from a blocking queue and will delegate the request to a CommandRunner
 * for processing
 */
class Worker {
public:
	/**
	 * Constructor
	 * id            : worker's id
	 * workingQueue  : requests queue
	 */
	Worker(int id,
			tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue,
			CommandRunner* command);
	/**
	 * starts the worker listening cicle
	 */
	void start();
protected:
private:
	int id;
	tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue;
	CommandRunner* runner;
};
}
;

#endif // WORKER_HPP
