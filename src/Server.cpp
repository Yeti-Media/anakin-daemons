#include "Server.hpp"

#include <ATCPServerSocket.hpp>
#include <AUDPServerSocket.hpp>
#include <sstream>
#include <CommandRunner.hpp>

using namespace Anakin;
using namespace std;

Server::Server(unsigned short port, bool verbose, char mode) {
    this->port = port;
    this->mode = mode;
    if (mode & TCP) {
        this->server = new ATCPServerSocket(port);
        this->server->setShowComs(verbose);
    } else if (mode & UDP) {
        this->server = new AUDPServerSocket(port);
        this->server->setShowComs(verbose);
    }
    this->verbose = verbose;
}

void Server::start(Flags* flags, DataOutput* output) {
    if (this->mode & TCP || this->mode & UDP) {
        this->socket = this->server->waitForConnection();
        this->socket->setShowComs(this->verbose);
    }
    string msg;
    vector<string> *input = new vector<string>(0);
    do {
        input->clear();
        msg = read();
        if (msg != "stop") {
            cout << "MESSAGE RECEIVED: " << msg << endl;
            stringstream ss_input(msg);
            while(ss_input.good()) {
                string value;
                ss_input >> value;
                input->push_back(value);
            }
            CommandRunner* runner = new CommandRunner(flags, output, input);
            runner->run();
        }
    } while (msg != "stop");
    output->close();
    if (this->mode & TCP || this->mode & UDP) this->server->stopServer();
}

string Server::read() {
    if (this->mode & CONSOLE) {
        string msg = "";
        getline(cin, msg);
        return msg;
    } else {
        return this->socket->read();
    }
}
