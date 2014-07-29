#include <connection/HTTPListener.hpp>
#include <connection/HTTPSocket.hpp>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace Anakin;
using namespace std;

HTTPSocket::HTTPSocket(string port, int threads) {
	this->port = port;
	this->readingQueue = new tbb::concurrent_bounded_queue<MessageData*>();
	this->writtingQueue = new BlockingMap<int, MessageData*>(NULL);
	this->writtingQueue->setOverridingKey(-1);
	startToListen(this->port, this->readingQueue, this->writtingQueue, threads);
}

HTTPSocket::~HTTPSocket() {
	delete this->readingQueue;
	delete this->writtingQueue;
}

void HTTPSocket::respond(string body, bool statusOK, int reqID) {
	int status = statusOK ? 200 : 422;
	HTTPSocket::MessageData* rd = new HTTPSocket::MessageData("", body,
			HTTPSocket::RESPONSE, status);
	this->writtingQueue->put(reqID, rd);
}

string HTTPSocket::read() {
	HTTPSocket::MessageData* md;
	this->readingQueue->pop(md);
	string body = md->body;
	delete md;
	return body;
}

void HTTPSocket::stop() {
	pthread_join(this->t, NULL);
	//FIXME memory leaks?
}

//PROTECTED

//PRIVATE

void HTTPSocket::startToListen(string port,
		tbb::concurrent_bounded_queue<HTTPSocket::MessageData*>* readingQueue,
		BlockingMap<int, MessageData*>* writtingQueue, int threads) {
	ListenerArgs* largs = new ListenerArgs(port, readingQueue, writtingQueue,
			threads);
	pthread_create(&this->t, NULL, startListener, (void*) largs);
}

void* HTTPSocket::startListener(void *ptr) {
	ListenerArgs* largs = (ListenerArgs*) ptr;
	HTTPListener* listener = HTTPListener::getInstance(largs->port,
			largs->readingQueue, largs->writtingQueue, largs->threads);
	listener->start();
}

