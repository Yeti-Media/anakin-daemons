#ifndef REQUESTSERVER_HPP
#define REQUESTSERVER_HPP

#include "Server.hpp"

#include <tbb/concurrent_queue.h>
#include <vector>


namespace Anakin {

class RequestServer : public Server {
    public:
        RequestServer(unsigned const short port, int cap, int threads, bool verbose=false, char mode=TCP);
        void start(Flags* flags, DataOutput* output);
    protected:
    private:
        void stopWorkers();
        void startWorkers(Flags* flags, DataOutput* output);
        static void * startWorker(void *ptr);
        int threads;
        int qcap;
        tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue;
        struct WorkerArgs{
            int id;
            Flags* flags;
            DataOutput* output;
            tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue;
            WorkerArgs( int id,
                        Flags* flags,
                        DataOutput* output,
                        tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue
                    ) : id(id), flags(flags), output(output), workingQueue(workingQueue) {}
        };
        std::vector<pthread_t>* workerThreads;
};
};

#endif // REQUESTSERVER_HPP
