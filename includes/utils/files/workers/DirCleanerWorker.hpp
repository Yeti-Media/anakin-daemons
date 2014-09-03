/*
 * DirCleanerWorker.hpp
 *
 *  Created on: 03/09/2014
 *      Author: Franco Pellegrini
 */

#ifndef DIRCLEANERWORKER_HPP_
#define DIRCLEANERWORKER_HPP_

#include <boost/filesystem/path.hpp>
#include <sys/types.h>
#include <utils/BlockingQueue.hpp>

namespace fs = boost::filesystem;

namespace Anakin {

/**
 * This class try to delete files located on BlockingQueue<fs::path*>* workingQueue
 */
class DirCleanerWorker {
public:
	/**
	 * workingQueue: blocking queue to use with other threads to deliver data
	 */
	DirCleanerWorker(BlockingQueue<fs::path*>* workingQueue,
			uint delayedSeconds);
	virtual ~DirCleanerWorker();

	void setWaiting(bool enabled);

	/**
	 * starts the worker listening cicle
	 */
	void start();
private:
	BlockingQueue<fs::path*>* fileQueue;
	uint delayedSeconds;
	bool waitingEnabled;
};

}
/* namespace Anakin */

#endif /* DIRCLEANERWORKER_HPP_ */
