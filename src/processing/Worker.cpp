#include <processing/Worker.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace Anakin;
using namespace std;

Worker::Worker(int id,
		tbb::concurrent_bounded_queue<Ptr<vector<string>>>* workingQueue,
		CommandRunner* command) {
	this->id = id;
	this->workingQueue = workingQueue;
	this->runner = command;
}

void Worker::start() {
	bool run = true;
	while (run) {
		Ptr<vector<string>> input;
		this->workingQueue->pop(input);
		if (input.get() != NULL) {
			runner->validateRequest(input);
			runner->run();
		} else {
			run = false;
		}
		delete input;
	}
}
