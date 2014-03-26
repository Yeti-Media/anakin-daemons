#include "HTTPSocket.hpp"
#include <iostream>
#include "HTTPListener.hpp"

using namespace Anakin;

HTTPSocket::HTTPSocket(std::string port, int threads) {
    this->port = port;
    this->readingQueue = new tbb::concurrent_bounded_queue<MessageData*>();
    this->writtingQueue = new tbb::concurrent_bounded_queue<MessageData*>();
    if (sem_init (&this->sem, 0, 1) != 0) {
        std::cout << "HTTPSocket#HTTPSocket: error initializing semaphore\n";
        exit(-1);
    }
    startToListen(this->port, this->readingQueue, this->writtingQueue, threads);
}

void HTTPSocket::respond(std::string body, bool statusOK) {
    int status = statusOK?200:400;
    HTTPSocket::MessageData* rd = new HTTPSocket::MessageData("", body, HTTPSocket::RESPONSE, status);
    this->writtingQueue->push(rd);
}

std::string HTTPSocket::read() {
    HTTPSocket::MessageData* md;
    this->readingQueue->pop(md);
    return md->body;
}

void HTTPSocket::stop() {
    pthread_join( this->t, NULL);
}

//PROTECTED

//PRIVATE

void HTTPSocket::startToListen( std::string port,
                                tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
                                tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* writtingQueue,
                                int threads) {
    ListenerArgs* largs = new ListenerArgs(port, readingQueue, writtingQueue, threads);
    pthread_create( &this->t, NULL, startListener, (void*) largs);
}

void * HTTPSocket::startListener(void *ptr) {
    ListenerArgs* largs = (ListenerArgs*) ptr;
    HTTPListener* listener = HTTPListener::getInstance(largs->port, largs->readingQueue, largs->writtingQueue, largs->threads);
    listener->start();
}
