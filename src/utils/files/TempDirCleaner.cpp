/*
 * TempDirCleaner.hpp
 *
 *  Created on: 3/9/2014
 *      Author: Franco Pellegrini
 */

#include <stddef.h>
#include <utils/files/TempDirCleaner.hpp>
#include <utils/files/workers/DirCleanerWorker.hpp>

using namespace Anakin;
using namespace std;
namespace fs = boost::filesystem;

TempDirCleaner::TempDirCleaner(uint delayedSeconds) {
	workingQueue = new BlockingQueue<fs::path*>();
	this->delayedSeconds = delayedSeconds;
	WorkerArgs* wargs = new WorkerArgs(workingQueue, delayedSeconds);
	pthread_create(&this->workerThread, NULL, startWorker, (void *) wargs);
}

void* TempDirCleaner::startWorker(void *ptr) {
	WorkerArgs* wargs = (WorkerArgs*) ptr;
	DirCleanerWorker* worker = new DirCleanerWorker(wargs->workingQueue, wargs->delayedSeconds);
	worker->start();
	delete worker;
}

TempDirCleaner::~TempDirCleaner() {
	close();
	delete workingQueue;
}

void TempDirCleaner::deleteFile(const fs::path & file) {
	workingQueue->push(new fs::path(file));
}

void TempDirCleaner::close() {
	workingQueue->push(NULL);
	pthread_join(this->workerThread, NULL);
}
