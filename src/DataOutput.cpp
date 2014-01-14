#include "DataOutput.hpp"

using namespace Anakin;
using namespace std;

DataOutput::DataOutput(Socket* s) {
    this->s = s;
    this->consoleOutput = false;
}

DataOutput::DataOutput() {
    this->consoleOutput = true;
}

void DataOutput::output(string data) {
    if (this->consoleOutput) {
        cout << data << endl;
    } else {
        this->s->send(data);
    }
}

void DataOutput::output(wstring data) {
    string s(data.begin(), data.end());
    output(s);
}

void DataOutput::close() {
    if (!this->consoleOutput) {
        this->s->send("stop");
    }
}
