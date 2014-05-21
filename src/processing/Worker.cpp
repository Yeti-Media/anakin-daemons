#include <processing/Worker.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace Anakin;

Worker::Worker(int id,
		tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue,
		CommandRunner* command) {
	this->id = id;
	this->workingQueue = workingQueue;
	this->runner = command;
}

void Worker::start() {
	bool run = true;
	while (run) {
		std::vector<std::string>* input;
		this->workingQueue->pop(input);
		std::cout << "worker(" << this->id << ") is processing a request"
				<< std::endl;
		if (input != NULL) {
			runner->validateRequest(input);
			runner->run();
		} else {
			run = false;
		}
	}
}
