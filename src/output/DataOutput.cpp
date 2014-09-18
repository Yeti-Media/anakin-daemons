#include <connection/HTTPSocket.hpp>
#include <output/DataOutput.hpp>
#include <output/workers/DataOutputWorkerConsole.hpp>
#include <output/workers/DataOutputWorkerHTTPSocket.hpp>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <utils/ClearVector.hpp>
#include <boost/locale.hpp>

using namespace Anakin;
using namespace std;

DataOutput::DataOutput(const Ptr<HTTPSocket> & httpSocket) {
	workingQueue = new BlockingQueue<Msj*>();
	WorkerArgs* wargs = new WorkerArgs(E_DataOutputType::http, httpSocket,
			workingQueue);
	pthread_create(&this->workerThread, NULL, startWorker, (void *) wargs);
}

DataOutput::DataOutput() {
	workingQueue = new BlockingQueue<Msj*>();
	Ptr<HTTPSocket> nullHTTPSocket;
	WorkerArgs* wargs = new WorkerArgs(E_DataOutputType::console, nullHTTPSocket,
			workingQueue);
	pthread_create(&this->workerThread, NULL, startWorker, (void *) wargs);
}

void* DataOutput::startWorker(void *ptr) {
	WorkerArgs* wargs = (WorkerArgs*) ptr;
	DataOutputWorker* worker = NULL;
	switch (wargs->outputType) {
	case console: {
		worker = new DataOutputWorkerConsole(wargs->workingQueue);
		break;
	}
	case http: {
		worker = new DataOutputWorkerHTTPSocket(wargs->workingQueue,
				wargs->httpSocket);
		break;
	}
	default: {
		cerr << "unknown DataOutputWotker";
		exit(EXIT_FAILURE);
		break;
	}
	}

	if (worker == NULL) {
		cerr << "unknown DataOutputWotker implementation";
		exit(EXIT_FAILURE);
	}
	worker->start();
	delete worker;
	delete wargs;
}

DataOutput::~DataOutput() {
	close();
	delete workingQueue;
}

void DataOutput::output(const Ptr<wstring> &data, int reqID) {
	lock_guard<mutex> lck(outputMutex);
	Msj* msj = new Msj(boost::locale::conv::utf_to_utf<char>(*data),
			E_DataOutputMsjType::common, reqID);
	workingQueue->push(msj);
}

void DataOutput::error(const Ptr<wstring> & data) {
	lock_guard<mutex> lck(outputMutex);
	Msj* msj = new Msj(boost::locale::conv::utf_to_utf<char>(*data),
			E_DataOutputMsjType::error);
	workingQueue->push(msj);
}

void DataOutput::close() {
	workingQueue->push(NULL);
	pthread_join(this->workerThread, NULL);
}
