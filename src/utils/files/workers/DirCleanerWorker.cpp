/*
 * DirCleanerWorker.cpp
 *
 *  Created on: 29/05/2014
 *      Author: Franco Pellegrini
 */

#include <boost/filesystem/operations.hpp>
#include <unistd.h>
#include <utils/files/workers/DirCleanerWorker.hpp>
#include <string>

using namespace std;

namespace Anakin {

DirCleanerWorker::DirCleanerWorker(BlockingQueue<fs::path*>* workingQueue,
		uint delayedSeconds) :
		fileQueue(workingQueue), delayedSeconds(delayedSeconds) {

	if (delayedSeconds == 0) {
		waitingEnabled = false;
	} else {
		waitingEnabled = true;
	}
}

DirCleanerWorker::~DirCleanerWorker() {
}

void DirCleanerWorker::start() {
	bool run = true;
	while (run) {
		fs::path* file = this->fileQueue->pop();
		if (file != NULL) {
			if (boost::filesystem::exists(*file))
				boost::filesystem::remove(*file);
			delete file;
			if (waitingEnabled)
				sleep(delayedSeconds);
		} else {
			run = false;
		}
	}
}

void DirCleanerWorker::setWaiting(bool enabled) {
	this->waitingEnabled = enabled;
}

} /* namespace Anakin */
