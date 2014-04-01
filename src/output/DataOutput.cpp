#include "output/DataOutput.hpp"

using namespace Anakin;
using namespace std;

DataOutput::DataOutput(Socket* s) {
    initSem();
    this->s = s;
    this->consoleOutput = false;
    this->httpOutput = false;
}

DataOutput::DataOutput(HTTPSocket* httpSocket) {
    initSem();
    this->httpSocket = httpSocket;
    this->consoleOutput = false;
    this->httpOutput = true;
}

DataOutput::DataOutput() {
    initSem();
    this->consoleOutput = true;
    this->httpOutput = false;
}

void DataOutput::output(string data, int reqID) {
    sem_wait(&this->ssem);
    if (this->consoleOutput) {
        flush(cout);
        cout << data << endl;
    } else if (this->httpOutput) {
        this->httpSocket->respond(data, true, reqID);
    } else {
        this->s->send(data);
    }
    sem_post(&this->ssem);
}

void DataOutput::output(wstring data, int reqID) {
    sem_wait(&this->wssem);
    string s(data.begin(), data.end());
    output(s,reqID);
    sem_post(&this->wssem);
}

void DataOutput::close() {
    if (!this->consoleOutput) {
        this->s->sendStop();
    }
}

//PRIVATE

void DataOutput::initSem() {
    if (sem_init (&this->ssem, 0, 1) != 0) {
        cout << "DataOutput#initSem: error initializing semaphore\n";
        exit(-1);
    }
    if (sem_init (&this->wssem, 0, 1) != 0) {
        cout << "DataOutput#initSem: error initializing semaphore\n";
        exit(-1);
    }
}
