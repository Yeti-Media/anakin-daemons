#include "connection/Server.hpp"

#include "connection/ATCPServerSocket.hpp"
#include "connection/AUDPServerSocket.hpp"
#include <sstream>
#include "processing/CommandRunner.hpp"
#include "connection/DTCPServerSocket.hpp"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

using namespace Anakin;
using namespace std;

Server::Server(unsigned short port, bool verbose, char mode, std::string ld, std::string md) {
    this->port = port;
    this->mode = mode;
    this->dbdriver = new DBDriver();
    if (this->dbdriver->connect()) {
        std::cout << this->dbdriver->lastMessageReceived << std::endl;
    } else {
        std::cerr << this->dbdriver->lastMessageReceived << std::endl;
        exit(-1);
    }
    this->cache = new SFBMCache(this->dbdriver, 9, true, 10);
    if (mode & TCP) {
        this->server = new ATCPServerSocket(port);
        this->server->setShowComs(verbose);
    } else if (mode & UDP) {
        this->server = new AUDPServerSocket(port);
        this->server->setShowComs(verbose);
    } else if (mode & DTCP) {
        this->server = new DTCPServerSocket(port, ld, md);
        this->server->setShowComs(verbose);
    } else if (mode & HTTP) {
        std::string sport = std::to_string(port);
        this->httpSocket = new HTTPSocket(sport, 15);
        this->httpSocket->setShowComs(verbose);
    }
    this->verbose = verbose;
}

void Server::start(AnakinFlags* aflags, DataOutput* output) {
    if (this->mode & TCP || this->mode & UDP || this->mode & DTCP) {
        this->socket = this->server->waitForConnection();
        this->socket->setShowComs(this->verbose);
    }
    this->aflags = aflags;
    this->output = output;
    startServer();
    std::string msg;
    bool run = true;
    bool stopReceivedInsideInput = false;
    do {
        msg = read();
        if (!stopMessageReceived(msg)) {
            std::cout << "MESSAGE RECEIVED:\n" << msg << std::endl;
            std::vector<std::vector<std::string>*>* inputs = getInputs(msg, &stopReceivedInsideInput);
            for (uint i = 0; i < inputs->size(); i++) {
                execute(inputs->at(i));
            }
            if (stopReceivedInsideInput) {
                stopReceived();
                run = false;
            }
        } else {
            stopReceived();
            run = false;
        }
    } while (run);
    endServer();
    output->close();
    if (this->mode & TCP || this->mode & UDP || this->mode & DTCP) this->server->stopServer();
    if (this->mode & HTTP) this->httpSocket->stop();
}

HTTPSocket* Server::getHttpSocket() {
    return this->httpSocket;
}

//PROTECTED

string Server::read() {
    if (this->mode & CONSOLE) {
        string msg = "";
        getline(cin, msg);
        return msg;
    } else if (this->mode & HTTP) {
        return this->httpSocket->read();
    } else {
        return this->socket->read();
    }
}

std::vector<std::vector<std::string>*>* Server::getInputs(std::string rawInput, bool * stopReceivedInsideInput) {
    std::vector<std::vector<std::string>*>* inputs = new std::vector<std::vector<std::string>*>(0);
    std::string msg = rawInput;
    if (this->mode & DTCP) {
        std::string ld = ((DTCPServerSocket*)this->server)->getLineDelimiter();
        std::string md = ((DTCPServerSocket*)this->server)->getMessageDelimiter();
        boost::regex ldRx(ld);
        boost::regex mdRx(md);
        std::string ldfmt("\n");
        std::string mdfmt("");
        msg = boost::regex_replace(msg, ldRx, ldfmt, boost::match_default | boost::format_all);
        msg = boost::regex_replace(msg, mdRx, mdfmt, boost::match_default | boost::format_all);
        std::istringstream iss(msg);
        std::string line;
        while (std::getline(iss, line)) {
            boost::algorithm::trim_right(line);
            if (stopMessageReceived(line)) {
                *stopReceivedInsideInput = true;
                break;
            }
            inputs->push_back(rawToInput(line));
        }
    } else {
        inputs->push_back(rawToInput(rawInput));
    }
    return inputs;
}

std::vector<std::string>* Server::rawToInput(std::string rawInput) {
    std::vector<std::string> *input = new std::vector<std::string>(0);
    stringstream ss_input(rawInput);
    while(ss_input.good()) {
        std::string value;
        ss_input >> value;
        input->push_back(value);
    }
    return input;
}

void Server::execute(std::vector<std::string>* input) {
    CommandRunner* runner = new CommandRunner(this->aflags->getFlags(), this->output, this->cache, input);
    runner->run();
}

void Server::executeStop() {}

bool Server::stopMessageReceived(std::string rawMsg) {
    return rawMsg == "stop";
}

void Server::endServer() {}

void Server::startServer() {}

void Server::stopReceived() {
    std::cout << "STOP MESSAGE RECEIVED" << std::endl;
    executeStop();
}
