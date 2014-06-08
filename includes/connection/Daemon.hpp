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
	 * start the daemon
	 */
	int start(vector<string> *input, bool useCache = false);

	virtual ~Daemon();
};

template<class SpecificCommandRunner>
Daemon<SpecificCommandRunner>::Daemon() {
	// TODO Auto-generated constructor stub

}

template<class SpecificCommandRunner>
int Daemon<SpecificCommandRunner>::start(vector<string> *input,
		bool useCache) {
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);
	cerr.tie(nullptr);
	char iMode = Server<SpecificCommandRunner>::CONSOLE;
	char oMode = Server<SpecificCommandRunner>::CONSOLE;

	unsigned int httpPort = 18003;

	string pghost = "";
	string pgport = "";
	string dbName = "";
	string login = "";
	string pwd = "";

	string logFile = "";
	unsigned int threads = 4;
	unsigned int queueCapacity = 10;
	bool verbose = false;

	CacheConfig cacheConfig;
	CacheConfig * pCacheConfig = &cacheConfig;

	//______________________________________________________________________//
	//                                 FLAGS                                //
	//______________________________________________________________________//

	Flags* anakinInput = new Flags();

	Flags::initTestingFlags(anakinInput);

	anakinInput->setOverridingFlag("help");
	anakinInput->setNoValuesFlag("verbose");

	//SERVER CONFIGS
	anakinInput->setOptionalFlag("pghost");
	anakinInput->setOptionalFlag("pgport");
	anakinInput->setOptionalFlag("dbName");
	anakinInput->setOptionalFlag("login");
	anakinInput->setOptionalFlag("pwd");

	anakinInput->setDependence("pghost", "pgport");
	anakinInput->setDependence("pghost", "dbName");
	anakinInput->setDependence("pghost", "login");
	anakinInput->setDependence("pghost", "pwd");

	anakinInput->setDependence("pgport", "pghost");
	anakinInput->setDependence("pgport", "dbName");
	anakinInput->setDependence("pgport", "login");
	anakinInput->setDependence("pgport", "pwd");

	anakinInput->setDependence("dbName", "pgport");
	anakinInput->setDependence("dbName", "pghost");
	anakinInput->setDependence("dbName", "login");
	anakinInput->setDependence("dbName", "pwd");

	anakinInput->setDependence("login", "pgport");
	anakinInput->setDependence("login", "pghost");
	anakinInput->setDependence("login", "dbName");
	anakinInput->setDependence("login", "pwd");

	anakinInput->setDependence("pwd", "pgport");
	anakinInput->setDependence("pwd", "pghost");
	anakinInput->setDependence("pwd", "dbName");
	anakinInput->setDependence("pwd", "login");

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
			cout << "Daemon arguments:" << endl << endl
					<< "[cacheLoadingTimeWeight <int>|cacheDiscardLessValuable <bool>|cacheSize <int>|cacheLife <int>|cacheScenesSize <int>|cacheScenesLife <int>|-oLogFile <path>|-threads <int>|-queueCapacity <int>|(pghost <name> pgport <port> dbName <name> login <user> pwd <password>)] (-iConsole|(-iHTTP <port>)) (-oConsole|-oHTTP)"
					<< endl << endl << "Flags:" << endl << endl
					<< "-iConsole/oConsole			: use console to input or output respectively"
					<< endl
					<< "-oLogFile <path>			: path to the output logging file. If it's ignored, logging will be skipped."
					<< endl
					<< "-iHTTP <port>/oHTTP			: use a HTTP connection with a port for input"
					<< endl
					<< "-pghost <name> 				: name of host to connect to. If this begins with a slash, it specifies Unix-domain communication rather than TCP/IP communication; the value is the name of the directory in which the socket file is stored. The default behavior when host is not specified is to connect to a Unix-domain socket in /tmp (or whatever socket directory was specified when PostgreSQL was built). On machines without Unix-domain sockets, the default is to connect to localhost. Empty string will use system environment defaults."
					<< endl
					<< "-pgport <port>				: port number to connect to at the server host, or socket file name extension for Unix-domain connections. Empty string will use system environment defaults."
					<< endl
					<< "-dbName <name>				: the database name. Defaults to be the same as the user name. Empty string will use system environment defaults."
					<< endl
					<< "-login <user>				: postgreSQL user name to connect as. Defaults to be the same as the operating system name of the user running the application. Empty string will use system environment defaults."
					<< endl
					<< "-pwd <password>				: password to be used if the server demands password authentication. Empty string will use system environment defaults."
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
			return EXIT_SUCCESS;
		}
		if (anakinInput->flagFound("verbose")) {
			verbose = true;
		}
		vector<string>* values = new vector<string>();

		//POSTGRES config

		if (anakinInput->flagFound("pghost")) {
			values->clear();
			values = anakinInput->getFlagValues("pghost");
			if (values->size() == 1) {
				pghost = values->at(0);
			} else {
				cout << "param pghost needs only one value" << endl;
				return EXIT_FAILURE;
			}
		}

		if (anakinInput->flagFound("pgport")) {
			values->clear();
			values = anakinInput->getFlagValues("pgport");
			if (values->size() == 1) {
				pgport = values->at(0);
			} else {
				cout << "param pgport needs only one value" << endl;
				return EXIT_FAILURE;
			}
		}

		if (anakinInput->flagFound("dbName")) {
			values->clear();
			values = anakinInput->getFlagValues("dbName");
			if (values->size() == 1) {
				dbName = values->at(0);
			} else {
				cout << "param dbName needs only one value" << endl;
				return EXIT_FAILURE;
			}
		}

		if (anakinInput->flagFound("login")) {
			values->clear();
			values = anakinInput->getFlagValues("login");
			if (values->size() == 1) {
				login = values->at(0);
			} else {
				cout << "param login needs only one value" << endl;
				return EXIT_FAILURE;
			}
		}

		if (anakinInput->flagFound("pwd")) {
			values->clear();
			values = anakinInput->getFlagValues("pwd");
			if (values->size() == 1) {
				pwd = values->at(0);
			} else {
				cout << "param pwd needs only one value" << endl;
				return EXIT_FAILURE;
			}
		}

		//CACHE CONFIG
		if (useCache) {
			if (anakinInput->flagFound("cacheLoadingTimeWeight")) {
				values->clear();
				values = anakinInput->getFlagValues("cacheLoadingTimeWeight");
				if (values->size() == 1) {
					cacheConfig.cacheLoadingTimeWeight = stoi(values->at(0));
				} else {
					cout << "param cacheLoadingTimeWeight needs only one value"
							<< endl;
					return EXIT_FAILURE;
				}
			}

			if (anakinInput->flagFound("cacheNoDiscardLessValuable")) {
				cacheConfig.cacheNoDiscardLessValuable = true;
			}

			if (anakinInput->flagFound("cacheSize")) {
				values->clear();
				values = anakinInput->getFlagValues("cacheSize");
				if (values->size() == 1) {
					cacheConfig.cacheSize = stoi(values->at(0));
				} else {
					cout << "param cacheSize needs only one value" << endl;
					return EXIT_FAILURE;
				}
			}

			if (anakinInput->flagFound("cacheLife")) {
				values->clear();
				values = anakinInput->getFlagValues("cacheLife");
				if (values->size() == 1) {
					cacheConfig.cacheLife = stoi(values->at(0));
				} else {
					cout << "param cacheLife needs only one value" << endl;
					return EXIT_FAILURE;
				}
			}

			if (anakinInput->flagFound("cacheScenesSize")) {
				values->clear();
				values = anakinInput->getFlagValues("cacheScenesSize");
				if (values->size() == 1) {
					cacheConfig.cacheScenesSize = stoi(values->at(0));
				} else {
					cout << "param cacheScenesSize needs only one value"
							<< endl;
					return EXIT_FAILURE;
				}
			}

			if (anakinInput->flagFound("cacheScenesLife")) {
				values->clear();
				values = anakinInput->getFlagValues("cacheScenesLife");
				if (values->size() == 1) {
					cacheConfig.cacheScenesLife = stoi(values->at(0));
				} else {
					cout << "param cacheScenesLife needs only one value"
							<< endl;
					return EXIT_FAILURE;
				}
			}
		}

		//THREADS
		if (anakinInput->flagFound("threads")) {
			values->clear();
			values = anakinInput->getFlagValues("threads");
			if (values->size() == 1) {
				threads = stoi(values->at(0));
			} else {
				cout << "param threads needs only one value" << endl;
				return EXIT_FAILURE;
			}
		}

		//BLOCKING CAPACITY QUEUE
		if (anakinInput->flagFound("queueCapacity")) {
			values->clear();
			values = anakinInput->getFlagValues("queueCapacity");
			if (values->size() == 1) {
				queueCapacity = stoi(values->at(0));
			} else {
				cout << "param queueCapacity needs only one value" << endl;
				return EXIT_FAILURE;
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
				httpPort = stoi(values->at(0));
			} else {
				cout << "param iHTTP needs only one value" << endl;
				return EXIT_FAILURE;
			}
		}

		//OUTPUT
		if (anakinInput->flagFound("oConsole")) {
			oMode = Server<SpecificCommandRunner>::CONSOLE;
		}
		if (anakinInput->flagFound("oLogFile")) {
			values->clear();
			values = anakinInput->getFlagValues("oLogFile");
			if (values->size() == 1) {
				logFile = values->at(0);
			} else {
				cout << "param oLogPath needs one value" << endl;
				return EXIT_FAILURE;
			}
		}

		if (anakinInput->flagFound("oHTTP")) {
			oMode = Server<SpecificCommandRunner>::HTTP;
		}

		delete values;
	} else {
		cout << "Input error!" << endl;
		return EXIT_FAILURE;
	}

	//______________________________________________________________________//
	//                         DAEMON INITIALIZATION                        //
	//______________________________________________________________________//

	//logger initialization
	if (!logFile.empty()) {
		Logging::OutputPolicyFile::SetFileStream(logFile);
	}

	std::string startComand;
	for (uint i = 0; i < input->size(); i++) {
		if (i != 0) {
			startComand.append(" ");
		}
		if (input->at(i).find(" ") != string::npos) {
			startComand.append("\"").append(input->at(i)).append("\"");
		} else {
			startComand.append(input->at(i));
		}
	}
	LOG_F("Args")<< startComand;

	HTTPSocket* httpSocket;

	if (!useCache) {
		pCacheConfig = NULL;
	}

	Server<SpecificCommandRunner>* server = new RequestServer<
			SpecificCommandRunner>(pCacheConfig, iMode, pghost, pgport, dbName,
			login, pwd, httpPort, queueCapacity, threads, verbose);

	DataOutput* output;
	if (oMode & Server<SpecificCommandRunner>::CONSOLE) {
		output = new DataOutput();
	} else if (oMode & Server<SpecificCommandRunner>::HTTP) {
		httpSocket = server->getHttpSocket();
		output = new DataOutput(httpSocket);
	} else {
		cerr << "unknown output mode \"" << oMode << "\"" << endl;
		return EXIT_FAILURE;
	}

	server->start(output);

	delete output;
	delete server;
	delete anakinInput;
}

template<class SpecificCommandRunner>
Daemon<SpecificCommandRunner>::~Daemon() {
}

}
;
/* namespace Anakin */

#endif /* DAEMON_H_ */
