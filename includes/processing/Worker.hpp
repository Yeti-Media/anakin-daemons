#ifndef WORKER_HPP
#define WORKER_HPP

#include "processing/Flags.hpp"
#include "output/DataOutput.hpp"
#include "processing/SFBMCache.hpp"
#include "processing/CommandRunner.hpp"

#include <tbb/concurrent_queue.h>

namespace Anakin
{

/**
* This class will receive requests from a blocking queue and will delegate the request to a CommandRunner
* for processing
*/
class Worker
{
public:
    /**
    * Constructor
    * id            : worker's id
    * flag          : arguments checker
    * output        : an object used to output responses/messages
    * cache         : cache used by CommandRunner
    * workingQueue  : requests queue
    */
    Worker(int id, Flags* flags, DataOutput* output, SFBMCache* cache, tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue);
    /**
    * starts the worker listening cicle
    */
    void start();
protected:
private:
    int id;
    Flags* flags;
    DataOutput* output;
    SFBMCache* cache;
    tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue;
    CommandRunner* runner;
};
};

#endif // WORKER_HPP
