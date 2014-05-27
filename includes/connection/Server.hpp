#ifndef SERVER_HPP
#define SERVER_HPP

//#include <boost/algorithm/string.hpp>
//#include <boost/regex.hpp>
#include <connection/HTTPSocket.hpp>
#include <db/DBDriver.hpp>
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <output/DataOutput.hpp>
#include <output/JSONValue.h>
#include <processing/SFBMCache.hpp>
#include <sys/types.h>
#include "connection/Server.hpp"
#include "processing/AnakinFlags.hpp"
//#include "processing/CommandRunner.hpp"
#include <cstdlib>
#include <iostream>
//#include <sstream>
#include <string>
#include <vector>

namespace Anakin {

/**
 * Base class for listening to requests
 *
 * this class implement a skeleton algorithm to process messages
 * a redactec version is shown below
 *
 *
 *    startServer();
 *    std::string msg;
 *    bool run = true;
 *    bool stopReceivedInsideInput = false;
 *    do {
 *        msg = read();
 *        if (!stopMessageReceived(msg)) {
 *            std::vector<std::vector<std::string>*>* inputs = getInputs(msg, &stopReceivedInsideInput);
 *            for (uint i = 0; i < inputs->size(); i++) {
 *                execute(inputs->at(i));
 *            }
 *            if (stopReceivedInsideInput) {
 *                stopReceived();
 *                run = false;
 *            }
 *        } else {
 *            stopReceived();
 *            run = false;
 *        }
 *    } while (run);
 *    endServer();
 *    output->close();
 *    [in some modes a stop function will be called for the socket used]
 *
 */
template <class SpecificCommandRunner>
class Server{
public:
	/**
	 * Constructor
	 * port : the port to listen
	 * verbose : if the server will output information to console or not
	 * mode : how to listen to requests (CONSOLE, TCP, UDP, DTCP, HTTP)
	 */
	Server(CacheConfig * cacheConfig,
			char mode, string pghost, string pgport,
			string dbName, string login, string pwd, unsigned int httpPort, bool verbose);
	/**
	 * this will start the skeleton algorithm shown above
	 * aflags : this will check the message and validate that have the required flags and values
	 * output : this is used to output the processing results
	 */
	void start(AnakinFlags* aflags, DataOutput* output);
	static const char CONSOLE = 1;
	static const char TCP = 2;
	static const char UDP = 4;
	static const char DTCP = 8;
	static const char HTTP = 16;
	/**
	 * Because HTTPSocket can't be used as a client the
	 * socket created by the server is the same used to
	 * respond.
	 */
	HTTPSocket* getHttpSocket();

	virtual ~Server();
protected:
	/**
	 * will read a message from the right input (socket, console, httpsocket)
	 */
	std::string read();
	/**
	 * this is used to support several requests on the same message
	 * the only mode that supports this is DTCP, all others will send
	 * one request per message
	 */
	std::vector<std::vector<std::string>*>* getInputs(std::string rawInput,
			bool * stopReceivedInsideInput);
	/**
	 * because the input is validated as a vector of strings, the original message
	 * must be transformed from one string to a vector
	 */
	std::vector<std::string>* rawToInput(std::string rawInput);
	/**
	 * this will process an input
	 */
	virtual void execute(std::vector<std::string>* input);
	/**
	 * this will execute when receiving a stop message
	 * this is the last function to be called by stopReceived
	 */
	virtual void executeStop();
	/**
	 * will return true if rawMsg is a stop message
	 */
	virtual bool stopMessageReceived(std::string rawMsg);

	/**
	 * this is used to execute anything needed before starting to receiving messages
	 */
	virtual void startServer();
	/**
	 * anything that needs to be done before closing all connections
	 */
	virtual void endServer();
	/**
	 * called after receiving a stop message
	 */
	void stopReceived();
	unsigned short port;
	//ServerSocket* server;
	bool verbose;
	//Socket* socket;
	HTTPSocket* httpSocket;
	char mode;
	AnakinFlags* aflags;
	DataOutput* output;
	SFBMCache* cache;
	DBDriver* dbdriver;

