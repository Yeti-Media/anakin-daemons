#include "processing/Worker.hpp"

using namespace Anakin;

Worker::Worker( int id,
                Flags* flags,
                DataOutput* output,
                SFBMCache* cache,
                tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue)
{
    this->id = id;
    this->flags = flags;
    this->output = output;
    this->cache = cache;
    this->workingQueue = workingQueue;
    this->runner = new CommandRunner(flags, output, cache);
}

void Worker::start()
{
    bool run = true;
    while(run)
    {
        std::vector<std::string>* input;
        this->workingQueue->pop(input);
        std::cout << "worker(" << this->id << ") is processing a request" << std::endl;
        if (input != NULL)
        {
            //CommandRunner* runner = new CommandRunner(flags, output, this->cache, input);
            runner->validateRequest(input);
            runner->run();
        }
        else
        {
            run = false;
        }
    }
}
