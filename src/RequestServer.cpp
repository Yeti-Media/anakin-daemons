#include "RequestServer.hpp"
#include "Worker.hpp"
#include <pthread.h>

using namespace Anakin;

RequestServer::RequestServer(   unsigned const short port,
                                int cap,
                                int threads,
                                bool verbose,
                                char mode) : Server(port, verbose, mode) {
    this->threads = threads;
    this->workerThreads = new std::vector<pthread_t>(threads);
    this->qcap = cap;
    this->workingQueue = new tbb::concurrent_bounded_queue<std::vector<std::string>*>();
    this->workingQueue->set_capacity(cap);
}

void RequestServer::start(Flags* flags, DataOutput* output) {
    if (this->mode & TCP || this->mode & UDP) {
        this->socket = this->server->waitForConnection();
        this->socket->setShowComs(this->verbose);
    }
    startWorkers(flags, output);
    string msg;
    do {
        vector<string> *input = new vector<string>(0);
        msg = read();
        if (msg != "stop") {
            cout << "MESSAGE RECEIVED: " << msg << endl;
            stringstream ss_input(msg);
            while(ss_input.good()) {
                string value;
                ss_input >> value;
                input->push_back(value);
            }
            this->workingQueue->push(input);
        } else {
            stopWorkers();
        }
    } while (msg != "stop");
    for (int t = 0; t < this->threads; t++) {
        pthread_join( this->workerThreads->at(t), NULL);
    }
    output->close();
    if (this->mode & TCP || this->mode & UDP) this->server->stopServer();
}


//PRIVATE

void RequestServer::startWorkers(Flags* flags, DataOutput* output) {
    for (uint w = 0; w < this->threads; w++) {
        WorkerArgs* wargs = new WorkerArgs(w+1, flags, output, this->workingQueue);
        pthread_create( &this->workerThreads->at(w), NULL, startWorker, (void*) wargs);
    }
}

void * RequestServer::startWorker(void *ptr) {
    WorkerArgs* wargs = (WorkerArgs*) ptr;
    Worker* worker = new Worker(wargs->id, wargs->flags, wargs->output, wargs->workingQueue);
    worker->start();
}

void RequestServer::stopWorkers() {
    for (uint w = 0; w < this->threads; w++) {
        this->workingQueue->push(NULL);
    }
}