	bool initialization = false;
	std::string initializationError;
	bool cacheInitializationError;
	std::wstring cacheInitializationErrorMsj;
private:
};

template <class SpecificCommandRunner>
Server<SpecificCommandRunner>::Server(CacheConfig * cacheConfig,
		char mode, string pghost, string pgport,
		string dbName, string login, string pwd,  unsigned int httpPort, bool verbose) {
	this->aflags = NULL;
	this->output = NULL;
	this->port = httpPort;
	this->mode = mode;
	this->dbdriver = new DBDriver();
	if (this->dbdriver->connect(pghost, pgport, dbName, login, pwd)) {
		this->initialization = true;
		std::cout << this->dbdriver->getMessage() << std::endl;
		LOG_F("INFO")<< this->dbdriver->getMessage();
	} else {
		this->initialization = false;
		this->initializationError = this->dbdriver->getMessage();
		LOG_F("ERROR") << this->dbdriver->getMessage();
		std::cout << this->dbdriver->getMessage() << std::endl;
		exit(EXIT_FAILURE);
	}
	if (this->initialization) {
		this->cache = new SFBMCache(this->dbdriver, cacheConfig);
		this->cacheInitializationError = false;
		JSONValue* cacheError = this->cache->getLastOperationResult(
				&this->cacheInitializationError);
		if (this->cacheInitializationError) {
			this->cacheInitializationErrorMsj = cacheError->Stringify().c_str();
			LOG_F("ERROR")<< this->cacheInitializationErrorMsj.c_str();
		}
	}
	if (mode & HTTP) {
		std::string sport = std::to_string(port);
		this->httpSocket = new HTTPSocket(sport, 15);
		this->httpSocket->setShowComs(verbose);
	}
	this->verbose = verbose;
}

template <class SpecificCommandRunner>
void Server<SpecificCommandRunner>::start(AnakinFlags* aflags, DataOutput* output) {
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
			std::vector<std::vector<std::string>*>* inputs = getInputs(msg,
					&stopReceivedInsideInput);
			if (stopReceivedInsideInput) {
				stopReceived();
				run = false;
			} else {
				for (uint i = 0; i < inputs->size(); i++) {
					execute(inputs->at(i));
				}
			}
		} else {
			stopReceived();
			run = false;
		}
	} while (run);
	endServer();
	output->close();
	if (this->mode & HTTP)
		this->httpSocket->stop();
}

template <class SpecificCommandRunner>
HTTPSocket* Server<SpecificCommandRunner>::getHttpSocket() {
	return this->httpSocket;
}

//PROTECTED

template <class SpecificCommandRunner>
std::string Server<SpecificCommandRunner>::read() {
	if (this->mode & CONSOLE) {
		std::string msg = "";
		getline(cin, msg);
		return msg;
	} else if (this->mode & HTTP) {
		std::string msj = this->httpSocket->read();
		LOG_F("Request")<< msj;
		return msj;
	}
	cout << "Server::read(): mode not recognized = " << this->mode << endl;
	return "-stop";
}

template <class SpecificCommandRunner>
std::vector<std::vector<std::string>*>* Server<SpecificCommandRunner>::getInputs(std::string rawInput,
		bool * stopReceivedInsideInput) {
	std::vector<std::vector<std::string>*>* inputs = new std::vector<
			std::vector<std::string>*>(0);
	std::string msg = rawInput;

	if (Server::stopMessageReceived(rawInput)) {
		*stopReceivedInsideInput = true;
	}
	inputs->push_back(rawToInput(rawInput));

	return inputs;
}
template <class SpecificCommandRunner>
std::vector<std::string>* Server<SpecificCommandRunner>::rawToInput(std::string rawInput) {
	std::vector<std::string> *input = new std::vector<std::string>(0);
	stringstream ss_input(rawInput);
	while (ss_input.good()) {
		std::string value;
		ss_input >> value;
		input->push_back(value);
	}
	return input;
}

template <class SpecificCommandRunner>
void Server<SpecificCommandRunner>::execute(std::vector<std::string>* input) {
//	CommandRunner* runner = new CommandRunner(this->aflags->getFlags(),
//			this->output, this->cache, input);
//	runner->run();
}


template <class SpecificCommandRunner>
bool Server<SpecificCommandRunner>::stopMessageReceived(std::string rawMsg) {
	return (rawMsg.find("-stop") != std::string::npos)
			|| (rawMsg.find("\"action\":\"stop\"") != std::string::npos);
}

/**
 * placeholder for inheritance
 */
template <class SpecificCommandRunner>
void Server<SpecificCommandRunner>::executeStop() {
}

/**
 * placeholder for inheritance
 */
template <class SpecificCommandRunner>
void Server<SpecificCommandRunner>::endServer() {
}

/**
 * placeholder for inheritance
 */
template <class SpecificCommandRunner>
void Server<SpecificCommandRunner>::startServer() {
}

template <class SpecificCommandRunner>
void Server<SpecificCommandRunner>::stopReceived() {
	std::cout << "STOP MESSAGE RECEIVED" << std::endl;
	executeStop();
}

template <class SpecificCommandRunner>
Server<SpecificCommandRunner>::~Server() {
}

}
; // namespace Anakin

#endif // SERVER_HPP
