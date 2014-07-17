#ifndef SERVER_HPP
#define SERVER_HPP

#include <connection/HTTPSocket.hpp>
#include <db/DBDriver.hpp>
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <output/DataOutput.hpp>
#include <processing/SFBMCache.hpp>
#include <sys/types.h>
#include "connection/Server.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace Anakin {

template<class SpecificCommandRunner>
class Server {
public:
	/**
	 * Constructor
	 * port : the port to listen
	 * verbose : if the server will output information to console or not
	 * mode : how to listen to requests (CONSOLE, TCP, UDP, DTCP, HTTP)
	 */
	Server(CacheConfig * cacheConfig, char mode, string pghost, string pgport,
			string dbName, string login, string pwd, unsigned int httpPort,
			bool verbose);
	/**
	 * this will start the skeleton algorithm shown above
	 * output : this is used to output the processing results
	 */
	void start(DataOutput* output);
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
	string read();
	/**
	 * this is used to support several requests on the same message
	 * the only mode that supports this is DTCP, all others will send
	 * one request per message
	 */
	vector<vector<string>*>* getInputs(string rawInput,
			bool * stopReceivedInsideInput);
	/**
	 * because the input is validated as a vector of strings, the original message
	 * must be transformed from one string to a vector
	 */
	vector<string>* rawToInput(string rawInput);
	/**
	 * this will process an input
	 */
	virtual void execute(vector<string>* input);
	/**
	 * this will execute when receiving a stop message
	 * this is the last function to be called by stopReceived
	 */
	virtual void executeStop();
	/**
	 * will return true if rawMsg is a stop message
	 */
	virtual bool stopMessageReceived(string rawMsg);

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
	bool verbose;
	HTTPSocket* httpSocket;
	char mode;
	DataOutput* output;
	SFBMCache* cache;
	DBDriver* dbdriver;

	bool initialization = false;
	string initializationError;
	bool cacheInitializationError;
	wstring cacheInitializationErrorMsj;
private:
};

template<class SpecificCommandRunner>
Server<SpecificCommandRunner>::Server(CacheConfig * cacheConfig, char mode,
		string pghost, string pgport, string dbName, string login, string pwd,
		unsigned int httpPort, bool verbose) {
	this->output = NULL;
	this->cache = NULL;
	this->port = httpPort;
	this->mode = mode;
	this->dbdriver = new DBDriver();
	if (this->dbdriver->connect(pghost, pgport, dbName, login, pwd)) {
		this->initialization = true;
		cout << this->dbdriver->getMessage() << endl;
		LOG_F("INFO")<< this->dbdriver->getMessage();
	} else {
		this->initialization = false;
		this->initializationError = this->dbdriver->getMessage();
		LOG_F("ERROR") << this->dbdriver->getMessage();
		cout << this->dbdriver->getMessage() << endl;
		exit(EXIT_FAILURE);
	}
	if (this->initialization) {
		this->cache = new SFBMCache(this->dbdriver, cacheConfig);
		this->cacheInitializationError = false;
		wstring cacheError = *this->cache->getLastOperationResult(
				&this->cacheInitializationError);
		if (this->cacheInitializationError) {
			this->cacheInitializationErrorMsj = cacheError;
			LOG_F("ERROR")<< this->cacheInitializationErrorMsj.c_str();
		}
	}
	if (mode & HTTP) {
		string sport = to_string(port);
		this->httpSocket = new HTTPSocket(sport, 15);
		this->httpSocket->setShowComs(verbose);
	}
	this->verbose = verbose;
}

template<class SpecificCommandRunner>
void Server<SpecificCommandRunner>::start(DataOutput* output) {
	this->output = output;
	startServer();
	string msg;
	bool run = true;
	bool stopReceivedInsideInput = false;
	LOG_F("INFO")<< "* Server started *";
	do {
		msg = read();
		if (!stopMessageReceived(msg)) {
			cout << "MESSAGE RECEIVED:" << endl << msg << endl;
			vector<vector<string>*>* inputs = getInputs(msg,
					&stopReceivedInsideInput);
			if (stopReceivedInsideInput) {
				stopReceived();
				run = false;
			} else {
				for (uint i = 0; i < inputs->size(); i++) {
					execute(inputs->at(i));
				}
			}
			delete inputs;
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

template<class SpecificCommandRunner>
HTTPSocket* Server<SpecificCommandRunner>::getHttpSocket() {
	return this->httpSocket;
}

//PROTECTED

template<class SpecificCommandRunner>
string Server<SpecificCommandRunner>::read() {
	if (this->mode & CONSOLE) {
		string msg = "";
		getline(cin, msg);
		return msg;
	} else if (this->mode & HTTP) {
		string msj = this->httpSocket->read();
		LOG_F("Request")<< msj;
		return msj;
	}
	cout << "Server::read(): mode not recognized = " << this->mode << endl;
	return "-stop";
}

template<class SpecificCommandRunner>
vector<vector<string>*>* Server<SpecificCommandRunner>::getInputs(
		string rawInput, bool * stopReceivedInsideInput) {
	vector<vector<string>*>* inputs = new vector<vector<string>*>(0);
	string msg = rawInput;

	if (Server::stopMessageReceived(rawInput)) {
		*stopReceivedInsideInput = true;
	}
	inputs->push_back(rawToInput(rawInput));

	return inputs;
}

template<class SpecificCommandRunner>
vector<string>* Server<SpecificCommandRunner>::rawToInput(string rawInput) {
	vector<string> *input = new vector<string>(0);
	stringstream ss_input(rawInput);
	while (ss_input.good()) {
		string value;
		ss_input >> value;
		input->push_back(value);
	}
	return input;
}

template<class SpecificCommandRunner>
void Server<SpecificCommandRunner>::execute(vector<string>* input) {
//  use for non request servers
//	CommandRunner* runner = new CommandRunner(this->aflags->getFlags(),
//			this->output, this->cache, input);
//	runner->run();
}

template<class SpecificCommandRunner>
bool Server<SpecificCommandRunner>::stopMessageReceived(string rawMsg) {
	return (rawMsg.find("-stop") != string::npos)
			|| (rawMsg.find("\"action\":\"stop\"") != string::npos);
}

/**
 * placeholder for inheritance
 */
template<class SpecificCommandRunner>
void Server<SpecificCommandRunner>::executeStop() {
}

/**
 * placeholder for inheritance
 */
template<class SpecificCommandRunner>
void Server<SpecificCommandRunner>::endServer() {
}

/**
 * placeholder for inheritance
 */
template<class SpecificCommandRunner>
void Server<SpecificCommandRunner>::startServer() {
}

template<class SpecificCommandRunner>
void Server<SpecificCommandRunner>::stopReceived() {
	cout << "STOP MESSAGE RECEIVED" << endl;
	executeStop();
}

template<class SpecificCommandRunner>
Server<SpecificCommandRunner>::~Server() {
	delete dbdriver;
	if (this->cache != NULL) {
		delete this->cache;
	}
	if (this->cache != NULL) {
		delete this->httpSocket;
	}
}

}
;
// namespace Anakin

#endif // SERVER_HPP
