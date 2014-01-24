#ifndef WORKER_HPP
#define WORKER_HPP

#include "Flags.hpp"
#include "DataOutput.hpp"

#include <tbb/concurrent_queue.h>

namespace Anakin {

class Worker {
    public:
        Worker(int id, Flags* flags, DataOutput* output, tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue);
        void start();
    protected:
    private:
        int id;
        Flags* flags;
        DataOutput* output;
        tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue;
};
};

#endif // WORKER_HPP
