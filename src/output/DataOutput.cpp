#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <output/DataOutput.hpp>
#include <mutex>
#include <string>

using namespace Anakin;
using namespace std;

DataOutput::DataOutput(HTTPSocket* httpSocket) {
	workingQueue = new BlockingQueue<Msj*>();
	WorkerArgs* wargs = new WorkerArgs(E_DataOutputType::http, httpSocket,
			workingQueue);
	pthread_create(&this->workerThread, NULL, startWorker, (void *) wargs);
}

DataOutput::DataOutput() {
	workingQueue = new BlockingQueue<Msj*>();
	WorkerArgs* wargs = new WorkerArgs(E_DataOutputType::console, NULL,
			workingQueue);
	pthread_create(&this->workerThread, NULL, startWorker, (void *) wargs);
}

void* DataOutput::startWorker(void *ptr) {
	WorkerArgs* wargs = (WorkerArgs*) ptr;
	DataOutputWorker* worker = new DataOutputWorker(wargs->outputType,
			wargs->httpSocket, wargs->workingQueue);
	worker->start();
}

DataOutput::~DataOutput() {
	delete workingQueue;
}
void DataOutput::output(string data, int reqID) {
	lock_guard<mutex> lck(mutex1);
	Msj* msj = new Msj(data, E_DataOutputMsjType::common, reqID);
	workingQueue->push(msj);
}

void DataOutput::output(wstring data, int reqID) {
	lock_guard<mutex> lck(mutex2);
	string s(data.begin(), data.end());
	output(s, reqID);
}

void DataOutput::error(string data) {
	lock_guard<mutex> lck(mutex1);
	Msj* msj = new Msj(data, E_DataOutputMsjType::error);
	workingQueue->push(msj);
}

void DataOutput::error(wstring data) {
	lock_guard<mutex> lck(mutex2);
	string s(data.begin(), data.end());
	error(s);
}

void DataOutput::close() {
	workingQueue->push(NULL);
	pthread_join(this->workerThread, NULL);
}
