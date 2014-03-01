#include "Worker.hpp"
#include "CommandRunner.hpp"

using namespace Anakin;

Worker::Worker( int id,
                Flags* flags,
                DataOutput* output,
                tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue) {
    this->id = id;
    this->flags = flags;
    this->output = output;
    this->workingQueue = workingQueue;
}

void Worker::start() {
    bool run = true;
    while(run) {
        std::vector<std::string>* input;
        this->workingQueue->pop(input);
        std::cout << "worker(" << this->id << ") is processing a request" << std::endl;
        if (input != NULL) {
            CommandRunner* runner = new CommandRunner(flags, output, input, false);
            runner->run();
        } else {
            run = false;
        }
    }
}
