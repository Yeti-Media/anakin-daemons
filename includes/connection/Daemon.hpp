/*
 * Daemon.h
 *
 *  Created on: 16/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef DAEMON_H_
#define DAEMON_H_

#include <connection/RequestServer.hpp>
#include <connection/Server.hpp>
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <output/DataOutput.hpp>
#include <processing/AnakinFlags.hpp>
#include <processing/Flags.hpp>
#include <processing/SFBMCache.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace Anakin {

template<class SpecificCommandRunner>
class Daemon {
public:
	Daemon();

	/**
	 * init the flags with testing additions (see testing README for mor info)
	 */
	static void initTestingFlags(Flags* flags);

	/**
	 * start the daemon
	 */
	void start(int argc, const char * argv[], bool useCache = false);

	virtual ~Daemon();
};

template<class SpecificCommandRunner>
Daemon<SpecificCommandRunner>::Daemon() {
	// TODO Auto-generated constructor stub

}

template<class SpecificCommandRunner>
void Daemon<SpecificCommandRunner>::initTestingFlags(Flags* flags) {
#if COMPILE_MODULE == ALLMODULES
	flags->setNoValuesFlag("modepatternmatching");
	flags->setNoValuesFlag("modematchercache");
	flags->setNoValuesFlag("modedbconnector");
	flags->setNoValuesFlag("modeextractor");
	flags->setNoValuesFlag("modetrainer");

	flags->setIncompatibility("modepatternmatching", "modematchercache");
	flags->setIncompatibility("modepatternmatching", "modedbconnector");
	flags->setIncompatibility("modepatternmatching", "modeextractor");
	flags->setIncompatibility("modepatternmatching", "modetrainer");

	flags->setIncompatibility("modematchercache", "modedbconnector");
	flags->setIncompatibility("modematchercache", "modeextractor");
	flags->setIncompatibility("modematchercache", "modetrainer");

	flags->setIncompatibility("modedbconnector", "modeextractor");
	flags->setIncompatibility("modedbconnector", "modetrainer");

	flags->setIncompatibility("modeextractor", "modetrainer");
#endif
}

