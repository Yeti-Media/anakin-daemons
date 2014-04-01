#ifndef REQUESTSERVER_HPP
#define REQUESTSERVER_HPP

#include "Server.hpp"

#include <tbb/concurrent_queue.h>
#include <vector>


namespace Anakin {

class RequestServer : public Server {
    public:
        RequestServer(unsigned const short port, int cap, int threads, bool verbose=false, char mode=TCP, std::string ld="", std::string md="");
    protected:
        void execute(std::vector<std::string>* input);
        void executeStop();
        bool stopMessageReceived(std::string rawMsg);
        void startServer();
        void endServer();
    private:
        void stopWorkers();
        void startWorkers(AnakinFlags* aflags, DataOutput* output);
        static void * startWorker(void *ptr);
        int threads;
        int qcap;
        tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue;
        struct WorkerArgs{
            int id;
            Flags* flags;
            DataOutput* output;
            SFBMCache* cache;
            tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue;
            WorkerArgs( int id,
                        Flags* flags,
                        DataOutput* output,
                        SFBMCache* cache,
                        tbb::concurrent_bounded_queue<std::vector<std::string>*>* workingQueue
                    ) : id(id), flags(flags), output(output), cache(cache), workingQueue(workingQueue) {}
        };
        std::vector<pthread_t>* workerThreads;
};
};

#endif // REQUESTSERVER_HPP
