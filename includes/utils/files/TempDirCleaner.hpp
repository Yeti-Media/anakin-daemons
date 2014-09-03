/*
 * TempDirCleaner.hpp
 *
 *  Created on: 3/9/2014
 *      Author: Franco Pellegrini
 */

#ifndef TEMPDIRCLEANER_HPP
#define TEMPDIRCLEANER_HPP

#include <boost/filesystem/path.hpp>
#include <pthread.h>
#include <sys/types.h>
#include <utils/BlockingQueue.hpp>
#include <mutex>


namespace fs = boost::filesystem;

using namespace std;

namespace Anakin {

/**
 * This class is used to clean/delete files in another delayed thread.
 */
class TempDirCleaner {
public:

	/**
	 * Constructor
	 *
	 * uint delayedSeconds = how many seconds to wait between file deletion
	 */
	TempDirCleaner(uint delayedSeconds);

	~TempDirCleaner();

	/**
	 * queue a file to be deleted.
	 */
	void deleteFile(const fs::path & file);

	/**
	 * Force the deletion of all files without the common pause between them.
	 */
	void close();
private:

	/**
	 * used for block the concurrent access to DataOutput methods
	 */
	mutex fileMutex;

	/**
	 * used for msj storage and future delivering (so a working thread can be
	 * released to continue)
	 */
	BlockingQueue<fs::path*>* workingQueue;


	/**
	 * start worker thread that dispatch msj
	 */
	static void * startWorker(void *ptr);

	/**
	 * structure to pass arguments to a Worker
	 */
	struct WorkerArgs {
		BlockingQueue<fs::path*>* workingQueue;
		uint delayedSeconds;
		WorkerArgs(BlockingQueue<fs::path*>* workingQueue, uint delayedSeconds) :
				workingQueue(workingQueue), delayedSeconds(delayedSeconds) {
		}
	};

	/**
	 * thread that read the workingQueue and deliver the data using some
	 * specific implementation
	 */
	pthread_t workerThread;


	/**
	 * waiting between file deletion
	 */
	uint delayedSeconds;

};
}
;

#endif // TEMPDIRCLEANER_HPP