template<class SpecificCommandRunner>
void Daemon<SpecificCommandRunner>::start(int argc, const char * argv[],
		bool useCache) {
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);
	cerr.tie(nullptr);
	char iMode = Server<SpecificCommandRunner>::CONSOLE;
	char oMode = Server<SpecificCommandRunner>::CONSOLE;
	unsigned short portIn = 18003;
	string logFile = "";
	unsigned int threads = 4;
	unsigned int queueCapacity = 10;
	bool verbose = false;

	CacheConfig cacheConfig;
	CacheConfig * pCacheConfig = &cacheConfig;

	//______________________________________________________________________//
	//                                 FLAGS                                //
	//______________________________________________________________________//

	vector<string> *input = new vector<string>(0);
	for (int i = 1; i < argc; i++) {
		input->push_back(argv[i]);
	}

	Flags* anakinInput = new Flags();

	Daemon<SpecificCommandRunner>::initTestingFlags(anakinInput);

	anakinInput->setOverridingFlag("help");
	anakinInput->setNoValuesFlag("verbose");

	//GENERAL CACHE CONFIG
	if (useCache) {
		anakinInput->setOptionalFlag("cacheLoadingTimeWeight");
		anakinInput->setNoValuesFlag("cacheDiscardLessValuable");
		anakinInput->setOptionalFlag("cacheSize");
		anakinInput->setOptionalFlag("cacheLife");
		anakinInput->setOptionalFlag("cacheScenesSize");
		anakinInput->setOptionalFlag("cacheScenesLife");
	}

	//GENERAL CONFIGS
	anakinInput->setOptionalFlag("threads");
	anakinInput->setOptionalFlag("queueCapacity");

	//INPUT
	anakinInput->setNoValuesFlag("iConsole");
	anakinInput->setOptionalFlag("iHTTP");
	anakinInput->setIncompatibility("iConsole", "iHTTP");

	//OUTPUT
	anakinInput->setNoValuesFlag("oConsole");
	anakinInput->setOptionalFlag("oLogFile");
	anakinInput->setNoValuesFlag("oHTTP");
	anakinInput->setIncompatibility("oHTTP", "oConsole");
	anakinInput->setDependence("iHTTP", "oHTTP");
	anakinInput->setDependence("oHTTP", "iHTTP");

	anakinInput->setMinCount(2);
	anakinInput->setVerbose(true);

	//______________________________________________________________________//
	//                         FLAGS PARSING                                //
	//______________________________________________________________________//

	if (anakinInput->validateInput(input)) {
		if (anakinInput->flagFound("help")) {
			//NOTE that every concrete commandrunner must have a static member called "help".
			// See PatternMatchingCommandRunner as an example
			cout << "Daemon arguments:" << endl
					<< "[cacheLoadingTimeWeight <int>|cacheDiscardLessValuable <bool>|cacheSize <int>|cacheLife <int>|cacheScenesSize <int>|cacheScenesLife <int>|-oLogFile <path>|-threads <int>|-queueCapacity <int>] (-iConsole|(-iHTTP <port>)) (-oConsole|-oHTTP)"
					<< endl << endl
					<< "-iConsole/oConsole			: use console to input or output respectively"
					<< endl << "-oLogFile <path>			: path to the output logging file. If it's ignored, logging will be skipped."
					<< endl
					<< "-iHTTP <port>/oHTTP			: use a HTTP connection with a port for input"
					<< endl
					<< "-cacheLoadingTimeWeight <int>		: (default 9) how many importance the loading time of a trainer will influence on his life"
					<< endl
					<< "-cacheNoDiscardLessValuable <bool>	: (default not defined = true) if a trainer is loaded from the db whose life is less than the object to be dropped then the new object will not be stored in the cache"
					<< endl
					<< "-cacheSize <int>			: (default 10) trainers cache size (how many trainers can be stored in the cache)"
					<< endl
					<< "-cacheLife <int>			: (default 1) trainers starting life"
					<< endl
					<< "-cacheScenesSize <int>			: (default 15) scenes cache size (how many scenes can be stored in the cache)"
					<< endl
					<< "-cacheScenesLife <int>			: (default 10) scenes starting life"
					<< endl
					<< "-threads <int>				: (default 4) threads to use at processing requests"
					<< endl
					<< "-queueCapacity <int>			: (default 10) processing queue max capacity"
					<< endl << endl
					<< SpecificCommandRunner::help->getFullHelp();
			delete SpecificCommandRunner::help;
			exit(EXIT_SUCCESS);
		}
		if (anakinInput->flagFound("verbose")) {
			verbose = true;
		}
		vector<string>* values;

		//CACHE CONFIG
		if (useCache) {
			if (anakinInput->flagFound("cacheLoadingTimeWeight")) {
				values = anakinInput->getFlagValues("cacheLoadingTimeWeight");
				if (values->size() == 1) {
					cacheConfig.cacheLoadingTimeWeight = stoi(values->at(0));
				} else {
					cout << "param cacheLoadingTimeWeight needs only one value"
							<< endl;
					exit(EXIT_FAILURE);
				}
			}

			if (anakinInput->flagFound("cacheNoDiscardLessValuable")) {
				cacheConfig.cacheNoDiscardLessValuable = true;
			}

			if (anakinInput->flagFound("cacheSize")) {
				values = anakinInput->getFlagValues("cacheSize");
				if (values->size() == 1) {
					cacheConfig.cacheSize = stoi(values->at(0));
				} else {
					cout << "param cacheSize needs only one value" << endl;
					exit(EXIT_FAILURE);
				}
			}

			if (anakinInput->flagFound("cacheLife")) {
				values = anakinInput->getFlagValues("cacheLife");
				if (values->size() == 1) {
					cacheConfig.cacheLife = stoi(values->at(0));
				} else {
					cout << "param cacheLife needs only one value" << endl;
					exit(EXIT_FAILURE);
				}
			}

			if (anakinInput->flagFound("cacheScenesSize")) {
				values = anakinInput->getFlagValues("cacheScenesSize");
				if (values->size() == 1) {
					cacheConfig.cacheScenesSize = stoi(values->at(0));
				} else {
					cout << "param cacheScenesSize needs only one value"
							<< endl;
					exit(EXIT_FAILURE);
				}
			}

			if (anakinInput->flagFound("cacheScenesLife")) {
				values = anakinInput->getFlagValues("cacheScenesLife");
				if (values->size() == 1) {
					cacheConfig.cacheScenesLife = stoi(values->at(0));
				} else {
					cout << "param cacheScenesLife needs only one value"
							<< endl;
					exit(EXIT_FAILURE);
				}
			}
		}

		//THREADS
		if (anakinInput->flagFound("threads")) {
			values = anakinInput->getFlagValues("threads");
			if (values->size() == 1) {
				threads = stoi(values->at(0));
			} else {
				cout << "param threads needs only one value" << endl;
				exit(EXIT_FAILURE);
			}
		}

		//BLOCKING CAPACITY QUEUE
		if (anakinInput->flagFound("queueCapacity")) {
			values = anakinInput->getFlagValues("queueCapacity");
			if (values->size() == 1) {
				queueCapacity = stoi(values->at(0));
			} else {
				cout << "param queueCapacity needs only one value" << endl;
				exit(EXIT_FAILURE);
			}
		}

		//INPUT
		if (anakinInput->flagFound("iConsole")) {
			iMode = Server<SpecificCommandRunner>::CONSOLE;
		}

		if (anakinInput->flagFound("iHTTP")) {
			iMode = Server<SpecificCommandRunner>::HTTP;
			values = anakinInput->getFlagValues("iHTTP");
			if (values->size() == 1) {
				portIn = stoi(values->at(0));
			} else {
				cout << "param iHTTP needs only one value" << endl;
				exit(EXIT_FAILURE);
			}
		}

		//OUTPUT
		if (anakinInput->flagFound("oConsole")) {
			oMode = Server<SpecificCommandRunner>::CONSOLE;
		}
		if (anakinInput->flagFound("oLogFile")) {
			values = anakinInput->getFlagValues("oLogFile");
			if (values->size() == 1) {
				logFile = values->at(0);
			} else {
				cout << "param oLogPath needs one value" << endl;
				exit(EXIT_FAILURE);
			}
		}

		if (anakinInput->flagFound("oHTTP")) {
			oMode = Server<SpecificCommandRunner>::HTTP;
		}
	} else {
		cout << "Input error!" << endl;
		exit(EXIT_FAILURE);
	}

	//______________________________________________________________________//
	//                         DAEMON INITIALIZATION                        //
	//______________________________________________________________________//

	//help cleanup
	delete SpecificCommandRunner::help;

	//logger initialization
	if (!logFile.empty()) {
		Logging::OutputPolicyFile::SetFileStream(logFile);
	}

	std::string startComand;
	for (int i = 1; i < argc; i++) {
		startComand = startComand + " " + argv[i];
	}
	LOG_F("Args")<< startComand;

	HTTPSocket* httpSocket;

	AnakinFlags* aflags = new AnakinFlags();

	if (!useCache) {
		pCacheConfig = NULL;
	}

	Server<SpecificCommandRunner>* server = new RequestServer<
			SpecificCommandRunner>(pCacheConfig, portIn, queueCapacity, threads,
			verbose, iMode);

	DataOutput* output;
	if (oMode & Server<SpecificCommandRunner>::CONSOLE) {
		output = new DataOutput();
	} else if (oMode & Server<SpecificCommandRunner>::HTTP) {
		httpSocket = server->getHttpSocket();
		output = new DataOutput(httpSocket);
	} else {
		cerr << "unknown output mode \"" << oMode << "\"" << endl;
		exit(EXIT_FAILURE);
	}

	server->start(aflags, output);

	delete httpSocket;
	delete aflags;
	delete server;
}

template<class SpecificCommandRunner>
Daemon<SpecificCommandRunner>::~Daemon() {
}

}
;
/* namespace Anakin */

#endif /* DAEMON_H_ */
