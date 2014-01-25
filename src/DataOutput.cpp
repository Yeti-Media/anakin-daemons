#include "DataOutput.hpp"

using namespace Anakin;
using namespace std;

DataOutput::DataOutput(Socket* s) {
    initSem();
    this->s = s;
    this->consoleOutput = false;
}

DataOutput::DataOutput() {
    initSem();
    this->consoleOutput = true;
}

void DataOutput::output(string data) {
    sem_wait(&this->ssem);
    if (this->consoleOutput) {
        flush(cout);
        cout << data << endl;
    } else {
        this->s->send(data);
    }
    sem_post(&this->ssem);
}

void DataOutput::output(wstring data) {
    sem_wait(&this->wssem);
    string s(data.begin(), data.end());
    output(s);
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
