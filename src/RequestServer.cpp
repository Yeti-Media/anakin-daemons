#include "RequestServer.hpp"
#include "Worker.hpp"
#include <pthread.h>
#include "DTCPServerSocket.hpp"
#include <boost/regex.hpp>

using namespace Anakin;

RequestServer::RequestServer(   unsigned const short port,
                                int cap,
                                int threads,
                                bool verbose,
                                char mode,
                                std::string ld,
                                std::string md) : Server(port, verbose, mode, ld, md) {
    this->threads = threads;
    this->workerThreads = new std::vector<pthread_t>(threads);
    this->qcap = cap;
    this->workingQueue = new tbb::concurrent_bounded_queue<std::vector<std::string>*>();
    this->workingQueue->set_capacity(cap);
}

//PROTECTED

void RequestServer::execute(std::vector<std::string>* input) {
    this->workingQueue->push(input);
}

void RequestServer::executeStop() {
    stopWorkers();
}

bool RequestServer::stopMessageReceived(std::string rawMsg) {
    std::string msg = rawMsg;
    if (this->mode & DTCP) {
        std::string ld = ((DTCPServerSocket*)this->server)->getLineDelimiter();
        std::string md = ((DTCPServerSocket*)this->server)->getMessageDelimiter();
        boost::regex ldRx(ld);
        boost::regex mdRx(md);
        std::string ldfmt("");
        std::string mdfmt("");
        msg = boost::regex_replace(msg, ldRx, ldfmt, boost::match_default | boost::format_all);
        msg = boost::regex_replace(msg, mdRx, mdfmt, boost::match_default | boost::format_all);
    }
    return msg == "stop";
}

void RequestServer::startServer() {
    startWorkers(this->aflags, this->output);
}

void RequestServer::endServer() {
    for (int t = 0; t < this->threads; t++) {
        pthread_join( this->workerThreads->at(t), NULL);
    }
}


//PRIVATE

void RequestServer::startWorkers(AnakinFlags* aflags, DataOutput* output) {
    for (int w = 0; w < this->threads; w++) {
        WorkerArgs* wargs = new WorkerArgs(w+1, aflags->getFlags(), output, this->cache, this->workingQueue);
        pthread_create( &this->workerThreads->at(w), NULL, startWorker, (void*) wargs);
    }
}

void * RequestServer::startWorker(void *ptr) {
    WorkerArgs* wargs = (WorkerArgs*) ptr;
    Worker* worker = new Worker(wargs->id, wargs->flags, wargs->output, wargs->cache, wargs->workingQueue);
    worker->start();
}

void RequestServer::stopWorkers() {
    for (int w = 0; w < this->threads; w++) {
        this->workingQueue->push(NULL);
    }
}
